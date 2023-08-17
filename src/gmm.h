#ifndef GMM_H
#define GMM_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "kmeans.h"


/* macros ------------------------------------------------ */
#define GMM_EPSILON   1e-6
#define GMM_MAX_ITER  300
#define GMM_TOLERANCE 1e-8
#define GMM_SQRT_2PI  2.5066282746310002

/* typedefs, structs, unions, and enums ------------------ */
typedef struct {

        size_t cluster_num;

        double* weight;
        double* mean;
        double* std;

} GMM_Model;

/* function declarations --------------------------------- */
void GMM_init(GMM_Model* model, size_t cluster_num, const Data* data);
void GMM_fit(GMM_Model* model, const Data* data);
void GMM_free(GMM_Model* model);
void GMM_print(const GMM_Model* model);

/* helper functions */
double GMM_calc_loglikelihood(const GMM_Model* model, const Data* data);
static double GMM_gaussian_pdf(double x, double w, double mean, double std);
static double** GMM_estimation(const GMM_Model* model, const Data* data);
static void GMM_maximization(const GMM_Model* model, const Data* data, double** resp);

#endif  /* GMM_H */
