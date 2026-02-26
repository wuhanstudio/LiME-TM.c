#include "mnist.h"

#if defined(__ZEPHYR__)
    LOG_MODULE_REGISTER(mnist);
#endif

static const char *TAG = "mnist";

#define MNIST_X_MEAN 33.318f
#define MNIST_X_STD 78.567f

static uint8_t bool_img[28 * 28 * MODEL_BITS];

#ifdef __AVR__
float erff(float x) {
    // Abramowitz & Stegun approximation
    float t = 1.0f / (1.0f + 0.5f * fabsf(x));
    float tau = t * expf(-x*x - 1.26551223f +
                         t*(1.00002368f +
                         t*(0.37409196f +
                         t*(0.09678418f +
                         t*(-0.18628806f +
                         t*(0.27886807f +
                         t*(-1.13520398f +
                         t*(1.48851587f +
                         t*(-0.82215223f +
                         t*0.17087277f)))))))));
    return (x >= 0) ? 1.0f - tau : tau - 1.0f;
}
#endif

static inline float norm_cdf(float x) {
    return 0.5f * (1.0f + erff(x / 1.41421356237f)); // sqrt(2)
}

static uint32_t read_u32_be(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           (uint32_t)p[3];
}

static float* mnist_int_to_float(const uint8_t *src, int rows, int cols) {
    float *dst = (float*) malloc(rows * cols * sizeof(float));
    if (!dst)
    {
        LOGE(TAG, "Failed to allocate memory for float image");
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dst[i * cols + j] = (float) src[i * cols + j];
        }
    }

    return dst;
}

static void mnist_normalize_img(float* X, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            X[i * cols + j] = (X[i * cols + j] - MNIST_X_MEAN) / MNIST_X_STD;
            X[i * cols + j] = norm_cdf(X[i * cols + j]);
        }
    }
}

static int mnist_booleanize_n_bit(float x, int num_bits, uint8_t *out_bits) {
    if (x < 0.0f || x > 1.0f)
        return -1;

    if (!(num_bits == 1 || num_bits == 2 || num_bits == 4 || num_bits == 8))
        return -2;

    int max_val = (1 << num_bits) - 1;

    /* round-to-nearest-even */
    int int_val = (int) lrintf(x * max_val);

    for (int i = 0; i < num_bits; i++) {
        out_bits[i] = (int_val >> (num_bits - 1 - i)) & 1;
    }

    return 0;
}

static uint8_t* mnist_booleanize_features(
    float* X,
    int rows,
    int cols,
    int num_bits
) {
    uint8_t* X_bool = (uint8_t*) malloc(rows * cols * num_bits * sizeof(uint8_t));
    if(!X_bool) {
        LOGE(TAG, "Failed to allocate memory for booleanized features");
        return NULL;
    }

    int offset = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mnist_booleanize_n_bit(X[i * cols + j], num_bits, &X_bool[offset]);
            offset += num_bits;
        }
    }
    return X_bool;
}

uint8_t* mnist_booleanize_img_n_bit(
    const uint8_t* img,
    int rows,
    int cols,
    int num_bits
)
{
    int total_pixels = rows * cols;
    int max_val = (1 << num_bits) - 1;
    int offset = 0;

    for (int i = 0; i < total_pixels; i++)
    {
        /* Normalize directly */
        float x = (float)img[i];
        x = (x - MNIST_X_MEAN) / MNIST_X_STD;
        x = norm_cdf(x);

        /* Clamp for safety */
        if (x < 0.0f) x = 0.0f;
        if (x > 1.0f) x = 1.0f;

        int int_val = (int)(x * max_val + 0.5f);

        /* Expand into num_bits separate bytes */
        for (int b = 0; b < num_bits; b++)
        {
            bool_img[offset++] =
                (int_val >> (num_bits - 1 - b)) & 1;
        }
    }

    return bool_img;
}

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
