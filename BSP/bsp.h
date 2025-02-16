#pragma once

#include "cmdlib.h"
#include "mathlib/vec3.h"
#include "polylib.h"
#include "bsplib.h"


typedef struct side_s {
	int				planenum;
	int				textureidx;
	winding_t		*winding;
	struct side_s		*original;
	bool				visible;
	bool				tested;
	bool				bevel;
} side_t;

typedef struct brush_s {
	int				entitynum;
	int				brushnum;

	int				contents;

	vec3_t mins, maxs;

	int				numsides;
	side_t			*original_sides;
} mapbrush_t;

typedef struct face_s {
	struct face_s		*next;
	winding_t		*w;
	int				planenum;
	int				numverts;
	side_t			*original;
} face_t;

typedef struct bspbrush_s {
	int				id;
	struct bspbrush_s 	*next;
	vec3_t			mins, maxs;
	int				side, testside;
	mapbrush_t		*original;
	int				numsides;
	side_t			sides[6];
} bspbrush_t;

typedef struct leafface_s {
	face_t			*pFace;
	struct leafface_s	*pNext;
} leafface_t;

typedef struct node_s {
	int				id;

	// leaves and internal nodes
	int				planenum;	// -1 = leaf node
	struct node_s		*parent;
	vec3_t			minb, maxb;
	// nodes only
	side_t			*side; 
	struct node_s		*children[2];
	face_t			*face;
	// leaves only
	bspbrush_t		*brushlist;
	leafface_t		*leaffacelist;
	int				contents;
	int				occupied;
	entity_t		*occupantlist;
	struct portal_s		*portals;
} node_t;

typedef struct portal_s {
	int				id;
	plane_t			*plane;
	node_t			*onnode;
	node_t			*nodes[2];
	struct portal_s		*next[2];
	winding_t		*winding;
} portal_t;

typedef struct tree_s {
	node_t			*headnode;
	node_t			*outside_node;
	vec3_t			mins, maxs;

} tree_t;



node_t		*AllocNode();
bspbrush_t	*AllocBrush(int numsides);
void 		FreeBrush(bspbrush_t *brushes);
void 		LeafNode (node_t *node, bspbrush_t *brushes);
side_t 		*SelectSplitSide(node_t *node);
node_t 		*BuildTree_r(node_t *node, bspbrush_t *brushes);
