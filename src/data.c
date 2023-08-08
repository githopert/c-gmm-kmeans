#include "data.h"


/* function definitions ---------------------------------- */
Data*
D_create_from_csv(const char* file_path)
{
        /* variables ------------------------------------- */
        FILE* file;
        Data* data;
        char buffer[INIT_SIZE];
        size_t array_size;


        array_size = INIT_SIZE;

        data = (Data*)malloc(sizeof(Data));
        data->length = 0;
        data->items = (double*)calloc(array_size, sizeof(double));

        /* open a file ----------------------------------- */
        file = fopen(file_path, "r");

        if (!file) {
                printf("Unable to open the file: %s\n", file_path);
                exit(-1);
        }

        /* parse the file -------------------------------- */
        /* fgets(buffer, INIT_SIZE, file);  skip the file header */

        while (!feof(file)) {

                if (data->length == array_size) {
                        array_size *= 2;
                        data->items = (double*)realloc(data->items,
                                                array_size * sizeof(double));
                }

                fscanf(file, "%lf\n", &data->items[data->length]);

                data->length++;
        }

        fclose(file);

        /* clip the array -------------------------------- */
        if (data->length < array_size) {
                data->items = (double*)realloc(data->items,
                                               data->length * sizeof(double));
        }

        return data;
}

void
D_free(Data* data)
{
        free(data->items);
        free(data);
}

void
D_print(const Data* data)
{
        for (size_t i = 0; i < data->length; ++i) {
                printf("%lf\n", data->items[i]);
        }

        printf("LENGTH: %ld\n", data->length);
}
