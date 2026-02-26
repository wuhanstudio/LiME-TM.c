#include "mnist.h"

#if defined(__ZEPHYR__)
    LOG_MODULE_REGISTER(mnist);
#endif

static const char *TAG = "mnist";

#define MNIST_X_MEAN 33.318f
#define MNIST_X_STD 78.567f

static uint8_t img[ 28 * 28];
static uint8_t bool_img[ 28 * 28 * MODEL_BITS];

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

static uint32_t read_u32_be(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  |
           (uint32_t)p[3];
}

static inline float norm_cdf(float x) {
    return 0.5f * (1.0f + erff(x / 1.41421356237f)); // sqrt(2)
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

uint32_t mnist_image_info(const char* path, int* out_rows, int* out_cols)
{
    File f = SD.open(path);
    if (!f) {
        LOGE(TAG, "Failed to open file %s", path);
        return 0;
    }

    uint8_t header[16];

    if (f.read(header, 16) != 16) {
        f.close();
        LOGE(TAG, "Failed to read header from file %s", path);
        return 0;
    }

    uint32_t magic      = read_u32_be(&header[0]);
    uint32_t num_images = read_u32_be(&header[4]);
    uint32_t rows       = read_u32_be(&header[8]);
    uint32_t cols       = read_u32_be(&header[12]);

    if (magic != 0x00000803) {
        f.close();
        LOGE(TAG, "Invalid magic number in file %s", path);
        return 0;
    }

    *out_rows = (int)rows;
    *out_cols = (int)cols;

    f.close();
    return num_images;
}

uint8_t* mnist_load_image(File f, int idx, int rows, int cols)
{
    size_t total = (size_t) rows * cols;

    // Seek to the start of the image
    if (!f.seek(16 + (size_t)idx * total)) {
        LOGE(TAG, "Failed to seek to image %d", idx);
        return nullptr;
    }

    // Read the image data
    if (f.read(img, total) != total) {
        LOGE(TAG, "Failed to read image %d", idx);
        return nullptr;
    }

    return img;
}

uint8_t* mnist_load_next_image(File f, int idx, int rows, int cols) {
    size_t total = (size_t) rows * cols;

    if (f.read(img, total) != total) {
        LOGE(TAG, "Failed to read %lu bytes data", total);
        return NULL;
    }

    return img;
}

uint32_t mnist_label_info(const char* path)
{
    // Open file from SD card
    File f = SD.open(path, FILE_READ);
    if (!f) {
        LOGE(TAG, "Failed to open file %s", path);
        return 0;
    }

    uint8_t header[8];

    // Read 8-byte header
    if (f.read(header, 8) != 8) {
        f.close();
        LOGE(TAG, "Failed to read header from file %s", path);
        return 0;
    }

    uint32_t magic      = read_u32_be(&header[0]);
    uint32_t num_labels = read_u32_be(&header[4]);

    // Check MNIST magic number for labels (0x00000801)
    if (magic != 0x00000801) {
        f.close();
        LOGE(TAG, "Invalid magic number in file %s", path);
        return 0;
    }

    f.close();
    return num_labels;
}

int8_t mnist_load_label(File f, int idx)
{
    // Seek to the label at position 8 + idx
    if (!f.seek(8 + (size_t)idx)) {
        LOGE(TAG, "Failed to seek to label %d", idx);
        return -1;
    }

    uint8_t label;
    // Read one byte
    if (f.read(&label, 1) != 1) {
        LOGE(TAG, "Failed to read label %d", idx);
        return -1;
    }

    return (int8_t)label;
}

int8_t mnist_load_next_label(File f, int idx) {
    uint8_t label;
    if (f.read(&label, 1) != 1) { return -1; }

    return label;
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
