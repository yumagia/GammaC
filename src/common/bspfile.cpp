#include "cmdlib.h"
#include "scriplib.h"
#include "veclib.h"
#include "bspfile.h"

void GetLeafNums(void);

int			nummodels;
dmodel_t	dmodels[MAX_MAP_MODELS];

int			visdatasize;
unsigned char	    dvisdata[MAX_MAP_VISIBILITY];
dvis_t		*dvis = (dvis_t *)dvisdata;

int			lightdatasize;
unsigned char	    dlightdata[MAX_MAP_LIGHTING];

int			entdatasize;
char		dentdata[MAX_MAP_ENTSTRING];

int			numleafs;
dleaf_t		dleafs[MAX_MAP_LEAFS];

int			numplanes;
dplane_t	dplanes[MAX_MAP_PLANES];

int			numvertexes;
dvertex_t	dvertexes[MAX_MAP_VERTS];

int			numnodes;
dnode_t		dnodes[MAX_MAP_NODES];

int			numtexinfo;
texinfo_t	texinfo[MAX_MAP_TEXINFO];

int			numfaces;
dface_t		dfaces[MAX_MAP_FACES];

int			numedges;
dedge_t		dedges[MAX_MAP_EDGES];

int			numleaffaces;
unsigned short		dleaffaces[MAX_MAP_LEAFFACES];

int			numleafbrushes;
unsigned short		dleafbrushes[MAX_MAP_LEAFBRUSHES];

int			numsurfedges;
int			dsurfedges[MAX_MAP_SURFEDGES];

int			numbrushes;
dbrush_t	dbrushes[MAX_MAP_BRUSHES];

int			numbrushsides;
dbrushside_t	dbrushsides[MAX_MAP_BRUSHSIDES];

int			numareas;
darea_t		dareas[MAX_MAP_AREAS];

int			numareaportals;
dareaportal_t	dareaportals[MAX_MAP_AREAPORTALS];

unsigned char	    dpop[256];


dheader_t	*header;

int			num_entities;
entity_t	entities[MAX_MAP_ENTITIES];


epair_t *ParseEpair(void) {
	epair_t *e;

	e = (epair_t*)calloc(sizeof(epair_t), 1);

	if(token.length() >= MAX_KEY - 1) {
		Error("ParseEpair: token too long");
	}
}

void	LoadBSPFile(char *filename) {
	int			i;


}

void	WriteBSPFile (char *filename) {

}