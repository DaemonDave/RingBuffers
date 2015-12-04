#include <stdio.h>
#include <cblas.h>
 
#include "config.h"
 
int
main ()
{
  int i;
  double x[] = { 1, 1, 1 };
 
#ifdef HAVE_CBLAS_DSCAL
  cblas_dscal(3, 4.323, x, 1);
#endif

  for (i = 0; i < 3; ++i)
    printf ("%f\n", x[i]);
 
	if (x[i] == 1.0)
	{
		printf("CBLAS not configured right \n");
		return -1;
	}
  return 0;
}
