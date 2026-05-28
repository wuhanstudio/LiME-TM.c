#include <time.h>
#include <stdint.h>

#include <mnist.h>
#include <tsetlin.h>

#ifdef _WIN32
    #include <windows.h>

    uint64_t get_tick_ms(void)
    {
        return GetTickCount64();
    }
#else
    uint64_t get_tick_ms(void)
    {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    }
#endif

#define DISK_MOUNT_PT "./mnist"
static const char *TAG = "main";

#define MNIST_TRAIN_IMG_PATH DISK_MOUNT_PT"/train-images-idx3-ubyte"
#define MNIST_TRAIN_LABEL_PATH DISK_MOUNT_PT"/train-labels-idx1-ubyte"

#define MNIST_TEST_IMG_PATH DISK_MOUNT_PT"/t10k-images-idx3-ubyte"
#define MNIST_TEST_LABEL_PATH DISK_MOUNT_PT"/t10k-labels-idx1-ubyte"

#define TSETLIN_MODEL_PATH DISK_MOUNT_PT"/tsetlin_model_8_bit.cpb"
//#define TSETLIN_MODEL_PATH DISK_MOUNT_PT"/tsetlin_model_8_bit.ipb"

uint32_t N_EPOCHS = 10;
uint32_t T = 10;
float    s = 7.5f;

#if !defined(TSETLIN_USING_PROTOBUF)
static inline void tsetlin__unpack(Tsetlin* model, size_t size) 
{
    // Do nothing
}

static inline void tsetlin__free_unpacked(Tsetlin* model, void* allocator)
{
    // Do nothing
}
#endif

#if defined(TSETLIN_USING_PROTOBUF)
Tsetlin* lime_tm_mnist_load_model(const char* model_path) 
{
    // Load Tsetlin model from file
    size_t size = 0;
    uint8_t* data = tsetlin_read_file(model_path, &size);
    if (!data) {
        LOGE(TAG, "Failed to read file");
        return NULL;
    }

    LOGI(TAG, "Model loaded (%ld Bytes)", size);

    Tsetlin* model = tsetlin__unpack(NULL, size, data);
    free(data);

    return model;
}
#endif

void print_progress(const char *label, int percent) 
{
    const int bar_width = 40;
    int filled = percent * bar_width / 100;

    printf("%s [", label);
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%%\r", percent);  // stay on same line
    fflush(stdout);
}

uint32_t mnist_get_info(const char* image_path, const char* label_path, int* out_rows, int* out_cols) 
{
    // Get dataset info
    uint32_t train_img_count = mnist_image_info(image_path, out_rows, out_cols);
    uint32_t train_label_count = mnist_label_info(label_path);

    if (train_img_count != train_label_count) {
        LOGE(TAG, "Image count and label count do not match!");
        return 0;
    }

    return train_img_count;
}

void mnist_print_info(FILE* f_train_imgs, FILE* f_train_labels, int img_index, int rows, int cols)
{
    uint8_t* train_img = mnist_load_image(f_train_imgs, img_index, rows, cols);
    if (!train_img) 
    {
        LOGE(TAG, "Failed to load the image");
        return;
    }

    mnist_print_img(train_img);
    free(train_img);

    int8_t train_label = mnist_load_label(f_train_labels, img_index);
    LOGI(TAG, "Image label: %d", train_label);

    return;
}

int lime_tm_mnist_inference(Tsetlin* model, uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class)
{
    // Booleanize image using threshold 75
    // mnist_booleanize_img(img, rows * cols, 75);

    // Booleanize image using 8-bit representation
    uint8_t* bool_img = mnist_booleanize_img_n_bit(img, rows, cols, 8);
    if (!bool_img) {
        LOGE(TAG, "Failed to booleanize image");
        return -1;
    }

    // Evaluate
    tsetlin_evaluate(model, bool_img, votes, predicted_class);
    free(bool_img);

    return 0;
}

