#include <stdio.h>
#include <math.h>

int main()
{
	double a[52];
	int n;
	for (n = 0; n < 52; n++)
	{
		a[n] = (n - 25) / 10.0;
		printf("%f, %d\n", a[n] * 10.0, (int)floor(a[n] * 10.0));
	};
	return 0;
};
