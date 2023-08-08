#ifndef KMEANS_H
#define KMEANS_H

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "data.h"


/* macros ------------------------------------------------ */
#define KM_MAX_ITER 300
#define KM_DIFF_THRESHOLD 1e-8

/* typedefs, structs, unions, and enums ------------------ */
typedef struct {

        size_t cluster_num;
        double* centroids;

} KM_Model;

/* function declarations --------------------------------- */
void KM_init(KM_Model* model, size_t cluster_num, const Data* data);
void KM_fit(KM_Model* model, const Data* data);
void KM_free(KM_Model* model);
void KM_print(const KM_Model* model);

/* helper functions */
static double* KM_init_centroids(size_t cluster_num, const Data* data);
static size_t KM_classify(const KM_Model* model, double point);
static double KM_calc_diff(const double* old_centroids,
                           const double* new_centroids,
                           size_t cluster_num);

#endif  /* KMEANS_H */
