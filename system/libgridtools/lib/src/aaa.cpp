

#include <stdio.h>
#include <math.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#include "aaa.h"


extern "C" int test_aa_c();

int test_aa_c()
{
	printf("====test_aa_c\n");
	
	return 0;
}

int aaa::test_aa()
{
	double x=7.0;
	double y=3.0;

	b = 23;
	printf("====start test_aa  b=%d \n", b);
	printf ("======7 ^ 3 = %f\n", pow (x, y) );
    printf ("=====4.73 ^ 12 = %f\n", pow (4.73, 12.0) );
    printf ("=====cos x = %f\n", cos (x) );
    printf ("=====sin x = %f\n", sin (x) );


#ifdef USE_MATH_NEON
    printf("===== NEON powf=%f \n",(double)powf_neon(3.0, 2.0f));
#else
    printf("====NO NEON pow=%f \n",pow(5, 2));
#endif

	return 0;
}
