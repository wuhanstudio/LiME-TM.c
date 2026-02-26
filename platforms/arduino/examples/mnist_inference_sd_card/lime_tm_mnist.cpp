#include "lime_tm_mnist.h"

static const char* TAG = "main";

void print_progress(const char *label, int percent) 
{
    const int bar_width = 40;
    int filled = percent * bar_width / 100;

    printf("%s [", label);
    for (int i = 0; i < bar_width; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %3d%%\n", percent);  // stay on same line
    // fflush(stdout);
}

uint32_t mnist_get_info(const char* image_path, const char* label_path, int* out_rows, int* out_cols) {
  // Get dataset info
  uint32_t train_img_count = mnist_image_info(image_path, out_rows, out_cols);
  uint32_t train_label_count = mnist_label_info(label_path);

  if (train_img_count != train_label_count) {
    LOGE(TAG, "Image count and label count do not match!");
    return 0;
  }

  return train_img_count;
}

void mnist_print_info(File f_train_imgs, File f_train_labels, int img_index, int rows, int cols) {
  uint8_t* train_img = mnist_load_image(f_train_imgs, img_index, rows, cols);
  if (!train_img) {
    LOGE(TAG, "Failed to load the image");
    return;
  }

  mnist_print_img(train_img);

  int8_t train_label = mnist_load_label(f_train_labels, img_index);
  LOGI(TAG, "Image label: %d", train_label);

  return;
}

int lime_tm_mnist_inference(Tsetlin* model, uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class)
{
    // Booleanize image using threshold 75
    // mnist_booleanize_img(img, rows * cols, 75);

    // Booleanize image using 8-bit representation
    uint8_t* bool_img = mnist_booleanize_img_n_bit(img, rows, cols, MODEL_BITS);
    if (!bool_img) {
        LOGE(TAG, "Failed to booleanize image");
        return -1;
    }

    // Evaluate
    tsetlin_evaluate(model, bool_img, votes, predicted_class);

    return 0;
}

int lime_tm_mnist_test_all(Tsetlin* model, File f_test_imgs, File f_test_labels, int test_img_count, int rows, int cols, int32_t* votes)
{
    int correct = 0;

    long total_utility_time = 0;
    long total_calc_time = 0;

    for (uint32_t i = 0; i < test_img_count; i++)
    {
        uint32_t start_utility = millis();

        if( i == 0) {
            // Skip the header
            f_test_imgs.seek(16);   // image file header
            f_test_labels.seek(8);  // label file header
        }

        uint8_t* img = mnist_load_next_image(f_test_imgs, i, rows, cols);
        if (!img) {
            LOGE(TAG, "Failed to load test image %d", i);
            continue;
        }

        int8_t label = mnist_load_next_label(f_test_labels, i);
        if (label < 0) {
            LOGE(TAG, "Failed to load test label %d", i);
            continue;
        }

        total_utility_time += (millis() - start_utility);

        uint32_t start = millis();

        uint8_t predicted_class = 0;

        int ret = lime_tm_mnist_inference(model, img, rows, cols, votes, &predicted_class);
        if (ret < 0) {
            LOGE(TAG, "Inference failed on test image %d", i);
            continue;
        }

        if (predicted_class == label) {
            correct++;
        }

        total_calc_time += (millis() - start);

        // Print progress every 1000 images
        if ((i + 1) % 1000 == 0) {
            char message[32];
            snprintf(message, sizeof(message), "Testing %d/%d", i + 1, test_img_count);
            print_progress(message, (i + 1) * 100 / test_img_count);
        }
    }

    printf("\n");

    float tks = test_img_count / (double)(total_calc_time) * 1000;
    printf("[TM] Achieved images/s: %d\n", (int)tks);

    float uts = test_img_count / (double)(total_utility_time) * 1000;
    printf("[FS] Achieved images/s: %d\n", (int)uts);

    printf("Correct predictions on test set %d / %d\n", (int) correct, (int) test_img_count);

    return 0;
}
