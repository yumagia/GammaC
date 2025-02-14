#pragma once

#include "mathlib/vec3.h"
#include "bsplib.h"


typedef struct plane_s  {
	vec3_t			normal, point;
	struct plane_s		*hash_chain;
} plane_t;

typedef struct winding_s {
	int				numpoints;
	vec3_t			*p;
	int				maxpoints;
	struct winding_s	*next;
} winding_t;

#define MAX_POINTS_ON_WINDING	64

winding_t	*AllocWinding (int points);