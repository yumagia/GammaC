#include "gbsp.h"

extern	float subdivide_size;

char		source[1024];
char		name[1024];

vec_t		microvolume = 1.0;
bool		noprune;
bool		glview;
bool		nodetail;
bool		fulldetail;
bool		onlyents;
bool		nomerge;
bool		nowater;
bool		nofill;
bool		nocsg;
bool		noweld;
bool		noshare;
bool		nosubdiv;
bool		notjunc;
bool		noopt;
bool		leaktest;
bool		verboseentities;

char		outbase[32];

int			block_xl = -8, block_xh = 7, block_yl = -8, block_yh = 7;

int			entity_num;


node_t		*block_nodes[10][10];

/**
 * @brief Dice the tree, this is done before building it
 */
node_t	*BlockTree(int xl, int yl, int xh, int yh) {
	node_t	*node;
	vec3_t	normal;
	float	dist;
	int		mid;

	if (xl == xh && yl == yh) {
		node = block_nodes[xl+5][yl+5];
		if (!node) {	// return an empty leaf
			node = AllocNode();
			node->planenum = PLANENUM_LEAF;
			node->contents = 0; //CONTENTS_SOLID;
			return node;
		}
		return node;
	}

	// create a seperator along the largest axis
	node = AllocNode();

	if (xh - xl > yh - yl) {	// split x axis
		mid = xl + (xh - xl) / 2 + 1;
		normal[0] = 1;
		normal[1] = 0;
		normal[2] = 0;
		dist = mid * 1024;
		node->planenum = FindFloatPlane(normal, dist);
		node->children[0] = BlockTree(mid, yl, xh, yh);
		node->children[1] = BlockTree(xl, yl, mid - 1, yh);
	}
	else {
		mid = yl + (yh - yl) / 2 + 1;
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		dist = mid * 1024;
		node->planenum = FindFloatPlane(normal, dist);
		node->children[0] = BlockTree(xl, mid, xh, yh);
		node->children[1] = BlockTree(xl, yl, xh, mid - 1);
	}

	return node;
}

int			brush_start, brush_end;



/**
 * @brief Process the world tree
 */
void ProcessWorldModel(void) {
	entity_t	*e;
	tree_t		*tree;
	bool		leaked;

	e = &entities[entity_num];

	brush_start = e->firstbrush;
	brush_end = brush_start + e->numbrushes;
	leaked = false;

	//
	// perform per-block operations
	//
	if (block_xh * 1024 > map_maxs[0]) {
		block_xh = floor(map_maxs[0]/1024.0);
	}
	if ( (block_xl+1) * 1024 < map_mins[0]) {
		block_xl = floor(map_mins[0]/1024.0);
	}
	if (block_yh * 1024 > map_maxs[1]) {
		block_yh = floor(map_maxs[1]/1024.0);
	}
	if ( (block_yl+1) * 1024 < map_mins[1]) {
		block_yl = floor(map_mins[1]/1024.0);
	}

	if (block_xl <-4) {
		block_xl = -4;
	}
	if (block_yl <-4) {
		block_yl = -4;
	}
	if (block_xh > 3) {
		block_xh = 3;
	}
	if (block_yh > 3) {
		block_yh = 3;
	}
}


int main(int, char**) {
    std::cout << "Hello, from GRAK!\n";
}