#include "mnist.h"

#if defined(__ZEPHYR__)
    LOG_MODULE_REGISTER(mnist);
#endif

static const char *TAG = "mnist";

#define MNIST_X_MEAN 33.318f
#define MNIST_X_STD 78.567f

void mnist_booleanize_img(uint8_t* img, uint32_t size, uint8_t threshold) {
    for (uint32_t i = 0; i < size; i++) {
        img[i] = (img[i] > threshold) ? 1 : 0;
    }
}

// ASCII lib from (https://www.jianshu.com/p/1f58a0ebf5d9)
static const char codeLib[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'.   ";
void mnist_print_img(const uint8_t* buf)
{
    for(int y = 0; y < 28; y++) 
    {
        for (int x = 0; x < 28; x++) 
        {
            int index = 0; 
            if(buf[y*28+x] > 75) index =69;
            if(index < 0) index = 0;
                printf("%c",codeLib[index]);
                printf("%c",codeLib[index]);
        }
        printf("\n");
    }
}
