#pragma once

#include "veclib.h"

typedef struct winding_s {
	int				numpoints;
	vec3_t			*p;
	int				maxpoints;
	struct winding_s	*next;
} winding_t;

#define MAX_POINTS_ON_WINDING	64

#define ON_EPSILON		0.1


winding_t	*AllocWinding(int points);
vec_t	WindingArea(winding_t *w);
void	WindingCenter(winding_t *w, vec3_t center);
void	ClipWindingEpsilon (winding_t *in, vec3_t normal, vec_t dist,
	vec_t epsilon, winding_t **front, winding_t **back);
winding_t	*ChopWinding(winding_t *in, vec3_t normal, vec_t dist);
winding_t	*CopyWinding(winding_t *w);
winding_t	*ReverseWinding(winding_t *w);
winding_t	*BaseWindingForPlane(vec3_t normal, vec_t dist);

void	WindingPlane(winding_t *w, vec3_t normal, vec_t *dist);

void 	RemoveColinearPoints(winding_t *w);
void	FreeWinding(winding_t *w);

void	ChopWindingInPlace (winding_t **w, vec3_t normal, vec_t dist, vec_t epsilon);

void pwinding(winding_t *w);