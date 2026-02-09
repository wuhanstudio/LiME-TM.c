
#ifndef CLAUSE_H
#define CLAUSE_H

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/unistd.h>
#endif

#if defined(__ZEPHYR__)
  /* Zephyr RTOS */
  #include <fast_rand.h>
#elif defined(ESP_PLATFORM)
  /* ESP-IDF */
  #include <esp_random.h>
#elif defined(__RTTHREAD__)
  /* RT-Thread RTOS */
  #include <rtthread.h>
  #include <fast_rand.h>
#else
  /* POSIX */
  #include <fast_rand.h>
#endif

#if defined(TSETLIN_USING_PROTOBUF)
  #include <tsetlin.pb-c.h>
#elif defined(TSETLIN_USING_STATIC_MODEL)
  #include <tsetlin_model.h>
#endif

float random_float_01(void);

uint8_t clause_evaluate(ClauseCompressed* clause, uint8_t* input, uint32_t n_state, uint32_t n_feature);

void clause_update_type_I(ClauseCompressed* clause, uint8_t* input, int8_t clause_output, uint32_t n_state, uint32_t n_feature, float s);
void clause_update_type_II(ClauseCompressed* clause, uint8_t* input, uint32_t n_state, uint32_t n_feature);

#endif // CLAUSE_H
