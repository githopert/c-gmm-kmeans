#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "kmeans.h"


int
main(int argc, char** argv)
{
        if (argc != 3) {
                printf("Wrong number of arguments: %d.\n", argc);
                printf("USAGE:\nkmeans file.csv cluster_num\n");
                exit(-1);
        }


        Data* data;
        KM_Model* model;
        size_t cluster_num;


        model = (KM_Model*)malloc(sizeof(KM_Model));
        data = D_create_from_csv(argv[1]);
        cluster_num = (size_t)atoi(argv[2]);

        KM_init(model, cluster_num, data);
        KM_fit(model, data);
        KM_print(model);

        D_free(data);
        KM_free(model);

        return 0;
}