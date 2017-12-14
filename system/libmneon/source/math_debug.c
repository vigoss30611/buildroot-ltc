#include <stdio.h>
#include "math_neon.h"

struct NeonDebugST neonDebug[MAX_ID] = {
							{NULL, 0, 0}, {"powf", 0, 0}, {"sqrtf", 0, 0},
							{"modf", 0, 0}, {"floorf", 0, 0}, {"ceilf", 0, 0},
							{"logf", 0, 0}, {"log10f", 0, 0}, {"sinf", 0, 0},
							{"cosf", 0, 0}, {"sincosf", 0, 0}, {"tanf", 0, 0},
							{"atanf", 0, 0}, {"atan2f", 0, 0}, {"asinf", 0, 0},
							{"acos", 0, 0}, {"sinh", 0, 0}, {"cosh", 0, 0},
							{"expf", 0, 0}, {"fabs", 0, 0}, {"ldexpf", 0, 0},
							{"frexp", 0, 0}, {"fmod", 0, 0}, {"invsqrtf", 0, 0}
							};

void PrintNeonDebugInfo()
{
	int i;

	printf("\nMATH NEON FUNCTION HITRATE:\n");
	for(i=MIN_ID;i<MAX_ID; i++){
		if(neonDebug[i].total>0){
			printf("%s: total:%lf, hit:%lf, hitRate:%lf%%\n",
			neonDebug[i].name, neonDebug[i].total, neonDebug[i].hit, neonDebug[i].hit*100/neonDebug[i].total);
		}
	}
}

