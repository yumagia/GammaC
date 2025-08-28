
#include "gbsp.h"

int		c_nodes;
int		c_nonvis;
int		c_active_brushes;

// Use a reasonable epsilon to spare a marginally 
// poking brush from a chop
#define	PLANESIDE_EPSILON	0.0001 

#define	PSIDE_FRONT			1
#define	PSIDE_BACK			2
#define	PSIDE_BOTH			(PSIDE_FRONT|PSIDE_BACK)
#define	PSIDE_FACING		4

/**
 * @brief Create a new brush from an AABB
 * 
 */
bspbrush_t	*BrushFromBounds(vec3_t mins, vec3_t maxs) {
	bspbrush_t	*b;
	int			i;
	vec3_t		normal;
	vec_t		dist;

	b = AllocBrush(6);
	b->numsides = 6;
	for (i = 0; i < 3; i++) {
		VectorClear(normal);
		normal[i] = 1;
		dist = maxs[i];
		b->sides[i].planenum = FindFloatPlane(normal, dist);

		normal[i] = -1;
		dist = -mins[i];
		b->sides[3 + i].planenum = FindFloatPlane(normal, dist);
	}

	CreateBrushWindings(b);

	return b;
}

/**
 * @brief Find the volume of a brush
 * 
 */
vec_t BrushVolume(bspbrush_t *brush) {
	int			i;
	winding_t	*w;
	vec3_t		corner;
	vec_t		d, area, volume;
	plane_t		*plane;

	if (!brush) {
		return 0;
	}

	// Grab the first valid point as the corner

	w = NULL;
	for (i = 0; i < brush->numsides; i++) {
		w = brush->sides[i].winding;
		if (w) {
			break;
		}
	}
	if (!w) {
		return 0;
	}
	VectorCopy(w->p[0], corner);

	// Do a tetrahedral 3D "fanning" sum

	volume = 0;
	for ( ; i < brush->numsides; i++) {
		w = brush->sides[i].winding;
		if (!w) {
			continue;
		}
		plane = &mapplanes[brush->sides[i].planenum];
		d = -(DotProduct(corner, plane->normal) - plane->dist);
		area = WindingArea(w);
		volume += d * area;
	}

	volume /= 3;
	return volume;
}



/**
 * @brief Sets the mins/maxs on a bspbrush based on the windings
 * 
 */
void BoundBrush(bspbrush_t *brush) {
	int			i, j;
	winding_t	*w;

	ClearBounds(brush->mins, brush->maxs);
	for (i=0 ; i<brush->numsides ; i++) {
		w = brush->sides[i].winding;
		if (!w) {
			continue;
		}
		for (j=0 ; j<w->numpoints ; j++) {
			AddPointToBounds(w->p[j], brush->mins, brush->maxs);
		}
	}
}

/**
 * @brief Generates and assigns windings for a brush
 * 
 */
void CreateBrushWindings(bspbrush_t *brush) {
	int			i, j;
	winding_t	*w;
	side_t		*side;
	plane_t		*plane;

	for (i = 0; i < brush->numsides; i++) {
		side = &brush->sides[i];
		plane = &mapplanes[side->planenum];
		w = BaseWindingForPlane(plane->normal, plane->dist);
		for (j = 0; j < brush->numsides && w; j++) {
			if (i == j) {
				continue;
			}
			if (brush->sides[j].bevel) {
				continue;
			}
			plane = &mapplanes[brush->sides[j].planenum^1];
			ChopWindingInPlace(&w, plane->normal, plane->dist, 0); //CLIP_EPSILON);
		}

		side->winding = w;
	}

	BoundBrush(brush);
}

/**
 * @brief Count the brushes
 * 
 */
int CountBrushList(bspbrush_t *brushes) {
	int	c;

	c = 0;
	for(; brushes; brushes = brushes->next) {
		c++;
	}
	
	return c;
}

/**
 * @brief Allocates a tree
 * 
 */
