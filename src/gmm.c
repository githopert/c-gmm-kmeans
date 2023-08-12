#include "gmm.h"


void
GMM_init(GMM_Model* model, size_t cluster_num, const Data* data)
{
        KM_Model* kmeans;


        srand(time(NULL));

        model->cluster_num = cluster_num;

        model->weight = (double*)calloc(model->cluster_num, sizeof(double));
        model->mean = (double*)calloc(model->cluster_num, sizeof(double));
        model->std = (double*)calloc(model->cluster_num, sizeof(double));

        for (size_t i = 0; i < cluster_num; ++i) {
                model->weight[i] = (double)1 / cluster_num; 
        }

        for (size_t j = 0; j < cluster_num; ++j) {
                model->std[j] = 0.2 + ((double)rand() / RAND_MAX) * 0.8; 
        }

        kmeans = (KM_Model*)malloc(sizeof(KM_Model));
        KM_init(kmeans, cluster_num, data);
        KM_fit(kmeans, data);

        for (size_t k = 0; k < cluster_num; ++k) {
                model->mean[k] = kmeans->centroids[k];
        }

        KM_free(kmeans);
}

void
GMM_fit(GMM_Model* model, const Data* data)
{
        double** resp;
        double old_log_lh, new_log_lh, log_lh_diff;
        size_t iter;


        iter = 1;
        old_log_lh = new_log_lh = 0;

        while (1) {

                old_log_lh = new_log_lh;

                resp = GMM_estimation(model, data);
                GMM_maximization(model, data, resp);

                new_log_lh = GMM_calc_loglikelihood(model, data);
                log_lh_diff = fabs(new_log_lh - old_log_lh);

                if (iter > GMM_MAX_ITER || log_lh_diff < GMM_TOLERANCE) {
                        break;
                }

                iter++;

        }
}

void
GMM_free(GMM_Model* model)
{
        free(model->mean);
        free(model->std);
        free(model->weight);
        free(model);

        model = NULL;
}

void
GMM_print(const GMM_Model* model)
{
        printf("NUM OF CLUSTERS: %ld.\n", model->cluster_num);

        /* print as formula */
        printf("p(x) = ");
        
        for (size_t i = 0; i < model->cluster_num; ++i) {
                printf("%.3lf * N(x | %.3lf, %.3lf) ", model->weight[i],
                                                       model->mean[i],
                                                       model->std[i]);

                if (i != model->cluster_num - 1) {
                        printf("+ ");
                }
        }

        printf("\n");
}

static double
GMM_calc_loglikelihood(const GMM_Model* model, const Data* data)
{
        double log_lh, lh;


        log_lh = 0;

        for (size_t dt = 0; dt < data->length; ++dt) {
                lh = 0;

                for (size_t cl = 0; cl < model->cluster_num; ++cl) {
                        lh += GMM_gaussian_pdf(data->items[dt],
                                               model->weight[cl],
                                               model->mean[cl],
                                               model->std[cl]);
                }

                log_lh += log(lh);
        }

        return log_lh;
}

static double
GMM_gaussian_pdf(double x, double w, double m, double s)
{
        return 1 / (s * GMM_SQRT_2PI) * \
               exp(-(x - m) * (x - m) / (2 * s * s));
}

static double**
GMM_estimation(const GMM_Model* model, const Data* data)
{
        double** resp;
        double r, row_sum;


        /* init array */
        resp = (double**)malloc(data->length * sizeof(double*));

        for (size_t dt = 0; dt < data->length; ++dt) {
                resp[dt] = (double*)calloc(model->cluster_num, sizeof(double));
        }


        /* fill array */
        for (size_t dt = 0; dt < data->length; ++dt) {
                row_sum = 0;

                for (size_t cl = 0; cl < model->cluster_num; ++cl) {

                        r = GMM_gaussian_pdf(data->items[dt], model->weight[cl],
                                             model->mean[cl], model->std[cl]);
                        resp[dt][cl] = r;
                        row_sum += r;

                }

                for (size_t cl = 0; cl < model->cluster_num; ++cl) {
                        resp[dt][cl] /= (row_sum + GMM_EPSILON);
                }
        }

        return resp;
}

static void
GMM_maximization(const GMM_Model* model, const Data* data, double** resp)
{
        double N_j, M_j, S_j;


        for (size_t cl = 0; cl < model->cluster_num; ++cl) {
                N_j = M_j = S_j = 0;

                for (size_t dt = 0; dt < data->length; ++dt) {
                        N_j += resp[dt][cl];
                        M_j += resp[dt][cl] * data->items[dt];
                }

                model->weight[cl] = N_j / data->length;
                model->mean[cl]   = M_j / (N_j + GMM_EPSILON);

                for (size_t dt = 0; dt < data->length; ++dt) {
                        S_j += resp[dt][cl] *\
                        (data->items[dt] - model->mean[cl]) *\
                        (data->items[dt] - model->mean[cl]);
                }

                /* use new mean values */
                model->std[cl] = sqrt(S_j / (N_j + GMM_EPSILON)) + GMM_EPSILON;
        }

        /* free array */
        for (size_t r = 0; r < data->length; ++r) {
                free(resp[r]);
        }

        free(resp);
}
