/**
 * @file util.c
 * @brief Utilities
 */
#include "util.h"

#include <stdlib.h>

float **alloc_dataset(const size_t size, const int elem_size) {
    if ((size == 0) || (elem_size == 0)) {
        return NULL;
    }

    float **dataset = malloc(sizeof(float*) * size);
    if (dataset == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        dataset[i] = malloc(sizeof(float) * elem_size);
        if (dataset[i] == NULL) {
            for (size_t j = 0; j < i; j++) {
                free(dataset[i]);
                dataset[i] = NULL;
            }
            free(dataset);
            dataset = NULL;
            return NULL;
        }
    }

    return dataset;
}

void free_dataset(float ***dataset, const size_t size) {
    if ((dataset == NULL) || (*dataset == NULL)) {
        return;
    }

    for (size_t i = 0; i < size; i++) {
        free((*dataset)[i]);
        (*dataset)[i] = NULL;
    }
    
    free(*dataset);
    *dataset = NULL;
}
