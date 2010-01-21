#include <stdio.h>
#include <stdlib.h>
#include "fft.h"

const int FFT_SIZE = 1024;  /* must be a power of two */

int main(int argc, char *argv[])
{
    int i;
    double *x = (double*)malloc(sizeof(double) * 2 * FFT_SIZE);
    for (i = 0; i < 2 * FFT_SIZE; i++)
        x[i] = (double)rand() / ((double)(RAND_MAX) + 1.0);
    FFT_transform(2 * FFT_SIZE, x);     /* forward transform */
    FFT_inverse(2 * FFT_SIZE, x);       /* backward transform */
    free(x);
    return 0;
}
