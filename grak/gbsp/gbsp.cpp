

#include "gbsp.h"

#define	PLANESIDE_EPSILON	0.0001 

#define	PSIDE_FRONT			1
#define	PSIDE_BACK			2
#define	PSIDE_BOTH			(PSIDE_FRONT|PSIDE_BACK)
#define	PSIDE_FACING		4

/*
================
AllocNode
================
*/
node_t *AllocNode() {
	static int s_NodeCount = 0;

	node_t *node;

	node = (node_t*)malloc(sizeof(*node));
	memset(node, 0, sizeof(*node));
	node->id = s_NodeCount;

	s_NodeCount++;

	return node;
}

/*
================
AllocBrush
================
*/
bspbrush_t *AllocBrush(int numsides) {
	static int s_BrushId = 0;

	bspbrush_t		*bb;
	int				c;

	c = (int)&(((bspbrush_t *)0)->sides[numsides]);
	bb = (bspbrush_t*)malloc(c);
	memset(bb, 0, c);
	bb->id = s_BrushId++;
	return bb;
}

/*
================
FreeBrush
================
*/
void FreeBrush(bspbrush_t *brushes) {
	int			i;
	for (i = 0; i < brushes->numsides; i++) {
		if (brushes->sides[i].winding) {
			FreeWinding(brushes->sides[i].winding);
		}
	free(brushes);
	}
}


/*
================
FreeBrushList
================
*/

void FreeBrushList(bspbrush_t *brushes) {
	bspbrush_t		*next;

	for ( ; brushes; brushes = next) {
		next = brushes->next;

		FreeBrush(brushes);
	}
}

/*
================
CopyBrush

Duplicates the brush and its windings and sides
================
*/
bspbrush_t *CopyBrush(bspbrush_t *brush) {
	bspbrush_t 		*newbrush;
	int				size;
	int				i;

	size = (int)&(((bspbrush_t *)0)->sides[brush->numsides]);

	newbrush = AllocBrush(brush->numsides);
	memcpy(newbrush, brush, size);

	for (i=0; i<brush->numsides; i++) {
		if (brush->sides[i].winding) {
			newbrush->sides[i].winding = CopyWinding(brush->sides[i].winding);
		}
	}

	return newbrush;
}


/*
================
PointInLeaf
================
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
}

/*
================
BoxOnPlaneSide

Returns PSIDE_FRONT, PSIDE_BACK, or PSIDE_BOTH
================
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

/*
================
QuickTestBrushToPlane
================
*/
int	QuickTestBrushToPlane(bspbrush_t *brush, int planenum, int *numsplits)
{
	int			i, num;
	plane_t		*plane;
	int			s;

	*numsplits = 0;

	// if the brush actually uses the planenum,
	// we can tell the side for sure
	for (i=0; i<brush->numsides; i++) {
		num = brush->sides[i].planenum;
		if (num >= 0x10000) {
			Error ("bad planenum");
		}
		if (num == planenum) {
			return PSIDE_BACK|PSIDE_FACING;
		}
		if (num == (planenum ^ 1)) {
			return PSIDE_FRONT|PSIDE_FACING;
		}
	}

	// box on plane side
	plane = &mapplanes[planenum];
	s = BoxOnPlaneSide(brush->mins, brush->maxs, plane);

	// if both sides, count the visible faces split
	if (s == PSIDE_BOTH) {
		*numsplits += 3;
	}

	return s;
}

/*
================
TestBrushToPlane
================
*/
int	TestBrushToPlane (bspbrush_t *brush, int planenum,
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
			Error ("bad planenum");
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
			continue;
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
		if (front && back)
		{
			if ( !(brush->sides[i].surf & SURF_SKIP) )
			{
				(*numsplits)++;
				if (brush->sides[i].surf & SURF_HINT)
					*hintsplit = true;
			}
		}
	}

	if ( (d_front > 0.0 && d_front < 1.0)
		|| (d_back < 0.0 && d_back > -1.0) )
		(*epsilonbrush)++;

#if 0
	if (*numsplits == 0)
	{	//	didn't really need to be split
		if (front)
			s = PSIDE_FRONT;
		else if (back)
			s = PSIDE_BACK;
		else
			s = 0;
	}
#endif

	return s;
}


/*
================
LeafNode
================
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

/*
================
SelectSplitSide

Uses a basic greedy heuristic to choose the best side for partitioning.
Returns NULL if there aren't any valid planes to split with.
================
*/
side_t *SelectSplitSide(bspbrush_t *brushes, node_t *node) {
	int				bspsplits, bestsplits;
	bspbrush_t		*brush, *test;
	side_t			*side, *bestside;
	int				i, j, pass, numpasses;

	bestside = NULL;
	bestsplits = 0;
	
	// the search order goes: visible-structural, visible-detail,
	// nonvisible-structural, nonvisible-detail.
	// If any valid plane is available in a pass, no further
	// passes will be tried.

	numpasses = 4;

	for (pass = 0; pass < numpasses; pass++) {
		for (brush = brushes; brush; brush=brush->next) {

		}
	}

	return bestside;
}

/*
================
BuildTree_r
================
*/
node_t *BuildTree_r(node_t *node, bspbrush_t *brushes) {
	node_t			*newnode;
	side_t			*bestside;

	bestside = SelectSplitSide(node);

	if (!bestside) {
		//We add a leaf
		node->side = NULL;
		LeafNode(node, brushes);
		return node;
	}

	node->side = bestside;
	node->planenum = bestside->planenum & ~1;	//Stuff like this is a bit less trivial in Scratch
	
	SplitBrushList();

	newnode = AllocNode();
	newnode->parent = node;
	node->children[0] = newnode;
	newnode = AllocNode();
	newnode->parent = node;
	node->children[1] = newnode;

	SplitBrush(node->planenum);

	node->children[0] = BuildTree_r(node->children[0], children[0]);
	node->children[1] = BuildTree_r(node->children[1], children[1]);

	return node;
}