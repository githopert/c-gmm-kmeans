#include "../data.h"


/* function implementations ------------------------------ */
int
main(int argc, char** argv)
{
        if (argc != 2) {
                printf("Wrong number of arguments: %d.\n", argc);
                printf("USAGE:\ntest-data file.csv\n");
                exit(-1);
        }


        Data* data;


        data = D_create_from_csv(argv[1]);
        D_print(data);
        D_free(data);

        return 0;
}
