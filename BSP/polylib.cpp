#pragma once

#include "polylib.h"
#include "veclib.h"
#include "cmdlib.h"

#define	BOGUS_RANGE	8192

void pwinding(winding_t *w) {
	int				i;
		
	for (i = 0; i < w->numpoints; i++) {
		printf ("(%f, %f, %f)\n", w->p[i][0], w->p[i][1],w->p[i][2]);
	}
}

winding_t *winding_pool[MAX_POINTS_ON_WINDING+4];

/*
================
AllocWinding
================
*/

winding_t *AllocWinding(int points) {
	winding_t		*w;

	if (winding_pool[points]) {
		w = winding_pool[points];
		winding_pool[points] = w->next;
	}
	else {
		w = (winding_t *)malloc(sizeof(*w));
		w->p = (vec3_t *)calloc(points, sizeof(vec3_t));
        }

	w->numpoints = 0;
	w->maxpoints = points;
	w->next = NULL;
	return w;
}


/*
============
FreeWinding
============
*/
void FreeWinding(winding_t *w) {
	if (w->numpoints == 0xdeaddead)
		Error("FreeWinding: freed a freed winding");

	w->numpoints = 0xdeaddead; //tombstone flag
	w->next = winding_pool[w->maxpoints];
	winding_pool[w->maxpoints] = w;
}

/*
============
RemoveColinearPoints
============
*/
int					c_removed;

void RemoveColinearPoints(winding_t *w) {
	int				i, j, k;
	vec3_t			v1, v2;
	int				nump;
	vec3_t			p[MAX_POINTS_ON_WINDING];

	nump = 0;
	for (i=0; i<w->numpoints; i++) {
		j = (i+1)%w->numpoints;
		k = (i+w->numpoints-1)%w->numpoints;
		VectorSubtract(w->p[j], w->p[i], v1);
		VectorSubtract(w->p[i], w->p[k], v2);
		VectorNormalize(v1, v2);
		VectorNormalize(v2, v2);
		if(DotProduct(v1, v2) < 0.999) {
			VectorCopy(w->p[i], p[nump]);
			nump++;
		}
	}

	if (nump == w->numpoints) {
		return;
	}

	c_removed += w->numpoints - nump;

	w->numpoints = nump;
	memcpy(w->p, p, nump*sizeof(p[0]));
}

/*
============
WindingPlane
============
*/
void WindingPlane(winding_t *w, vec3_t normal, vec_t *dist) {
	vec3_t			v1, v2;

	VectorSubtract(w->p[1], w->p[0], v1);
	VectorSubtract(w->p[2], w->p[0], v2);
	CrossProduct(v2, v1, normal);
	VectorNormalize(normal, normal);
	*dist = DotProduct(w->p[0], normal);
}

/*
=============
WindingArea
=============
*/
vec_t	WindingArea (winding_t *w) {
	int				i;
	vec3_t			d1, d2, cross;
	vec_t			total;

	total = 0;
	for (i=2; i<w->numpoints; i++) {
		VectorSubtract(w->p[i-1], w->p[0], d1);
		VectorSubtract(w->p[i], w->p[0], d2);
		CrossProduct(d1, d2, cross);
		total += 0.5 * VectorLength(cross);
	}
	return total;
}

void	WindingBounds (winding_t *w, vec3_t mins, vec3_t maxs) {
	vec_t			v;
	int				i,j;

	mins[0] = mins[1] = mins[2] = MAX_BOUND_DIM;
	maxs[0] = maxs[1] = maxs[2] = -MAX_BOUND_DIM;

	for (i=0; i<w->numpoints; i++) {
		for (j=0; j<3; j++) {
			v = w->p[i][j];
			if (v < mins[j])
				mins[j] = v;
			if (v > maxs[j])
				maxs[j] = v;
		}
	}
}

/*
=============
WindingCenter
=============
*/
void	WindingCenter(winding_t *w, vec3_t center) {
	int				i;
	float			scale;

	VectorCopy(vec3_origin, center);
	for(i=0; i<w->numpoints; i++)
		VectorAdd (w->p[i], center, center);

	scale = 1.0/w->numpoints;
	VectorScale(center, scale, center);
}

/*
=================
BaseWindingForPlane
=================
*/
winding_t *BaseWindingForPlane(vec3_t normal, vec_t dist) {
	int		i, x;
	vec_t	max, v;
	vec3_t	org, vright, vup;
	winding_t	*w;

	// find the major axis

	max = -BOGUS_RANGE;
	x = -1;
	for (i=0 ; i<3; i++) {
		v = fabs(normal[i]);
		if (v > max) {
			x = i;
			max = v;
		}
	}

	if (x==-1) {
		Error("BaseWindingForPlane: no axis found");
	}

	VectorCopy(vec3_origin, vup);
	switch (x)
	{
	case 0:
	case 1:
		vup[2] = 1;
		break;
	case 2:
		vup[0] = 1;
		break;
	}

	v = DotProduct(vup, normal);
	VectorMA(vup, -v, normal, vup);
	VectorNormalize(vup, vup);

	VectorScale(normal, dist, org);

	CrossProduct(vup, normal, vright);

	VectorScale(vup, BOGUS_RANGE, vup);
	VectorScale(vright, BOGUS_RANGE, vright);

	// project a really big	axis aligned box onto the plane
	w = AllocWinding (4);

	VectorSubtract (org, vright, w->p[0]);
	VectorAdd (w->p[0], vup, w->p[0]);

	VectorAdd (org, vright, w->p[1]);
	VectorAdd (w->p[1], vup, w->p[1]);

	VectorAdd (org, vright, w->p[2]);
	VectorSubtract (w->p[2], vup, w->p[2]);

	VectorSubtract (org, vright, w->p[3]);
	VectorSubtract (w->p[3], vup, w->p[3]);

	w->numpoints = 4;

	return w;
}