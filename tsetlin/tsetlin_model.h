#ifndef __TSETLIN_MODEL_H__
#define __TSETLIN_MODEL_H__

#include <stdint.h>

// #define TSETLIN_MODEL_TRAINABLE

typedef enum {
    MODEL_TYPE__INFERENCE = 0,
    MODEL_TYPE__TRAINING = 1,

    MODEL_TYPE__COMPRESSED = 2,
    MODEL_TYPE__COMPRESSED_OFFSET = 3,
    MODEL_TYPE__COMPRESSED_BITPACK = 4,
} ModelType;

typedef struct {
    uint32_t *data;
} Clause;

#ifdef TSETLIN_MODEL_TRAINABLE
    typedef struct {
        const uint16_t n_pos_literal;
        const uint16_t n_neg_literal;
        const uint16_t *position;
        uint16_t *data;
    } ClauseCompressed;

    typedef struct {
        const uint16_t n_pos_literal;
        const uint16_t n_neg_literal;
        const uint16_t *offset;
        uint16_t *data;
    } ClauseCompressedOffset;

    typedef struct {
        const uint32_t n_literal;
        uint32_t *data;
    } ClauseCompressedBitpack;
#else
    typedef struct {
        const uint16_t n_pos_literal;
        const uint16_t n_neg_literal;
        const uint16_t *position;
        const uint16_t *data;
    } ClauseCompressed;

    typedef struct {
        const uint16_t n_pos_literal;
        const uint16_t n_neg_literal;
        const uint16_t *offset;
        const uint16_t *data;
    } ClauseCompressedOffset;

    typedef struct {
        const uint32_t n_literal;
        const uint32_t *data;
    } ClauseCompressedBitpack;
#endif // TSETLIN_MODEL_TRAINABLE

typedef struct {
    const uint32_t n_class;
    const uint32_t n_feature;
    const uint32_t n_clause;
    const uint32_t n_state;

    const ModelType model_type;

    const Clause *clauses;
    const ClauseCompressed *clauses_compressed;
    const ClauseCompressedOffset *clauses_compressed_offset;
    const ClauseCompressedBitpack *clauses_compressed_bitpack;
} Tsetlin;

#endif // __TSETLIN_MODEL_H__
