#include "gd.h"
#include <stdio.h>
#include "gdtest.h"
#include <math.h>
int main()
{
    double matrix[6] = {
        0.000000,	0.150000,
        0.150000,	-0.000000,
        0.000000,	0.000000
    };
    double matrix_inv[6];
    double matrix_inv_exp[6] = {
        0.0000000000, 6.6666666667,
        6.6666666667, -0.0000000000,
        -0.0000000000, 0.0000000000
    };

    int res = gdAffineInvert(matrix_inv, matrix);
    if (!res) return -1;
    for (int i=0; i < 6; i++) {
        double rounded_res = round(matrix_inv[i] * 10);
        double rounded_exp = round(matrix_inv_exp[i] * 10);
        if (rounded_res != rounded_exp) {
            printf("%i failed %f exp %f", i, matrix_inv[i], matrix_inv_exp[i]);
            return -i;
        }
    }
	return 0;
}