tree_t *AllocTree (void) {
	tree_t	*tree;

	tree = (tree_t*)malloc(sizeof(*tree));
	memset(tree, 0, sizeof(*tree));
	ClearBounds(tree->mins, tree->maxs);


	return tree;
}

/**
 * @brief Allocates a blank node
 * 
 */
node_t *AllocNode() {
	static int s_NodeCount = 0;

	node_t *node;

	node = (node_t*)calloc(sizeof(node_t), 1);
	node->id = s_NodeCount;

	s_NodeCount++;

	return node;
}

/**
 * @brief Allocates a new brush with a set number of sides
 * 
 */
bspbrush_t *AllocBrush(int numsides) {
	static int s_BrushId = 0;

	bspbrush_t		*bb;

	bb = (bspbrush_t*)calloc(sizeof(bspbrush_t) + numsides * sizeof(side_t), 1);
	bb->id = s_BrushId++;
	c_active_brushes++;
	return bb;
}

/**
 * @brief Frees a brush 
 * 
 */
void FreeBrush(bspbrush_t *brush) {
	int			i;
	for (i = 0; i < brush->numsides; i++) {
		if (brush->sides[i].winding) {
			FreeWinding(brush->sides[i].winding);
		}
	}
	free(brush);
	c_active_brushes--;
}

/**
 * @brief Traverses a chain of brushes, freeing each one along the way
 * 
 */
void FreeBrushList(bspbrush_t *brushes) {
	bspbrush_t		*next;

	for ( ; brushes; brushes = next) {
		next = brushes->next;

		FreeBrush(brushes);
	}
}

/**
 * @brief Duplicates the brush along with its windings and sides
 * 
 */
bspbrush_t *CopyBrush(bspbrush_t *brush) {
	bspbrush_t 		*newbrush;
	int				size;
	int				i;

	size = sizeof(bspbrush_t) + sizeof(side_t) * (brush->numsides);

	newbrush = AllocBrush(brush->numsides);
	memcpy(newbrush, brush, size);

	for (i=0; i<brush->numsides; i++) {
		if (brush->sides[i].winding) {
			newbrush->sides[i].winding = CopyWinding(brush->sides[i].winding);
		}
	}

	return newbrush;
}


/**
 * @brief Finds which leaf a point in a tree will fall to
 * 
 */
node_t *PointInLeaf(node_t *node, vec3_t point) {
	vec_t			d;
	plane_t			*plane;

	while (node->planenum != PLANENUM_LEAF) {
		plane = &mapplanes[node->planenum];
		d = DotProduct(point, plane->normal) - plane->dist;
		if (d > 0) {
			node = node->children[0];
		}
		else {
			node = node->children[1];
		}
	}

	return node;
}

/**
 * @brief Finds which side of a plane an AABB will reside
 * 
 * @return Importantly will be one of the following: PSIDE_FRONT, 
 * PSIDE_BACK, or PSIDE_BOTH
 * 
 */
int BoxOnPlaneSide(vec3_t mins, vec3_t maxs, plane_t *plane) {
	int		side;
	int		i;
	vec3_t	corners[2];
	vec_t	dist1, dist2;

	if (plane->type < 3) {
		side = 0;
		if (maxs[plane->type] > plane->dist+PLANESIDE_EPSILON) {
			side |= PSIDE_FRONT;
		}
		if (mins[plane->type] < plane->dist-PLANESIDE_EPSILON) {
			side |= PSIDE_BACK;
		}
		return side;
	}

	// Create the proper leading and trailing verts for the box

	for (i=0; i<3; i++) {
		if (plane->normal[i] < 0) {
			corners[0][i] = mins[i];
			corners[1][i] = maxs[i];
		}
		else {
			corners[1][i] = mins[i];
			corners[0][i] = maxs[i];
		}
	}

	dist1 = DotProduct(plane->normal, corners[0]) - plane->dist;
	dist2 = DotProduct(plane->normal, corners[1]) - plane->dist;
	side = 0;
	if (dist1 >= PLANESIDE_EPSILON) {
		side = PSIDE_FRONT;
	}
	if (dist2 < PLANESIDE_EPSILON) {
		side |= PSIDE_BACK;
	}
	return side;
}

