
#include "cmdlib.h"
#include "scriplib.h"
#include "veclib.h"
#include "polylib.h"
#include "bspfile.h"
#include <iostream>
#include <vector>

#define	MAX_BRUSH_SIDES		128
#define	CLIP_EPSILON		0.1

#define	BOGUS_RANGE			8192

#define	TEXINFO_NODE		-1			// Side is already on a node

typedef struct plane_s {
	vec3_t			normal;
	vec_t			dist;				// The distance from normal vector
	int		type;						// Denotes specialized cases	
	struct plane_s		*hash_chain;
} plane_t;

typedef struct brush_texture_s {
	vec_t	shift[2];
	vec_t	rotate;
	vec_t	scale[2];
	std::string		name;
	int		flags;
	int		value;
} brush_texture_t;

typedef struct side_s {
	int				planenum;
	int				texinfo;
	winding_t		*winding;
	struct side_s		*original;		// bspbrush_t sides will reference the mapbrush_t sides
	int				contents;	// Content flag from Miptex
	int				surf;		// Surface flag from Miptex
	bool			visible;		// Visible are chosen first
	bool			tested;			// True upon use as a splitter, tested are never reused
	bool			bevel;			// Beveled are never used to split
} side_t;

typedef struct brush_s {
	int				entitynum;
	int				brushnum;

	int				contents;

	vec3_t mins, maxs;

	int				numsides;
	side_t			*original_sides;
} mapbrush_t;

#define PLANENUM_LEAF		-1

#define MAXEDGES		20

typedef struct face_s {
	struct face_s		*next;			// The next on the node
	winding_t		*w;
	int				planenum;
	int				numverts;
	side_t			*original;		// Save the side this face came from
} face_t;

typedef struct bspbrush_s {
	int				id;
	struct bspbrush_s 	*next;
	vec3_t			mins, maxs;
	int				side;		// Side of node during construction
	int				testside;	// Used for split testing
	mapbrush_t		*original;
	int				numsides;
	side_t			sides[];
} bspbrush_t;

typedef struct leafface_s {
	face_t			*pFace;
	struct leafface_s	*pNext;
} leafface_t;

#define MAX_NODE_BRUSHES	8

typedef struct node_s {
	int				id;

	// leaves and internal nodes
	int				planenum;	// -1 = Leaf node
	struct node_s		*parent;
	vec3_t			minb, maxb;		// Valid post-portalization
	bspbrush_t		*volume;		// A single bounding box for every node, leafs too
	// non-leaf nodes only
	bool			detail_seperator;	// A detail brush caused the split
	side_t			*side;			// The side which created the node
	struct node_s		*children[2];
	face_t			*sidefaces;		// The ones which reside on the plane of the side
	// leaves only
	bspbrush_t		*brushlist;		// Fragments of all brushes on this leaf
	leafface_t		*leaffacelist;
	int				contents;	// OR of all brush contents
	int				occupied;	// >= 1 means it can reach an entity
	entity_t		*occupantlist;		// For flood testing
	int				cluster;	// Used for VIS
	int				area;		// For areaportals
	struct portal_s		*portals;		// Also on nodes during BSP, but are pushed down on VIS
} node_t;

typedef struct portal_s {
	int				id;
	plane_t			*plane;	
	node_t			*onnode;		// NULL = The outside box
	node_t			*nodes[2];		// [0] = Front
	struct portal_s		*next[2];
	winding_t		*winding;
	bool			sidefound;		// False if ->side hasn't been checked
	side_t			*side;			// NULL = Non-visible
	face_t		    	*face[2];		// Portal Face, Write into BSP file
} portal_t;

typedef struct tree_s {
	node_t			*headnode;
	node_t			*outside_node;
	vec3_t			mins, maxs;

} tree_t;

extern	int			entity_num;

extern	plane_t		mapplanes[MAX_MAP_PLANES];
extern	int			nummapplanes;

extern	int			nummapbrushes;
extern	mapbrush_t	mapbrushes[MAX_MAP_BRUSHES];

extern	vec3_t		map_mins, map_maxs;

#define	MAX_MAP_SIDES		(MAX_MAP_BRUSHES*6)

extern	vec_t		microvolume;

extern	int			nummapbrushsides;
extern	side_t		brushsides[MAX_MAP_SIDES];

extern	std::string		source;

/**=============================================
 * map.cpp
 * 
 * =============================================
 */

void 	LoadMapFile(fs::path inputpath);
int		FindFloatPlane(vec3_t normal, vec_t dist);

/**=============================================
 * csg.cpp
 * 
 * =============================================
 */

bspbrush_t *MakeBspBrushList (int startbrush, int endbrush,
		vec3_t clipmins, vec3_t clipmaxs);
bspbrush_t *ChopBrushes (bspbrush_t *head);
bspbrush_t *InitialBrushList (bspbrush_t *list);
bspbrush_t *OptimizedBrushList (bspbrush_t *list);

void WriteBrushMap (char *name, bspbrush_t *list);


/**=============================================
 * brushbsp.cpp
 * 
 * =============================================
 */

 // Allocs
tree_t		*AllocTree();
node_t		*AllocNode();
bspbrush_t	*AllocBrush(int numsides);

// Frees
void 		FreeBrush(bspbrush_t *brushes);
void		FreeBrushList(bspbrush_t *brushes);

// Other (brush) funcs
bspbrush_t	*CopyBrush(bspbrush_t *brush);
vec_t		BrushVolume(bspbrush_t *brush);
void		CreateBrushWindings(bspbrush_t *brush);
void		BoundBrush(bspbrush_t *brush);
void SplitBrush(bspbrush_t *brush, int planenum,
		bspbrush_t **front, bspbrush_t **back);
int CountBrushList(bspbrush_t *brushes);

// The tree func
tree_t *BrushBSP(bspbrush_t *brushlist, vec3_t mins, vec3_t maxs);


/**=============================================
 * writebsp.cpp
 * 
 * =============================================
 */

void SetModelNumbers(void);
void SetLightStyles(void);

void BeginBSPFile(void);
void WriteBSP(node_t *headnode);
void EndBSPFile(void);
void BeginModel(void);
void EndModel(void);

/**=============================================
 * vkdraw.cpp
 * 
 * =============================================
 */

// Uniquely brushbsp stuff. Remove?
// node_t		*PointInLeaf(node_t *node, vec3_t point);
// int		BoxOnPlaneSide(vec3_t mins, vec3_t maxs, plane_t *plane);

// int		QuickTestBrushToPlanenum(bspbrush_t *brush, int planenum, int *numsplits);
// int		TestBrushToPlanenum(bspbrush_t *brush, int planenum,
// 					int *numsplits, bool *hintsplit, int *epsilonbrush);

// bool		WindingIsTiny(winding_t *w);
// bool		WindingIsHuge(winding_t *w);

// void 		LeafNode(node_t *node, bspbrush_t *brushes);

// void		CheckPlaneAgainstParents(int pnum, node_t *node);
// bool		CheckPlaneAgainstVolume(int pnum, node_t *node);

// side_t		*SelectSplitSide(bspbrush_t *brushes, node_t *node);
// int			BrushMostlyOnSide(bspbrush_t *brush, plane_t *plane);
// void		SplitBrush(bspbrush_t *brush, int planenum,
// 				bspbrush_t **front, bspbrush_t **back);
// void		SplitBrushList(bspbrush_t *brushes, 
// 				node_t *node, bspbrush_t **front, bspbrush_t **back);

// node_t		*BuildTree_r(node_t *node, bspbrush_t *brushes);

