#include "mnist_model.h"
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

int lime_tm_mnist_inference(Tsetlin* model, uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class)
{
    // Booleanize image using a threshold
    mnist_booleanize_img(img, rows * cols, 75);

    // Evaluate
    tsetlin_evaluate(model, img, votes, predicted_class);

    return 0;
}
