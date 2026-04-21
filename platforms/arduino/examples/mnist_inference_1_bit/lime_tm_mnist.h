#include <tsetlin.h>
#include "mnist.h"

#define MODEL_BITS 1

void print_progress(const char *label, int percent);

int lime_tm_mnist_inference(Tsetlin* model, uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class);
