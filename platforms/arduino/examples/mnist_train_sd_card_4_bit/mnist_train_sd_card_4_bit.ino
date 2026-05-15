/* ! Important                                         */
/* In Arduino/libraries/LiME-TM/src/tsetlin_model.h   */
/* Please uncomment the line below for model training */
/* // #define TSETLIN_MODEL_TRAINABLE                 */

#include <SPI.h>
#include <SD.h>

#include "mnist_model.h"
#include "lime_tm_mnist.h"

static const char* TAG = "main";

/* =========================
   ESP32 SD SPI PINS
   Change if needed
   ========================= */
#define SD_SCK   18
#define SD_MISO  19
#define SD_MOSI  23
#define SD_CS     5

// #define CONSOLE_USE_SERIAL
#define CONSOLE_USE_CDC
// #define CONSOLE_USE_RTT

// Print to Serial 1
#if defined(CONSOLE_USE_SERIAL)
HardwareSerial Serial1(PA10, PA9);
#define Console Serial1
#endif

// Print to USB CDC
#if defined(CONSOLE_USE_CDC)
#define Console Serial
#endif

// Print to ST-Link / CMSIS-DAP Debugger (RTT)
#if defined(CONSOLE_USE_RTT)
#include <RTTStream.h>
RTTStream rtt;
#define Console rtt
#endif

// Printf requries std library and _write implementation
extern "C" int _write(int file, char* ptr, int len) {
  (void)file;
  Console.write((uint8_t*)ptr, len);
  return len;
}

#define MNIST_TRAIN_IMG_PATH "/train-images-idx3-ubyte"
#define MNIST_TRAIN_LABEL_PATH "/train-labels-idx1-ubyte"

#define MNIST_TEST_IMG_PATH "/t10k-images-idx3-ubyte"
#define MNIST_TEST_LABEL_PATH "/t10k-labels-idx1-ubyte"

uint32_t N_EPOCHS = 10;
uint32_t T = 10;
float s = 7.5f;

// #if !defined(TSETLIN_USING_PROTOBUF)
// void tsetlin__unpack(Tsetlin* model, size_t size) {
//   // Do nothing
// }

// void tsetlin__free_unpacked(Tsetlin* model, void* allocator) {
//   // Do nothing
// }
// #endif

void printDirectory(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if (entry.isDirectory()) {
      LOGI(TAG, "/");
      printDirectory(entry);
    } else {
      // files have sizes, directories do not
      LOGI(TAG, "%s | %d Bytes", entry.name(), entry.size());
    }
    entry.close();
  }
}