int lime_tm_mnist_test_all(Tsetlin* model, FILE* f_test_imgs, FILE* f_test_labels, int test_img_count, int rows, int cols, int32_t* votes)
{
    int correct = 0;

    long total_utility_time = 0;
    long total_calc_time = 0;

    for (uint32_t i = 0; i < test_img_count; i++)
    {
        uint64_t start_utility = get_tick_ms();

        if( i == 0) {
            // Skip the header
            fseek(f_test_imgs, 16, SEEK_SET);
            fseek(f_test_labels, 8, SEEK_SET);
        }

        uint8_t* img = mnist_load_next_image(f_test_imgs, i, rows, cols);
        if (!img) {
            LOGE(TAG, "Failed to load test image %d", i);
            continue;
        }

        int8_t label = mnist_load_next_label(f_test_labels, i);
        if (label < 0) {
            LOGE(TAG, "Failed to load test label %d", i);
            free(img);
            continue;
        }

        total_utility_time += (get_tick_ms() - start_utility);

        uint64_t start = get_tick_ms();

        uint8_t predicted_class = 0;
        int ret = lime_tm_mnist_inference(model, img, rows, cols, votes, &predicted_class);
        if (ret < 0) {
            LOGE(TAG, "Inference failed on test image %d", i);
            continue;
        }

        if (predicted_class == label) {
            correct++;
        }

        total_calc_time += (get_tick_ms() - start);

        // Print progress every 1000 images
        if ((i + 1) % 1000 == 0) {
            char message[32];
            snprintf(message, sizeof(message), "Testing %d/%d", i + 1, test_img_count);
            print_progress(message, (i + 1) * 100 / test_img_count);
        }
    }

    printf("\n");

    float uts = (double)(total_utility_time) / test_img_count;
    printf("[FS] Achieved %.2f ms/image\n", uts);

    float tks = (double)(total_calc_time) / test_img_count;
    printf("[TM] Achieved %.2f ms/image\n", tks);

    printf("Accuracy on test set (%d): %.2f%% \n", test_img_count, (double)correct / test_img_count * 100);

}

