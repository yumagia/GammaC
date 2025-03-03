#pragma once

typedef struct winding_s {
	int				numpoints;
	vec3_t			*p;
	int				maxpoints;
	struct winding_s	*next;
} winding_t;

#define MAX_POINTS_ON_WINDING	64


winding_t	*AllocWinding(int points);

winding_t	*ChopWinding (winding_t *in, vec3_t normal, vec_t dist);
winding_t	*CopyWinding (winding_t *w);
winding_t	*ReverseWinding (winding_t *w);
winding_t	*BaseWindingForPlane (vec3_t normal, vec_t dist);

void		FreeWinding(winding_t *w);
void 		RemoveColinearPoints (winding_t *w);