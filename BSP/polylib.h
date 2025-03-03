#pragma once

typedef struct winding_s {
	int				numpoints;
	vec3_t			*p;
	int				maxpoints;
	struct winding_s	*next;
} winding_t;

#define MAX_POINTS_ON_WINDING	64


winding_t	*AllocWinding(int points);


void		FreeWinding(winding_t *w);
void 		RemoveColinearPoints (winding_t *w);