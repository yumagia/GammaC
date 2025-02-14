

#include "bsp.h"
//#include "cmdlib.h"

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
LeafNode
================
*/
void LeafNode (node_t *node, bspbrush_t *brushes) {
	bspbrush_t		*b;
	int				i;

	node->planenum = -1;
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

Uses a basic greedy hueristic to choose the best side for partitioning.
Returns NULL if there aren't any valid planes to split with.
================
*/
side_t *SelectSplitSide(node_t *node) {
	int				splits, bestsplits;
	side_t			*side, *bestside;
	int				i;

	bestside = NULL;
	bestsplits = -99999;

	
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
		// We add a leaf
		node->side = NULL;
		node->planenum = -1;
		LeafNode(node, brushes);
		return node;
	}

	node->side = bestside;
	node->planenum = bestside->planenum & ~1;	// From Quake II. It's a bit less trivial in Scratch
	
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