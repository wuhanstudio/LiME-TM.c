#include <tsetlin.h>
#include "mnist.h"

void print_progress(const char *label, int percent);

uint32_t mnist_get_info(const char* image_path, const char* label_path, int* out_rows, int* out_cols);
void mnist_print_info(File f_train_imgs, File f_train_labels, int img_index, int rows, int cols);

int lime_tm_mnist_inference(Tsetlin* model, uint8_t* img, int rows, int cols, int32_t* votes, uint8_t* predicted_class);
int lime_tm_mnist_test_all(Tsetlin* model, File f_test_imgs, File f_test_labels, int test_img_count, int rows, int cols, int32_t* votes);