/**
 * @brief Does a crude splits test to find the number of splits between a brush 
 * and a plane. It uses the brush's minimum AABB to actually calculate splits
 * 
 */
int	QuickTestBrushToPlanenum(bspbrush_t *brush, int planenum, int *numsplits) {
	int			i, num;
	plane_t		*plane;
	int			s;

	*numsplits = 0;

	// If the brush actually uses the planenum,
	// We can tell the side for sure
	for (i=0; i<brush->numsides; i++) {
		num = brush->sides[i].planenum;
		if (num >= 0x10000) {
			Error("bad planenum");
		}
		if (num == planenum) {
			return PSIDE_BACK|PSIDE_FACING;
		}
		if (num == (planenum ^ 1)) {
			return PSIDE_FRONT|PSIDE_FACING;
		}
	}

	// Box on plane side
	plane = &mapplanes[planenum];
	s = BoxOnPlaneSide(brush->mins, brush->maxs, plane);

	// If both sides, count the visible faces split
	if (s == PSIDE_BOTH) {
		*numsplits += 3;
	}

	return s;
}

/**
 * @brief Properly finds the number of splits between a brush and a plane by
 * doing all the checks
 * 
 */
int	TestBrushToPlanenum(bspbrush_t *brush, int planenum,
				int *numsplits, bool *hintsplit, int *epsilonbrush) {
	int			i, j, num;
	plane_t		*plane;
	int			s;
	winding_t	*w;
	vec_t		d, d_front, d_back;
	int			front, back;

	*numsplits = 0;
	*hintsplit = false;

	// If the brush actually uses the planenum,
	// We can tell the side for sure
	for (i=0; i<brush->numsides; i++) {
		num = brush->sides[i].planenum;
		if (num >= 0x10000) {
			Error("bad planenum");
		}
		if (num == planenum) {
			return PSIDE_BACK|PSIDE_FACING;
		}
		if (num == (planenum ^ 1)) {
			return PSIDE_FRONT|PSIDE_FACING;
		}
	}

	// Box on plane side
	plane = &mapplanes[planenum];
	s = BoxOnPlaneSide(brush->mins, brush->maxs, plane);

	if (s != PSIDE_BOTH) {
		return s;
	}

	// If both sides, count the visible faces split
	d_front = d_back = 0;

	for (i=0; i<brush->numsides; i++) {
		if (brush->sides[i].texinfo == TEXINFO_NODE) {
			continue;		// On node, don't worry about splits
		}
		if (!brush->sides[i].visible) {
			continue;		// We don't care about non-visible
		}

		w = brush->sides[i].winding;

		if (!w) {
			continue;		// Huh?
		}

		front = back = 0;
		for (j=0; j<w->numpoints; j++) {
			d = DotProduct (w->p[j], plane->normal) - plane->dist;
			if (d > d_front) {
				d_front = d;
			}
			if (d < d_back) {
				d_back = d;
			}

			if (d > PLANESIDE_EPSILON) {
				front = 1;
			}
			if (d < -PLANESIDE_EPSILON) {
				back = 1;
			}
		}
		if (front && back) {
			if ( !(brush->sides[i].surf & SURF_SKIP) ) {
				(*numsplits)++;
				if (brush->sides[i].surf & SURF_HINT) {
					*hintsplit = true;
				}
			}
		}
	}

	if ((d_front > 0.0 && d_front < 1.0)
		||(d_back < 0.0 && d_back > -1.0)) {
		(*epsilonbrush)++;
	}

	return s;
}

#define	EDGE_LENGTH	0.2
/**
 * @brief Finds whether the winding's total length is below EDGE_LENGTH
 * 
 */
