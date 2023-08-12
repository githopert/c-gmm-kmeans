#include <stdio.h>
#include <stdlib.h>

#include "data.h"
#include "gmm.h"


int
main(int argc, char** argv)
{
        if (argc != 3) {
                printf("Wrong number of arguments: %d.\n", argc);
                printf("USAGE:\ngmm file.csv cluster_num\n");
                exit(-1);
        }


        Data* data;
        GMM_Model* model;
        size_t cluster_num;


        data = D_create_from_csv(argv[1]);
        model = (GMM_Model*)malloc(sizeof(GMM_Model));
        cluster_num = (size_t)atoi(argv[2]);

        GMM_init(model, cluster_num, data);
        GMM_fit(model, data);
        GMM_print(model);
        
        D_free(data);
        GMM_free(model);

        return 0;
}