int lime_tm_mnist_main() {
  // Step 0: Load Tsetlin model
  Tsetlin* model = NULL;

#if defined(TSETLIN_USING_PROTOBUF)
  model = lime_tm_mnist_load_model(DISK_MOUNT_PT "/tsetlin_model.cpb");
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

  LOGI(TAG, "n_class   = %u", model->n_class);
  LOGI(TAG, "n_feature = %u", model->n_feature);
  LOGI(TAG, "n_clause  = %u", model->n_clause);
  LOGI(TAG, "n_state   = %u", model->n_state);
  LOGI(TAG, "model_type = %u", model->model_type);
  LOGI(TAG, "");

  // Outputs for image dimensions
  int rows, cols;

  // Outputs for model evaluation
  uint8_t predicted_class = 0;
  int32_t* votes = (int32_t*)malloc(sizeof(int32_t) * model->n_class);
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
  int img_index = random(train_img_count);

  LOGI(TAG, "Loading and printing training image %d", img_index);

  File f_train_imgs = SD.open(MNIST_TRAIN_IMG_PATH);
  if (!f_train_imgs) {
    LOGE(TAG, "Failed to open file %s", MNIST_TRAIN_IMG_PATH);
    return -1;
  }

  File f_train_labels = SD.open(MNIST_TRAIN_LABEL_PATH);
  if (!f_train_labels) {
    LOGE(TAG, "Failed to open file %s", MNIST_TRAIN_LABEL_PATH);
    return -1;
  }

  mnist_print_info(f_train_imgs, f_train_labels, img_index, rows, cols);

  LOGI(TAG, "");

  // Step 4: Print a random mnist test image
  img_index = random(train_img_count);

  LOGI(TAG, "Loading and printing testing image %d", img_index);

  File f_test_imgs = SD.open(MNIST_TEST_IMG_PATH);
  if (!f_test_imgs) {
    LOGE(TAG, "Failed to open file %s", MNIST_TEST_IMG_PATH);
    return -1;
  }

  File f_test_labels = SD.open(MNIST_TEST_LABEL_PATH);
  if (!f_test_labels) {
    LOGE(TAG, "Failed to open file %s", MNIST_TEST_LABEL_PATH);
    return -1;
  }

  mnist_print_info(f_test_imgs, f_test_labels, img_index, rows, cols);

  LOGI(TAG, "");

  // Step 5: Evaluate model on a random test image
  img_index = random(test_img_count);

  uint8_t* img = mnist_load_image(f_test_imgs, img_index, rows, cols);
  if (!img) {
    LOGE(TAG, "Failed to load test image");

    // tsetlin__free_unpacked(model, NULL);
    return -1;
  }

  int8_t label = mnist_load_label(f_test_labels, img_index);
  if (label < 0) {
    LOGE(TAG, "Failed to load test label");

    // tsetlin__free_unpacked(model, NULL);
    free(img);
    return -1;
  }

  mnist_print_img(img);

  LOGI(TAG, "Evaluating model on test image %d (label %d)", img_index, label);

  int ret = lime_tm_mnist_inference(model, img, rows, cols, votes, &predicted_class);
  if (ret < 0) {
    LOGE(TAG, "Inference failed");
    free(img);

    // tsetlin__free_unpacked(model, NULL);
    return -1;
  }
  free(img);

  for (size_t i = 0; i < model->n_class; i++) {
    LOGI(TAG, "Class %d: %d votes", i, votes[i]);
  }
  LOGI(TAG, "Predicted class: %d with %d votes", predicted_class, votes[predicted_class]);

  LOGI(TAG, "");

  // Step 6: Evaluate on the entire test set
  lime_tm_mnist_test_all(model, f_test_imgs, f_test_labels, test_img_count, rows, cols, votes);

  // Step 7: Train the model on the training set
  if (model->model_type == MODEL_TYPE__INFERENCE) {
    LOGE(TAG, "Inference-only Model cannot be trained.");
    return -1;
  }

  for (size_t i = 0; i < N_EPOCHS; i++) {
    // Benchmark
    long total_fs_time = 0;
    long total_boolean_time = 0;
    long total_calc_time = 0;
  
    for (uint32_t j = 0; j < train_img_count; j++) {
      if (j == 0) {
        // Skip the header
        f_test_imgs.seek(16);
        f_test_labels.seek(8);
      }

      uint32_t start_utility = micros();
      uint8_t* X_img = mnist_load_image(f_train_imgs, j, rows, cols);
      if (!X_img) {
        LOGE(TAG, "Failed to load train image %d", j);
        continue;
      }

      int8_t y_target = mnist_load_label(f_train_labels, j);
      if (y_target < 0) {
        LOGE(TAG, "Failed to load train label %d", j);
        continue;
      }
      total_fs_time += (micros() - start_utility);
  
      // Booleanize image using threshold 75
      // mnist_booleanize_img(X_img, rows * cols, 75);

      // Booleanize image using 4-bit representation
      uint32_t start_boolean = micros();
      uint8_t* bool_img = mnist_booleanize_img_n_bit(X_img, rows, cols, MODEL_BITS);
      total_boolean_time += (micros() - start_boolean);

      uint32_t start_calc = micros();
      tsetlin_step(model, bool_img, y_target, T, s);
      total_calc_time += (micros() - start_calc);
      free(img);

      // Print progress every 1000 images
      if ((j + 1) % 1000 == 0) {
        char message[32];
        snprintf(message, sizeof(message), "Epoch %d: Processed %d/%d", i + 1, j + 1, train_img_count);
        print_progress(message, (j + 1) * 100 / train_img_count);
      }
    }

    printf("[FS] Achieved %d us/image\n", (int)(total_fs_time / train_img_count));
    printf("[BOOL] Achieved %d us/image\n", (int)(total_boolean_time / train_img_count));
    printf("[TM] Achieved %d us/image\n", (int)(total_calc_time / train_img_count));

    printf("\n");

    // Evaluate on test set after each epoch
    lime_tm_mnist_test_all(model, f_test_imgs, f_test_labels, test_img_count, rows, cols, votes);
  }

  free(votes);

  f_train_imgs.close();
  f_train_labels.close();
  f_test_imgs.close();
  f_test_labels.close();

  return 0;
}

void setup() {
  // Initialize Console
#if defined(CONSOLE_USE_SERIAL)
  // Print to Serial 1
  Serial1.begin(115200);
  while (!Serial1) { ; }
#elif defined(CONSOLE_USE_CDC)
  // Print to USB CDC
  Serial.begin(115200);
  while (!Serial) { ; }
#elif defined(CONSOLE_USE_RTT)
  // Print to ST-Link / CMSIS-DAP Debugger (RTT)
  rtt.blockUpBufferFull();
#endif

  LOGI(TAG, "Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    LOGE(TAG, "SD card initialization failed!");
    while (1) {
      delay(1000);
    }
  }
  LOGI(TAG, "SD card initialized.");

  // Print files on the SD card
  File root = SD.open("/");
  if (root) {
    printDirectory(root);
  } else {
    LOGI(TAG, "Could not open root");
  }
  root.close();
}

void loop() {
  int ret = lime_tm_mnist_main();

  if (ret < 0) {
    LOGE(TAG, "Error occured inside the minst example");
  }

  while (1) {
    delay(500);
  };
}
