#pragma once

#define NUM_LUMPS			10

// MAP UPPER BOUNDS
#define MAX_MAP_MODELS		256
#define MAX_MAP_ENTITIES	512
#define MAX_MAP_PLANES		16384

#define MAX_MAP_NODES		32768
#define MAX_MAP_LEAFS		32768
#define MAX_MAP_LEAF_FACES	32768
#define MAX_MAP_VERTS		65536
#define MAX_MAP_FACE_VERTS	65536
#define MAX_MAP_FACES		32768
#define MAX_MAP_MATERIALS	512
#define MAX_MAP_TEX_INFOS	32768
#define MAX_LIGHTMAP		131072
#define MAX_MAP_LIGHT_BASES	65536


#define	MAX_KEY		16
#define	MAX_VALUE	512

// MISC MAP DEFS
#define BOUND_PADDING 16
#define MAX_WINDING 32


struct FileLump {
	int		offset, length;
};

enum {
	LUMP_MODELS,
	LUMP_ENTITIES,
	LUMP_PLANES,
	LUMP_NODES,
	LUMP_LEAFS,
	LUMP_LEAFFACES,
	LUMP_VERTS,
	LUMP_FACE_VERTS,
	LUMP_FACES,
	LUMP_MATERIALS
};

struct FileHeader {
	int		version;
	int		identifier;
	FileLump lumps[NUM_LUMPS];
};

struct FileModel {
	float		origin[3];
	int			headNode;
	int			firstFace, numFaces;
	float		minBound[3], maxBound[3];
};

struct FileEntity {
	float	origin[3];
	int		type;
	int		model;
	int		firstKey, numKeys;
};

struct FilePlane {
	float	normal[3];
	float	dist;
};

struct FileNode {
	int				planeNum;
	int				children[2];
	int				firstFace, numFaces;
	int				minBound[3], maxBound[3];
};

struct FileLeaf {
	unsigned int	firstLeafFace, numLeafFaces;
	int				minBound[3], maxBound[3];
	
	int				visOffset;
};

struct FileVert {
	float			point[3];
};

struct FileFace {
	unsigned int	planeNum;
	unsigned int	firstVert, numVerts;

	int				textInfo;

	int				lightMapOffset;
};

struct FileTexInfo {
	float			uAxis[3];
	float			uOffset;

	float			vAxis[3];
	float			vOffset;

	int				material;
};

struct FileLighting {
	float			color[3];
	float			hBasis[6];
};

struct FileMaterial {
	float			diffuse[3];
	float			specular[3];
	float			emissive[3];

	float			specCoeff;
};

struct BspFile {
	FileHeader		fileHeader;
	FileModel		fileModels[MAX_MAP_MODELS];
	FileEntity		fileEntities[MAX_MAP_ENTITIES];
	FilePlane		filePlanes[MAX_MAP_PLANES];
	FileNode		fileNodes[MAX_MAP_NODES];
	FileLeaf		fileLeafs[MAX_MAP_LEAFS];
	unsigned int	fileLeafFaces[MAX_MAP_LEAF_FACES];
	FileVert		fileVerts[MAX_MAP_VERTS];
	unsigned int	fileFaceVerts[MAX_MAP_FACE_VERTS];
	FileFace		fileFaces[MAX_MAP_FACES];
	FileMaterial	fileMaterials[MAX_MAP_MATERIALS];

	FileTexInfo		fileTexInfos[MAX_MAP_TEX_INFOS];
	
	unsigned int	lightMap[MAX_LIGHTMAP];
	FileLighting	fileLightBases[MAX_MAP_LIGHT_BASES];

	bool			valid = false;
};