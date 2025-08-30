#include "gfiles.h"


extern	int			nummodels;
extern	dmodel_t	dmodels[MAX_MAP_MODELS];

extern	int			visdatasize;
extern	unsigned char		dvisdata[MAX_MAP_VISIBILITY];
extern	dvis_t		*dvis;

extern	int			lightdatasize;
extern	unsigned char		dlightdata[MAX_MAP_LIGHTING];

extern	int			entdatasize;
extern	std::string			dentdata;

extern	int			numleafs;
extern	dleaf_t		dleafs[MAX_MAP_LEAFS];

extern	int			numplanes;
extern	dplane_t	dplanes[MAX_MAP_PLANES];

extern	int			numvertexes;
extern	dvertex_t	dvertexes[MAX_MAP_VERTS];

extern	int			numnodes;
extern	dnode_t		dnodes[MAX_MAP_NODES];

extern	int			numtexinfo;
extern	texinfo_t	texinfo[MAX_MAP_TEXINFO];

extern	int			numfaces;
extern	dface_t		dfaces[MAX_MAP_FACES];

extern	int			numedges;
extern	dedge_t		dedges[MAX_MAP_EDGES];

extern	int			numleaffaces;
extern	unsigned short	dleaffaces[MAX_MAP_LEAFFACES];

extern	int			numleafbrushes;
extern	unsigned short	dleafbrushes[MAX_MAP_LEAFBRUSHES];

extern	int			numsurfedges;
extern	int			dsurfedges[MAX_MAP_SURFEDGES];

extern	int			numareas;
extern	darea_t		dareas[MAX_MAP_AREAS];

extern	int			numareaportals;
extern	dareaportal_t	dareaportals[MAX_MAP_AREAPORTALS];

extern	int			numbrushes;
extern	dbrush_t	dbrushes[MAX_MAP_BRUSHES];

extern	int			numbrushsides;
extern	dbrushside_t	dbrushsides[MAX_MAP_BRUSHSIDES];

extern	unsigned char		dpop[256];

void	LoadBSPFile(char *filename);
void	WriteBSPFile(std::string filename);

typedef struct epair_s {
	struct epair_s	*next;
	std::string		key;
	std::string		val;
} epair_t;

typedef struct entity_s {
	vec3_t			origin;
	int				firstbrush;
	int				numbrushes;
	epair_t			*epairs;

	// Only valid for func_areaportals
	int			areaportalnum;
	int			portalareas[2];
} entity_t;

extern	int			num_entities;
extern	entity_t	entities[MAX_MAP_ENTITIES];

void	ParseEntities(void);
void	UnparseEntities(void);

void	SetKeyValue(entity_t *ent, std::string key, std::string val);
std::string		ValueForKey(entity_t *ent, std::string key);

vec_t	FloatForKey(entity_t *ent, std::string key);
void	GetVectorForKey(entity_t *ent, std::string key, vec3_t vec);

epair_t *ParseEpair(void);

void PrintEntity(entity_t *ent);