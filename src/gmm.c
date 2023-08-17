#include "gmm.h"


void
GMM_init(GMM_Model* model, size_t cluster_num, const Data* data)
{
        KM_Model* kmeans;
        size_t* labels;
        double sqr_dist_from_mean[cluster_num];
        size_t num_in_cluster[cluster_num];


        srand(time(NULL));

        model->cluster_num = cluster_num;

        model->weight = (double*)calloc(model->cluster_num, sizeof(double));
        model->mean = (double*)calloc(model->cluster_num, sizeof(double));
        model->std = (double*)calloc(model->cluster_num, sizeof(double));

        for (size_t cl = 0; cl < cluster_num; ++cl) {
                num_in_cluster[cl] = 0;
                sqr_dist_from_mean[cl] = 0;
        }

        kmeans = (KM_Model*)malloc(sizeof(KM_Model));
        KM_init(kmeans, cluster_num, data);
        KM_fit(kmeans, data);

        labels = KM_cluster(kmeans, data);

        for (size_t dt = 0; dt < data->length; ++dt) {
                num_in_cluster[labels[dt]] += 1;
                sqr_dist_from_mean[labels[dt]] +=\
                        (data->items[dt] - kmeans->centroids[labels[dt]]) *\
                        (data->items[dt] - kmeans->centroids[labels[dt]]);

        }

        /* initialize the gmm model from the k-means++ algorithm */
        for (size_t cl = 0; cl < cluster_num; ++cl) {
                model->weight[cl] = (double)num_in_cluster[cl] / data->length; 
        }

        for (size_t cl = 0; cl < cluster_num; ++cl) {
                model->std[cl] = sqrt(sqr_dist_from_mean[cl] /\
                                      (num_in_cluster[cl] - 1));
        }

        for (size_t cl = 0; cl < cluster_num; ++cl) {
                model->mean[cl] = kmeans->centroids[cl];
        }

        KM_free(kmeans);
        free(labels);
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

double
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
        return 1 / (s * GMM_SQRT_2PI) *\
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
                        resp[dt][cl] /= row_sum;
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
                model->mean[cl]   = M_j / N_j;

                for (size_t dt = 0; dt < data->length; ++dt) {
                        S_j += resp[dt][cl] *\
                        (data->items[dt] - model->mean[cl]) *\
                        (data->items[dt] - model->mean[cl]);
                }

                /* use new mean values */
                model->std[cl] = sqrt(S_j / N_j) + GMM_EPSILON;
        }

        /* free array */
        for (size_t r = 0; r < data->length; ++r) {
                free(resp[r]);
        }

        free(resp);
}
