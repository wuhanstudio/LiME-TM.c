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

int lime_tm_mnist_inference(Tsetlin* model, const uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class)
{
    // Booleanize image using 8-bit representation
    uint8_t* bool_img = mnist_booleanize_img_n_bit(img, rows, cols, MODEL_BITS);
    if (!bool_img) {
        LOGE(TAG, "Failed to booleanize image");
        return -1;
    }

    // Evaluate
    unsigned long start_time = micros();
    tsetlin_evaluate(model, bool_img, votes, predicted_class);
    unsigned long inference_time = micros() - start_time;
    LOGI(TAG, "Inference time %lu us", inference_time);

    return 0;
}
