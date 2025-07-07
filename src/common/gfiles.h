

/**=============================================================================
 * 
 * .BSP file format
 * 
 * =============================================================================
 */

#define IDBSPHEADER		(('P'<<24)+('S'<<16)+('B'<<8)+'I')
				// little-endian "IBSP"

#define BSPVERSION	1

// Upper design bounds
#define	MAX_MAP_MODELS		1024
#define	MAX_MAP_BRUSHES		8192
#define	MAX_MAP_ENTITIES	2048
#define	MAX_MAP_PATHS		2048
#define	MAX_MAP_ENTSTRING	0x20000
#define	MAX_MAP_TEXTURES	1024
#define	MAX_MAP_TEXINFO		8192

#define	MAX_MAP_AREAS		256
#define	MAX_MAP_AREAPORTALS	1024
#define MAX_MAP_PLANES		65536
#define	MAX_MAP_NODES		65536
#define	MAX_MAP_BRUSHSIDES	65536
#define	MAX_MAP_LEAFS		65536
#define	MAX_MAP_VERTS		65536
#define	MAX_MAP_FACES		65536
#define	MAX_MAP_LEAFFACES	65536
#define	MAX_MAP_LEAFBRUSHES	65536
#define	MAX_MAP_PORTALS		65536
#define	MAX_MAP_EDGES		128000
#define	MAX_MAP_SURFEDGES	256000
#define	MAX_MAP_MIPTEX		0x200000
#define	MAX_MAP_LIGHTING	0x200000
#define	MAX_MAP_VISIBILITY	0x100000

// key/value pair sizes

#define	MAX_KEY		32
#define	MAX_VALUE	1024

// =============================================================================

typedef struct lump_t {
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_PLANES			1
#define	LUMP_VERTEXES		2
#define	LUMP_VISIBILITY		3
#define	LUMP_NODES			4
#define	LUMP_TEXINFO		5
#define	LUMP_FACES			6
#define	LUMP_LIGHTING		7
#define	LUMP_LEAFS			8
#define	LUMP_LEAFFACES		9
#define	LUMP_LEAFBRUSHES	10
#define	LUMP_EDGES			11
#define	LUMP_SURFEDGES		12
#define	LUMP_MODELS			13
#define	LUMP_BRUSHES		14
#define	LUMP_BRUSHSIDES		15
#define	LUMP_POP			16
#define	LUMP_AREAS			17
#define	LUMP_AREAPORTALS	18
#define	HEADER_LUMPS		19

typedef struct dheader_s {
	int			version;
	int			ident;
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct dmodel_s {
	float		mins[3], maxs[3];
	float		origin[3];		// for sounds or lights
	int			headnode;
	int			visleafs;		// not including the solid leaf 0
	int			firstface, numfaces;
} dmodel_t;

typedef struct dvertex_s {
	float	point[3];
} dvertex_t;

// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

// planes (x&~1) and (x&~1)+1 are always opposites

typedef struct dplane_s {
	float	normal[3];
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ 
} dplane_t;


// Lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_SOLID			1		// An eye is never valid in a solid
#define	CONTENTS_WINDOW			2		// Translucent, but not liquid
#define	CONTENTS_AUX			4
#define	CONTENTS_LAVA			8
#define	CONTENTS_SLIME			16
#define	CONTENTS_WATER			32
#define	CONTENTS_MIST			64		// Translucent and air-like

#define	LAST_VISIBLE_CONTENTS	64

// Remaining contents are non-visible, and don't eat brushes
#define	CONTENTS_AREAPORTAL		0x8000

#define	CONTENTS_PLAYERCLIP		0x10000
#define	CONTENTS_NPCCLIP		0x20000

// Currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0			0x40000
#define	CONTENTS_CURRENT_90			0x80000
#define	CONTENTS_CURRENT_180	0x100000
#define	CONTENTS_CURRENT_270	0x200000
#define	CONTENTS_CURRENT_UP			0x400000
#define	CONTENTS_CURRENT_DOWN	0x800000

#define	CONTENTS_ORIGIN			0x1000000		// Removed before bsping an entity

#define	CONTENTS_DETAIL			0x8000000		// Brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000		// Auto set if any surface has transparency
#define	CONTENTS_LADDER			0x20000000

#define	SURF_LIGHT				0x1				// Value will hold the light strength

#define	SURF_SKY				0x4				// Don't draw, but add to skybox
#define	SURF_WARP				0x8				// Texture warp
#define	SURF_TRANS33	0x10
#define	SURF_TRANS66	0x20

#define	SURF_NODRAW				0x80			// Don't bother referencing the texture

#define	SURF_HINT				0x100			// Make a primary bsp splitter
#define	SURF_SKIP				0x200			// Completely ignore, allowing non-closed brushes


typedef struct dnode_s {
	int			planenum;
	int			children[2];	// Negative numbers are -(leafs+1), not nodes
	short		mins[3];		// Used for frustom culling
	short		maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;	// Include counts on both sides
} dnode_t;

typedef struct texinfo_s {
	float		vecs[2][4];		// [s/t][xyz offset]
	int			miptex;
	int			flags;			// Miptex flags + overrides
	int			value;			// Light Emmission and stuff
	char		texture[32];	// Texture name (texture/*.wal)
	int			nexttexinfo;	// For animations, -1 = end of chain
} texinfo_t;

// Note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct dedge_s {
	unsigned short	v[2];		// vertex numbers
} dedge_t;

#define	MAXLIGHTMAPS	4

typedef struct dface_s {
	unsigned short	planenum;
	short		side;

	int			firstedge;		// We must support more than 64k edges
	short		numedges;
	short		texinfo;

// lighting info
	unsigned char	styles[MAXLIGHTMAPS];
	int			lightofs;		// start of [numstyles*surfsize] samples
} dface_t;

typedef struct dleaf_s {
	int			contents;			// OR of all brushes
	int			visofs;				// -1 = no visibility info

	short			cluster;
	short			area;

	short		mins[3];			// for frustum culling
	short		maxs[3];

	unsigned short		firstleafface;
	unsigned short		numleaffaces;

	unsigned short		firstleafbrush;
	unsigned short		numleafbrushes;
} dleaf_t;

typedef struct dbrushside_s {
	unsigned short	planenum;		// facing out of the leaf
	short	texinfo;
} dbrushside_t;

typedef struct dbrush_s {
	int			firstside;
	int			numsides;
	int			contents;
} dbrush_t;

// The visibility lump consists of a header with a count, then
// byte offsets for the PVS and PAS of each cluster, then the raw
// compressed bit vectors
#define	DVIS_PVS	0
#define	DVIS_PAS	1
typedef struct dvis_s {
	int			numclusters;
	int			bitofs[8][2];	// bitofs[numclusters][2]
} dvis_t;

// Each area has a list of portals that lead into other areas.
// When portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
typedef struct dareaportal_s {
	int		portalnum;
	int		otherarea;
} dareaportal_t;

typedef struct darea_t {
	int		numareaportals;
	int		firstareaportal;
} darea_t;