bool WindingIsTiny(winding_t *w) {
	int		i, j;
	vec_t	len;
	vec3_t	delta;
	int		edges;

	edges = 0;
	for (i=0 ; i<w->numpoints ; i++) {
		j = i == w->numpoints - 1 ? 0 : i+1;
		VectorSubtract (w->p[j], w->p[i], delta);
		len = VectorLength(delta);
		if (len > EDGE_LENGTH) {
			if (++edges == 3) {
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Finds at any point, whether we have a whopping length on this winding
 * 
 */
bool WindingIsHuge(winding_t *w) {
	int		i, j;

	for (i = 0; i < w->numpoints; i++) {
		for (j = 0; j < 3; j++) {
			if (w->p[i][j] < -8000 || w -> p[i][j] > 8000) {
				return true;
			}
		}
	}
	return false;
}

/**
 * @brief Writes a leaf
 * 
 */
void LeafNode(node_t *node, bspbrush_t *brushes) {
	bspbrush_t		*b;
	int				i;

	node->planenum = PLANENUM_LEAF;
	node->contents = 0;

	for (b=brushes; b; b=b->next) {
		//Check if the brush eats everything
		if (b->original->contents) {
			if (i == b->numsides) {
				node->contents = 1;
				break;
			}
		}
		node->contents |= b->original->contents;
	}
	node->brushlist = brushes;
}

/**
 * @brief Make sure that we don't proceed with plane shennanagins
 * which would otherwise cause issues
 * 
 */
void CheckPlaneAgainstParents(int pnum, node_t *node) {
	node_t			*p;

	for (p=node->parent ; p ; p=p->parent) {
		if (p->planenum == pnum) {
			Error("Tried parent");
		}
	}
}

/**
 * @brief Checks the bounding Volume of a node against a plane
 * 
 */
bool CheckPlaneAgainstVolume(int pnum, node_t *node) {
	bspbrush_t		*front, *back;
	bool			good;

	SplitBrush(node->volume, pnum, &front, &back);

	good = (front && back);

	if (front) {
		FreeBrush (front);
	}
	if (back) {
		FreeBrush (back);
	}

	return good;
}

/**
 * @brief Using a hueristic, choose one of the sides out of the brushlist
 * to partition the brushes with. Return NULL if there aren't any valid planes 
 * to split with.
 * 
 */
side_t *SelectSplitSide(bspbrush_t *brushes, node_t *node) {
	int				value, bestvalue;
	bspbrush_t		*brush, *test;
	side_t			*side, *bestside;
	int				i, j, pass, numpasses;
	int				pnum;
	int				s;
	int				front, back, both, facing, splits;
	int				bsplits, bestsplits;
	int				epsilonbrush;
	bool			hintsplit;


	bestside = NULL;
	bestvalue = -99999;
	bestsplits = 0;
	
	// The search order goes: visible-structural, visible-detail,
	// nonvisible-structural, nonvisible-detail.
	// If any valid plane is available in a pass, no further
	// passes will be tried.

	numpasses = 4;
	for(pass = 0; pass < numpasses; pass++) {
		for(brush = brushes; brush; brush = brush->next) {
			if((pass & 1) && !(brush->original->contents & CONTENTS_DETAIL)) {
				continue;
			}
			if(!(pass & 1) && (brush->original->contents & CONTENTS_DETAIL)) {
				continue;
			}
			for(i = 0; i < brush->numsides; i++) {
				side = brush->sides + i;
				if(side->bevel) {
					continue;	// Never use a bevel as a spliter
				}
				if(!side->winding) {
					continue;	// Nothing visible, so it can't split
				}
				if(side->texinfo == TEXINFO_NODE) {
					continue;	// Already a node splitter
				}
				if(side->tested) {
					continue;	// We already have metrics for this plane
				}
				if(side->surf & SURF_SKIP) {
					continue;	// Skip surfaces are never chosen
				}
				if(side->visible ^ (pass < 2)) {
					continue;	// Only check visible faces on first pass
				}

				pnum = side->planenum;
				pnum &= ~1;		// Always use positive-facing

				CheckPlaneAgainstParents(pnum, node);
				if(!CheckPlaneAgainstVolume(pnum, node)) {
					continue;	// would produce a tiny volume
				}

				// Setup the sentinel vals and do the actual tests
				front = 0;
				back = 0;
				both = 0;
				facing = 0;
				splits = 0;
				epsilonbrush = 0;
				
				for(test = brushes; test; test = test->next) {
					s = TestBrushToPlanenum(test, pnum, &bsplits, &hintsplit, &epsilonbrush);

					splits += bsplits;
					if(bsplits && (s&PSIDE_FACING)) {
						Error("PSIDE_FACING with splits");
					}
					test->testside = s;
					// If the brush shares this face, don't bother
					// with testing that facenum as a splitter again
					if(s & PSIDE_FACING) {
						facing++;
						for(j = 0; j < test->numsides; j++) {
							if((test->sides[j].planenum&~1) == pnum) {
								test->sides[j].tested = true;
							}
						}
					}
					if(s & PSIDE_FRONT) {
						front++;
					}
					if(s & PSIDE_BACK) {
						back++;
					}
					if(s == PSIDE_BOTH) {
						both++;
					}
				}


				// Give a value estimate for using this plane

				value =  5*facing - 5*splits - abs(front-back);
				// value =  -5*splits;
				// value =  5*facing - 5*splits;
				if(mapplanes[pnum].type < 3) {
					value+=5;		// Axial is better
				}
				value -= epsilonbrush*1000;	// Avoid!

				// Never split a hint side except with another hint
				if(hintsplit && !(side->surf & SURF_HINT)) {
					value = -9999999;
				}

				// Save off the side test so we don't need
				// to recalculate it when we actually seperate
				// the brushes
				if (value > bestvalue) {
					bestvalue = value;
					bestside = side;
					bestsplits = splits;
					for (test = brushes ; test ; test=test->next) {
						test->side = test->testside;
					}
				}

			}
		}

		// If we found a good plane, don't bother trying any
		// other passes
		if (bestside) {
			if (pass > 1) {
				c_nonvis++;
			}
			if (pass > 0) {
				node->detail_seperator = true;	// Not considered for VIS
			}
			break;					// Don't return bestside yet!
		}

	}

	//
	// Clear all the tested flags we set.
	// Do this before returning.
	//
	for (brush = brushes ; brush ; brush=brush->next) {
		for (i = 0; i < brush->numsides; i++) {
			brush->sides[i].tested = false;
		}
	}

	return bestside;
}

/**
 * @brief Find which side a brush exists in moreso
 */
int BrushMostlyOnSide(bspbrush_t *brush, plane_t *plane) {
	int			i, j;
	winding_t	*w;
	vec_t		d, max;
	int			side;

	max = 0;
	side = PSIDE_FRONT;
	for (i=0; i < brush->numsides; i++) {
		w = brush->sides[i].winding;
		if (!w) {
			continue;
		}
		for (j = 0; j < w->numpoints; j++) {
			d = DotProduct(w->p[j], plane->normal) - plane->dist;
			if (d > max) {
				max = d;
				side = PSIDE_FRONT;
			}
			if (-d > max) {
				max = -d;
				side = PSIDE_BACK;
			}
		}
	}
	return side;
}

/**
 * @brief Generate splits for a brush into two new brushes, leaving 
 * the original unchanged
 */
void SplitBrush(bspbrush_t *brush, int planenum,
		bspbrush_t **front, bspbrush_t **back) {
	bspbrush_t	*b[2];
	int			i, j;
	winding_t	*w, *cw[2], *midwinding;
	plane_t		*plane, *plane2;
	side_t		*s, *cs;
	float		d, d_front, d_back;

	*front = *back = NULL;
	plane = &mapplanes[planenum];

	// Check all points
	d_front = d_back = 0;
	for (i = 0; i < brush->numsides; i++) {
		w = brush->sides[i].winding;
		if (!w) {
			continue;
		}
		for (j = 0; j < w->numpoints; j++) {
			d = DotProduct(w->p[j], plane->normal) - plane->dist;
			if (d > 0 && d > d_front) {
				d_front = d;
			}
			if (d < 0 && d < d_back) {
				d_back = d;
			}
		}
	}
	if (d_front < PLANESIDE_EPSILON) {	// Only on back
		*back = CopyBrush(brush);
		return;
	}
	if (d_back > -PLANESIDE_EPSILON) {	// Only on front
		*front = CopyBrush(brush);
		return;
	}

	// Create a new winding from the split plane

	w = BaseWindingForPlane(plane->normal, plane->dist);
	for (i=0 ; i<brush->numsides && w ; i++) {
		plane2 = &mapplanes[brush->sides[i].planenum ^ 1];
		ChopWindingInPlace(&w, plane2->normal, plane2->dist, PLANESIDE_EPSILON);
	}

	if (!w || WindingIsTiny(w)) {	// The brush isn't really split
		int		side;

		side = BrushMostlyOnSide(brush, plane);
		if (side == PSIDE_FRONT)
			*front = CopyBrush(brush);
		if (side == PSIDE_BACK)
			*back = CopyBrush(brush);
		return;
	}

	if (WindingIsHuge(w)) {
		printf("WARNING: huge winding\n");
	}

	midwinding = w;

	// Split it for real now

	for (i = 0; i < 2; i++) {
		b[i] = AllocBrush(brush->numsides+1);
		b[i]->original = brush->original;
	}

	// Split all the current windings

	for (i=0 ; i<brush->numsides ; i++) {
		s = &brush->sides[i];
		w = s->winding;
		if (!w) {
			continue;
		}
		ClipWindingEpsilon(w, plane->normal, plane->dist,
			0, &cw[0], &cw[1]);
		
		for (j=0 ; j<2 ; j++) {
			if (!cw[j]) {
				continue;
			}
		
			cs = &b[j]->sides[b[j]->numsides];
			b[j]->numsides++;
			*cs = *s;
			cs->winding = cw[j];
			cs->tested = false;
		}
	}

	// See if we have valid polygons on both sides

	for (i = 0; i < 2; i++) {
		BoundBrush(b[i]);
		for (j=0 ; j<3 ; j++) {
			if (b[i]->mins[j] < -4096 || b[i]->maxs[j] > 4096) {
				printf("bogus brush after clip\n");
				break;
			}
		}

		if (b[i]->numsides < 3 || j < 3) {
			FreeBrush(b[i]);
			b[i] = NULL;
		}

	}

	if (!(b[0] && b[1])) {
		if (!b[0] && !b[1]) {
			printf("split removed brush\n");
		}
		else {
			printf("split not on both sides\n");
		}

		if (b[0]) {
			FreeBrush(b[0]);
			*front = CopyBrush(brush);
		}
		if (b[1]) {
			FreeBrush(b[1]);
			*back = CopyBrush(brush);
		}
		return;
	}

	// Add the midwinding to both sides
	for (i = 0; i < 2; i++) {
		cs = &b[i]->sides[b[i]->numsides];
		b[i]->numsides++;

		cs->planenum = planenum^i^1;
		cs->texinfo = TEXINFO_NODE;
		cs->visible = false;
		cs->tested = false;
		if (i == 0) {
			cs->winding = CopyWinding(midwinding);
		}
		else {
			cs->winding = midwinding;
		}
	}

	*front = b[0];
	*back = b[1];
}

/**
 * @brief Split all the brushes on a node
*/
void SplitBrushList(bspbrush_t *brushes, 
		node_t *node, bspbrush_t **front, bspbrush_t **back) {
	bspbrush_t	*brush, *newbrush, *newbrush2;
	side_t		*side;
	int			sides;
	int			i;

	*front = *back = NULL;

	for (brush = brushes ; brush ; brush=brush->next) {
		sides = brush->side;

		if (sides == PSIDE_BOTH) {	// Split into two
			SplitBrush(brush, node->planenum, &newbrush, &newbrush2);
			if (newbrush) {
				newbrush->next = *front;
				*front = newbrush;
			}
			if (newbrush2) {
				newbrush2->next = *back;
				*back = newbrush2;
			}
			continue;
		}

		newbrush = CopyBrush(brush);

		// If the planenum is actualy a part of the brush,
		// find the plane and flag it as used so it won't be tried
		// as a splitter again
		if (sides & PSIDE_FACING) {
			for (i = 0; i < newbrush->numsides; i++) {
				side = newbrush->sides + i;
				if ((side->planenum& ~1) == node->planenum) {
					side->texinfo = TEXINFO_NODE;
				}
			}
		}

		if (sides & PSIDE_FRONT) {
			newbrush->next = *front;
			*front = newbrush;
			continue;
		}
		if (sides & PSIDE_BACK) {
			newbrush->next = *back;
			*back = newbrush;
			continue;
		}
	}

}

/**
 * The recursive function used to construct the BSP tree
*/
node_t *BuildTree_r(node_t *node, bspbrush_t *brushes) {
	node_t			*newnode;
	side_t			*bestside;
	bspbrush_t	*children[2];

	c_nodes++;

	bestside = SelectSplitSide(brushes, node);
	if (!bestside) {
		//We add a leaf
		node->side = NULL;
		LeafNode(node, brushes);
		return node;
	}

	node->side = bestside;
	node->planenum = bestside->planenum & ~1;
	
	SplitBrushList(brushes, node, &children[0], &children[1]);
	FreeBrushList(brushes);

	newnode = AllocNode();
	newnode->parent = node;
	node->children[0] = newnode;
	newnode = AllocNode();
	newnode->parent = node;
	node->children[1] = newnode;

	SplitBrush(node->volume, node->planenum, &node->children[0]->volume,
		&node->children[1]->volume);
	// Recurse onto children
	node->children[0] = BuildTree_r(node->children[0], children[0]);
	node->children[1] = BuildTree_r(node->children[1], children[1]);

	return node;
}

/**
 * Take the selection of brushes, and bsp them
*/
tree_t *BrushBSP(bspbrush_t *brushlist, vec3_t mins, vec3_t maxs) {
	node_t		*node;
	bspbrush_t	*b;
	int			c_faces, c_nonvisfaces;
	int			c_brushes;
	tree_t		*tree;
	int			i;
	vec_t		volume;

	std::cout << "--- BrushBSP ---" << std::endl;

	tree = AllocTree();

	c_faces = 0;
	c_nonvisfaces = 0;
	c_brushes = 0;

	for(b = brushlist; b; b = b->next) {
		c_brushes++;

		volume = BrushVolume(b);
		if (volume < microvolume) {
			printf("WARNING: entity %i, brush %i: microbrush\n",
				b->original->entitynum, b->original->brushnum);
		}

		for (i=0 ; i<b->numsides ; i++) {
			if (b->sides[i].bevel) {
				continue;
			}
			if (!b->sides[i].winding) {
				continue;
			}
			if (b->sides[i].texinfo == TEXINFO_NODE) {
				continue;
			}
			if (b->sides[i].visible) {
				c_faces++;
			}
			else {
				c_nonvisfaces++;
			}
				
		}

		AddPointToBounds(b->mins, tree->mins, tree->maxs);
		AddPointToBounds(b->maxs, tree->mins, tree->maxs);
	}


	std::cout << c_brushes << " brushes" << std::endl;
	std::cout << c_faces << " visible faces" << std::endl;
	std::cout << c_nonvisfaces << " nonvisible faces" << std::endl;

	c_nodes = 0;
	c_nonvis = 0;

	node = AllocNode();

	node->volume = BrushFromBounds(mins, maxs);

	tree->headnode = node;

	node = BuildTree_r(node, brushlist);

	std::cout << c_nodes/2 - c_nonvis << " visible nodes" << std::endl;
	std::cout << c_nonvis << " nonvis nodes" << std::endl;
	std::cout << (c_nodes + 1)/2 << " leafs" << std::endl;

	return tree;
}