#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>


/* macros ------------------------------------------------ */
#define INIT_SIZE 256

/* typedefs, structs, unions, and enums ------------------ */
typedef struct {

        double* items;
        size_t length;

} Data;

/* function declarations --------------------------------- */
Data* D_create_from_csv(const char* file_path);
void D_free(Data* data);

void D_print(const Data* data);

#endif  /* DATA_H */