int main(int argc, char* argv[]) {

    // Step 0: Load Tsetlin model
    Tsetlin* model = NULL;

    #if defined(TSETLIN_USING_PROTOBUF)
        model = lime_tm_mnist_load_model(TSETLIN_MODEL_PATH);
    #elif defined(TSETLIN_USING_STATIC_MODEL)
        model = &tsetlin_model;
    #else
        LOGE(TAG, "No model loading method defined!");
        return -1;
    #endif

    if (!model) {
        LOGE(TAG, "Failed to load model");
        return -1;
    }

    LOGI(TAG, "");
    LOGI(TAG, "Model loaded successfully");

    LOGI(TAG, "n_class   = %d", model->n_class);
    LOGI(TAG, "n_feature = %d", model->n_feature);
    LOGI(TAG, "n_clause  = %d", model->n_clause);
    LOGI(TAG, "n_state   = %d", model->n_state);
    LOGI(TAG, "model_type = %d", model->model_type);
    LOGI(TAG, "");

    // Outputs for image dimensions
    int rows, cols;

    // Outputs for model evaluation
    uint8_t predicted_class = 0;
    int32_t* votes = malloc(sizeof(int32_t) * model->n_class);
    if (!votes) {
        LOGE(TAG, "Failed to allocate memory for votes");
        free(votes);

        return -1;
    }

    LOGI(TAG, "");

    // Step 1: Print mnist training set info
    uint32_t train_img_count = mnist_get_info(MNIST_TRAIN_IMG_PATH, MNIST_TRAIN_LABEL_PATH, &rows, &cols);
    LOGI(TAG, "MNIST training set: %d images of size %dx%d", train_img_count, rows, cols);

    if (train_img_count == 0) {
        LOGE(TAG, "No images found in training dataset!");
        return -1;
    }

    // Step 2: Print mnist testing set info
    uint32_t test_img_count = mnist_get_info(MNIST_TEST_IMG_PATH, MNIST_TEST_LABEL_PATH, &rows, &cols);
    LOGI(TAG, "MNIST testing set: %d images of size %dx%d", test_img_count, rows, cols);

    if (test_img_count == 0) {
        LOGE(TAG, "No images found in testing dataset!");
        return -1;
    }

    LOGI(TAG, "");

        // Step 3: Print a random mnist train image

    int img_index = fast_rand() % train_img_count;
    LOGI(TAG, "Loading and printing training image %d", img_index);

    FILE* f_train_imgs = fopen(MNIST_TRAIN_IMG_PATH, "rb");
    if (!f_train_imgs) {
        LOGE(TAG, "Failed to open file %s", MNIST_TRAIN_IMG_PATH);
        return -1;
    }

    FILE *f_train_labels = fopen(MNIST_TRAIN_LABEL_PATH, "rb");
    if (!f_train_labels) {
        LOGE(TAG, "Failed to open file %s", MNIST_TRAIN_LABEL_PATH);
        return -1;
    }

    mnist_print_info(f_train_imgs, f_train_labels, img_index, rows, cols);

    LOGI(TAG, "");

    // Step 4: Print a random mnist test image
    img_index = fast_rand() % test_img_count;
    LOGI(TAG, "Loading and printing testing image %d", img_index);

    FILE* f_test_imgs = fopen(MNIST_TEST_IMG_PATH, "rb");
    if (!f_test_imgs) {
        LOGE(TAG, "Failed to open file %s", MNIST_TEST_IMG_PATH);
        return -1;
    }

    FILE* f_test_labels = fopen(MNIST_TEST_LABEL_PATH, "rb");
    if (!f_test_labels) {
        LOGE(TAG, "Failed to open file %s", MNIST_TEST_LABEL_PATH);
        return -1;
    }

    mnist_print_info(f_test_imgs, f_test_labels, img_index, rows, cols);

    LOGI(TAG, "");

    // Step 5: Evaluate model on a random test image
    img_index = fast_rand() % test_img_count;

    uint8_t* img = mnist_load_image(f_test_imgs, img_index, rows, cols);
    if (!img) {
        LOGE(TAG, "Failed to load test image");

        tsetlin__free_unpacked(model, NULL);
        return -1;
    }

    int8_t label = mnist_load_label(f_test_labels, img_index);
    if (label < 0) {
        LOGE(TAG, "Failed to load test label");

        tsetlin__free_unpacked(model, NULL);
        free(img);
        return -1;
    }

    mnist_print_img(img);

    LOGI(TAG, "Evaluating model on test image %d (label %d)", img_index, label);

    int ret = lime_tm_mnist_inference(model, img, rows, cols, votes, &predicted_class);
    if (ret < 0) {
        LOGE(TAG, "Inference failed");

        tsetlin__free_unpacked(model, NULL);
        return -1;
    }

    for (size_t i = 0; i < model->n_class; i++)
    {
        LOGI(TAG, "Class %ld: %d votes", i, votes[i]);
    }
    LOGI(TAG, "Predicted class: %d with %d votes", predicted_class, votes[predicted_class]);

    LOGI(TAG, "");

    // Step 6: Evaluate on the entire test set
    lime_tm_mnist_test_all(model, f_test_imgs, f_test_labels, test_img_count, rows, cols, votes);

    LOGI(TAG, "");

    // Step 7: Train the model on the training set
	if (model->model_type == MODEL_TYPE__INFERENCE) 
    {
        LOGE(TAG, "Inference-only Model cannot be trained."); 
        return -1; 
    }

    for (size_t i = 0; i < N_EPOCHS; i++)
    {
        for (uint32_t j = 0; j < train_img_count; j++)
        {
            if( j == 0) {
                fseek(f_train_imgs, 16, SEEK_SET);
                fseek(f_train_labels, 8, SEEK_SET);
            }

            uint8_t* X_img = mnist_load_next_image(f_train_imgs, j, rows, cols);
            if (!X_img) {
                LOGE(TAG, "Failed to load train image %d", j);
                continue;
            }

            int8_t y_target = mnist_load_next_label(f_train_labels, j);
            if (y_target < 0) {
                LOGE(TAG, "Failed to load train label %d", j);
                continue;
            }

            // Booleanize image using threshold 75
            // mnist_booleanize_img(X_img, rows * cols, 75);

            // Booleanize image using 8-bit representation
            uint8_t* bool_img = mnist_booleanize_img_n_bit(X_img, rows, cols, 8);

            tsetlin_step(model, bool_img, y_target, T, s);
            free(bool_img);

            // Print progress every 1000 images
            if ((j + 1) % 1000 == 0) {
                char message[32];
                snprintf(message, sizeof(message), "Epoch %ld: Processed %d/%d", i + 1, j + 1, train_img_count);
                print_progress(message, (j + 1) * 100 / train_img_count);
            }
        }

        printf("\n");

        // Evaluate on test set after each epoch
        lime_tm_mnist_test_all(model, f_test_imgs, f_test_labels, test_img_count, rows, cols, votes);
    }

#if defined(TSETLIN_USING_PROTOBUF)
    // free protobuf
    tsetlin__free_unpacked(model, NULL);
#endif

    fclose(f_train_imgs);
    fclose(f_test_imgs);

    fclose(f_train_labels);
    fclose(f_test_labels);

    return 0;
}
