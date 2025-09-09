#include "model.h"

void DrawPoly(poly_t *p) {
	int		i;
	float	*v;


}

int		c_renderfaces;

void RenderSurface(surface_t *face) {
	c_renderfaces++;

	DrawPoly(face->poly);
}

