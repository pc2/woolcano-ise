#include <stdio.h>
#include <stdlib.h>
#include "sor.h"

const int SOR_SIZE = 100; /* NxN grid */

int main(int argc, char *argv[])
{
    // random matrix
    int i;
    int j;
    /* allocate matrix */
    double **A = (double **) malloc(sizeof(double*)*SOR_SIZE);
    for (i=0; i<SOR_SIZE; i++)
    {
        A[i] = (double *) malloc(sizeof(double)*SOR_SIZE);
        for (j=0; j<SOR_SIZE; j++)
            A[i][j] = (double)rand() / ((double)(RAND_MAX) + 1.0);
    }
    // SOR
    SOR_execute(SOR_SIZE, SOR_SIZE, 1.25, A, 1);
    // free
    for (i=0; i<SOR_SIZE; i++)
        free(A[i]);
    free(A);
    return 0;
}
