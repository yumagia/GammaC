#include "cmdlib.h"
#include "vec3.h"

vec3_t vec3_origin = {0,0,0};


double VectorLength(vec3_t v)
{
	int		i;
	double	length;

	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrt(length);

	return length;
}