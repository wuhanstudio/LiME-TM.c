# LiME-TM

>  A Lightning Fast and Memory Efficient Machine Learning Model for Training on MCUs

## Supported Platforms

- [Windows / Linux](platforms/win_linux)
- [Zephyr RTOS](zephyr)
- [RT-Thread RTOS](rt-thread)
- [ESP-IDF (FreeRTOS)](esp-idf)
- [Arduino](arduino)

## Quick Start (PC)

```
git clone --recursive https://github.com/wuhanstudio/LiME-TM.c
```

```
cd platforms/win_linux
```

For Linux:

```
cmake -S . -B build
cmake --build build
```

For Windows:

```
cmake -S . -B build -G "Visual Studio 17 2022"
```

## API Examples

### Load the model

Load the model from file system (#define TSETLIN_USING_PROTOBUF):

```
Tsetlin* model = NULL;
model = lime_tm_mnist_load_model(TSETLIN_MODEL_PATH);
```

Alternatively, the model can be compiled as a C header file (#define TSETLIN_USING_STATIC_MODEL):

```
#include "mnist_model.h"

Tsetlin* model = NULL;
model = &tsetlin_model;
```

Print model information:

```
LOGI(TAG, "n_class   = %d", model->n_class);
LOGI(TAG, "n_feature = %d", model->n_feature);
LOGI(TAG, "n_clause  = %d", model->n_clause);
LOGI(TAG, "n_state   = %d", model->n_state);
LOGI(TAG, "model_type = %d", model->model_type);
```

### Evaluation:

```
// Outputs for model evaluation
uint8_t predicted_class = 0;
int32_t votes[10];

// Booleanize image using 8-bit representation
uint8_t* bool_img = mnist_booleanize_img_n_bit(img, rows, cols, 8);

// Evaluate
tsetlin_evaluate(model, bool_img, votes, predicted_class);
```

### Training:

```
for (size_t i = 0; i < N_EPOCHS; i++)
{
    for (uint32_t j = 0; j < train_img_count; j++)
    {
        // Booleanize image using 8-bit representation
        uint8_t* bool_img = mnist_booleanize_img_n_bit(X_img, rows, cols, 8);

        tsetlin_step(model, bool_img, y_target, T, s);
        free(bool_img);
    }
}
```
