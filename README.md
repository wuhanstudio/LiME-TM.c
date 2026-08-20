# LiME-TM

>  A Lightning Fast and Memory Efficient Machine Learning Model for Training on MCUs

| MNIST Dataset | 2D-CNN | Tsetlin Machine (TM) |
|---|---:|---:|
| **Tested on** | ESP32 | ESP32 |
| Accuracy | 95.04% | 93.86% |
| Training | 114.2 ms | 0.37 ms |
| Inference | 24.6 ms | 1.20 ms |
| Operations | Convolution | & + |
| Batch Size | 8 | 1 |
| Optimizer | Gradient Descent | Reinforcement Feedback |




## Supported Platforms

- [Windows / Linux](platforms/win_linux)
- [Zephyr RTOS](platforms/zephyr)
- [RT-Thread RTOS](platforms/rt-thread)
- [ESP-IDF (FreeRTOS)](platforms/esp-idf)
- [Arduino](platforms/arduino)

> [!NOTE]
> Please refer to the corresponding platform directory for build and usage instructions.

## API Examples

### Load the model

#### Option 1 : Load the model from file system (Protobuf)

```
#define TSETLIN_USING_PROTOBUF

#if defined(TSETLIN_USING_PROTOBUF)
Tsetlin* lime_tm_mnist_load_model(const char* model_path) 
{
    // Load Tsetlin model from file
    size_t size = 0;
    uint8_t* data = tsetlin_read_file(model_path, &size);
    if (!data) {
        LOGE(TAG, "Failed to read file");
        return NULL;
    }

    LOGI(TAG, "Model loaded (%ld Bytes)", size);

    Tsetlin* model = tsetlin__unpack(NULL, size, data);
    free(data);

    return model;
}
#endif
```

```
Tsetlin* model = NULL;
model = lime_tm_mnist_load_model(TSETLIN_MODEL_PATH);
```

#### Option 2: the model can be compiled as a C header file:

```
#define TSETLIN_USING_STATIC_MODEL

#include "mnist_model.h"

Tsetlin* model = NULL;
model = &tsetlin_model;
```

#### Print model information:

```
LOGI(TAG, "n_class   = %d", model->n_class);
LOGI(TAG, "n_feature = %d", model->n_feature);
LOGI(TAG, "n_clause  = %d", model->n_clause);
LOGI(TAG, "n_state   = %d", model->n_state);
LOGI(TAG, "model_type = %d", model->model_type);
```

### Model Evaluation:

```
// Outputs for model evaluation
uint8_t predicted_class = 0;
int32_t votes[10];

// Booleanize image using 8-bit representation
uint8_t* bool_img = mnist_booleanize_img_n_bit(img, rows, cols, 8);

// Evaluate
tsetlin_evaluate(model, bool_img, votes, predicted_class);
```

### Model Training:

```
#define N_EPOCHS 10
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
