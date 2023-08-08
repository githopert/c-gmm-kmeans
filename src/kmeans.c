#include "kmeans.h"


/* function implementations ------------------------------- */
void
KM_init(KM_Model* model, size_t cluster_num, const Data* data)
{
        model->cluster_num = cluster_num;
        model->centroids = KM_init_centroids(cluster_num, data);
}

void
KM_fit(KM_Model* model, const Data* data)
{
        double new_centroids[model->cluster_num];
        size_t nums[model->cluster_num];
        double diff;
        size_t label, iter;


        for (size_t n = 0; n < model->cluster_num; ++n) {
                new_centroids[n] = 0;
                nums[n] = 0;
        }

        iter = 1;

        while (1) {

                for (size_t i = 0; i < data->length; ++i) {
                        label = KM_classify(model, data->items[i]);

                        new_centroids[label] += data->items[i];
                        nums[label] += 1;
                }


                for (size_t j = 0; j < model->cluster_num; ++j) {
                        new_centroids[j] /= nums[j];
                }

                diff = KM_calc_diff(model->centroids,
                                    new_centroids,
                                    model->cluster_num);

                if (iter > MAX_ITER || diff < DIFF_THRESHOLD) {
                        break;
                }

                for (size_t k = 0; k < model->cluster_num; ++k) {
                        model->centroids[k] = new_centroids[k];
                }

                for (size_t n = 0; n < model->cluster_num; ++n) {
                        new_centroids[n] = 0;
                        nums[n] = 0;
                }

                iter++;

        }
}

void
KM_free(KM_Model* model)
{
        free(model->centroids);
        free(model);

        model = NULL;
}

void
KM_print(const KM_Model* model)
{
        printf("NUM OF CLUSTERS: %ld\n", model->cluster_num);

        for (size_t i = 0; i < model->cluster_num; ++i) {
                printf("CENTROID %ld: %lf\n", i + 1, model->centroids[i]);
        }
}

static double*
KM_init_centroids(size_t cluster_num, const Data* data)
{
        double* centroids;
        double dist, min_dist, max_min_dist;
        size_t max_idx;


        srand(time(NULL));
        centroids = (double*)calloc(cluster_num, sizeof(double));
        centroids[0] = data->items[(size_t)((double)rand() / RAND_MAX * data->length)];

        for (size_t i = 1; i < cluster_num; ++i) {

                /* find the point farthest from all clusters */

                max_min_dist = -1;
                max_idx = -1;

                for (size_t j = 0; j < data->length; ++j) {

                        min_dist = DBL_MAX;

                        for (size_t k = 0; k < i; ++k) {
                                dist = fabs(data->items[j] - centroids[k]);

                                if (dist < min_dist) {
                                        min_dist = dist;
                                }
                        }

                        if (min_dist > max_min_dist) {
                                max_min_dist = min_dist;
                                max_idx = j;
                        }

                }

                centroids[i] = data->items[max_idx];
        }

        return centroids;
}

static size_t
KM_classify(const KM_Model* model, double point)
{
        double dist, min_dist;
        size_t label;


        min_dist = DBL_MAX;

        for (size_t i = 0; i < model->cluster_num; ++i) {
                dist = fabs(point - model->centroids[i]);

                if (dist < min_dist) {
                        min_dist = dist;
                        label = i;
                }
        }

        return label;
}

static double
KM_calc_diff(const double* old_centroids, const double* new_centroids,
             size_t cluster_num)
{
        double sum;


        sum = 0;

        for (size_t i = 0; i < cluster_num; ++i) {
                sum += (old_centroids[i] - new_centroids[i]) *\
                       (old_centroids[i] - new_centroids[i]);
        }

        return sqrt(sum);
}