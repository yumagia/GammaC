
#include <string>
#include <vector>
#include <map>

/**=============================================
 * BASIC TYPES
 * 
 * =============================================
 */

struct vec3i {
	int x, y, z;
};

struct vec3f {
	float x, y, z;
};

struct BoundBox {
	vec3f min, max;
};

struct BoundBoxInt {
	vec3i min, max;
};


/**=============================================
 * BSP FORMAT
 * 
 * =============================================
 */

enum LumpTypes {
	entities = 0,
	planes,
	vertices,
	visibility,
	nodes,
	texInfos,
	faces,
	lightmaps,
	leaves,
	leafFaces,
	leafBrushes,
	edges,
	faceEdges,
	models,
	brushes,
	brushSides
};
 
struct BspLump {
	int length;
};

struct BspHeader {
	char		magic[4];
	int			version;
	BspLump		lump[19];
};

/**=============================================
 * LEVEL LAYOUT DEFS
 * 
 * =============================================
 */

struct BspModel {
	BoundBox	bound;
	vec3f		origin;
	int			headNode;
	int			frontNode, backNode;
	int			outsideNode;

	int			numLeafs;

	int			firstFace;
	int			numFaces;
};

struct BspVertex {
	float		x, y, z;
};

struct BspEdge {
	int			startVert, endVert;
};

struct BspSurface {
	vec3f	vectorS;
	float	distS;

	vec3f	vectorT;
	float	distT;
	
	int		texture;

	int		animated;
};

struct BspFace {
	int plane;
	int planeSide;

	int firstEdge;
	int numEdges;

	int texInfo;

	int lightMapStyles[4];
	int lightMapOffset;
};

struct BspTexInfo {
	vec3f		uAxis;
	float		uOffset;
	
	vec3f		vAxis;
	float		vOffset;

	int			flags;
	int			value;

	std::string	name;

	int			next;
};

struct BspEntities {
	std::string	ents;
};

/**=============================================
 * BSP TREE DEFS
 * 
 * =============================================
 */

struct BspPlane {
	vec3f	normal;
	float	dist;
	int		type;
};

struct BspNode {
	int		plane;
	
	int		children[2];

	BoundBoxInt box;

	int		firstFace;
	int		numFaces;
};

struct BspLeaf {
	int		brushOr;

	int		cluster;
	int		area;

	int		visList;

	int		firstLeafFace;
	int		numLeafFaces;

	BoundBoxInt box;
};

typedef int BspLeafFace;
typedef int BspFaceEdge;

struct BspVisOffset {
	int			pvs;
	int			aud;
};