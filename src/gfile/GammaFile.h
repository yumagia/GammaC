#define MAX_MAP_MODELS		256
#define MAX_MAP_ENTITIES	512
#define MAX_MAP_PLANES		16384

#define MAX_MAP_NODES		32768
#define MAX_MAP_LEAFS		32768
#define MAX_MAP_VERTS		65536
#define MAX_MAP_EDGES		128000

#define MAX_MAP_FACES		32768

#define	MAX_KEY		16
#define	MAX_VALUE	512

struct FileLump {
	int		offset, length;
};

enum {
	LUMP_MODELS,
	LUMP_ENTITIES,
	LUMP_PLANES,
	LUMP_NODES,
	LUMP_LEAFS,
	LUMP_VERTS,
	LUMP_EDGES,
	LUMP_FACES
};

struct FileHeader {
	int		version;
	int		identifier;
	FileLump lumps[6];
};

struct FileModel {
	float		minBound[3], maxBound[3];
	float		origin[3];
	int			headNode;
	int			firstFace, numFaces;
};

struct FileEntity {
	float	origin[3];
	int		type;
	int		model;
	int		numKeys[MAX_KEY];
};

struct FilePlane {
	float	normal[3];
	float	dist;
};

struct FileNode {
	int				planeNum;
	int				children[2];
	int				minBound[3], maxBound[3];
};

struct FileLeaf {
	unsigned int	firstFace, numFaces;
	int				minBound[3], maxBound[3];
	
	int				visOffset;
};

struct FileVert {
	float			point[3];
};

struct FileEdge {
	unsigned int	v[2];
};

struct FileFace {
	unsigned int	planeNum;
	int				FirstEdge, numEdges;
	int				material;
};