#include <tsetlin.h>
#include "mnist_model.h"
#include "lime_tm_mnist.h"

static const char* TAG = "main";

// #define CONSOLE_USE_SERIAL
#define CONSOLE_USE_CDC
// #define CONSOLE_USE_RTT

// Print to Serial 1
#if defined(CONSOLE_USE_SERIAL)

#ifndef HAVE_HWSERIAL1
HardwareSerial Serial1(PA10, PA9);
#endif

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

#define MNIST_TEST_SAMPLES \
    (sizeof(mnist_test_images) / sizeof(mnist_test_images[0]))

#ifdef __AVR__
FILE f_out;
int sput(char c, __attribute__((unused)) FILE* f) {
  return !Console.write(c);
}
#endif

// Printf requries std library and _write implementation
extern "C" int _write(int file, char* ptr, int len) {
  (void)file;
  Console.write((uint8_t*)ptr, len);
  return len;
}

int lime_tm_mnist_main() {
  // Step 0: Load Tsetlin model
  Tsetlin* model = &tsetlin_model;

  LOGI(TAG, "n_class   = %u", model->n_class);
  LOGI(TAG, "n_feature = %u", model->n_feature);
  LOGI(TAG, "n_clause  = %u", model->n_clause);
  LOGI(TAG, "n_state   = %u", model->n_state);
  LOGI(TAG, "model_type = %u", model->model_type);
  LOGI(TAG, "");

  // Outputs for image dimensions
  const int rows = 28, cols = 28;

  // Outputs for model evaluation
  uint8_t predicted_class = 0;
  int32_t votes[10];

  // Step 1: Evaluate model on testing images
  for (size_t i = 0; i < MNIST_TEST_SAMPLES; i++) {
    uint8_t* img = mnist_test_images[i];

    mnist_print_img(img);
    LOGI(TAG, "Evaluating model on test image %d (label %d)", i, mnist_test_labels[i]);

    int ret = lime_tm_mnist_inference(model, img, rows, cols, votes, &predicted_class);
    if (ret < 0) {
      return -1;
    }

    for (size_t i = 0; i < model->n_class; i++) {
      LOGI(TAG, "Class %d: %d votes", i, votes[i]);
    }
    LOGI(TAG, "Predicted class: %d with %d votes", predicted_class, votes[predicted_class]);
  }

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
#ifdef __AVR__
  fdev_setup_stream(&f_out, sput, nullptr, _FDEV_SETUP_WRITE);  // cf https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaf41f158c022cbb6203ccd87d27301226
  stdout = &f_out;
#endif
  while (!Serial) { ; }
#elif defined(CONSOLE_USE_RTT)
  // Print to ST-Link / CMSIS-DAP Debugger (RTT)
  rtt.blockUpBufferFull();
#endif
}

void loop() {
  int ret = lime_tm_mnist_main();

  if (ret < 0) {
    LOGE(TAG, "Inference Failed.");
  }

  while (1) {
    delay(500);
  };
}
