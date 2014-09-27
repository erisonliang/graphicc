#include "linearalgebra.h"
#include <stdio.h>

void real3_write_array(const real3 c)
{
	printf("[%f, %f, %f]", c[0], c[1], c[2]);
}

void real3_print(const real3 c, const char *caption)
{
	printf("%s ==> ", caption);
	real3_write_array(c);
	printf("\n");
}

void test_arithmetic()
{
	real3 c;
	real3 a = { 1, 0, 1 };
	real3 b = { 0, 1, 0 };
	real3 u = { 1, 1, 1 };
	real3 ux = { 1, 0, 0 };
	real3 uy = { 0, 1, 0 };

	real3_add(c, a, b);
	real3_print(c, "c = a + b");

	real3_sub(c, a, b);
	real3_print(c, "c = a - b");

	real3_neg(c, a);
	real3_print(c, "c = -a");

	real3_mul_scalar(c, a, 5);
	real3_print(c, "c = a * 5");

	real3_div_scalar(c, c, 5);
	real3_print(c, "c = c / 5");

	printf("MAG: %f\n", real3_mag(u));
	printf("DOT: %f\n", real3_dot(u, u));

	real3_cross(c, ux, uy);
	real3_print(c, "c = ux cross uy");
}

int main(int argc, char **argv)
{
	test_arithmetic();

	return 0;
}