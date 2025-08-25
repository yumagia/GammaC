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
std::string			dentdata;

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

void StripTrailing(std::string e) {
	while(!e.empty() && std::isspace(static_cast<unsigned char>(e.back()))) {
		e.pop_back();
	}
}

epair_t *ParseEpair(void) {
	epair_t *e;

	e = (epair_t*)calloc(sizeof(epair_t), 1);

	if(token.length() >= MAX_KEY - 1) {
		Error("ParseEpair: epair key too long");
	}
	e->key = token;
	GetToken(false);
	if(token.length() >= MAX_VALUE - 1) {
		Error("ParseEpair: epair val too long");
	}
	e->val = token;

	StripTrailing(e->key);
	StripTrailing(e->val);

	return e;
}

bool	ParseEntity(void) {
	epair_t		*e;
	entity_t	*mapent;

	if(!GetToken(true)) {
		return false;
	}

	if(token.find("{") == std::string::npos) {
		Error("ParseEntity: { not found");
	}

	if(num_entities == MAX_MAP_ENTITIES) {
		Error("num_entities == MAX_MAP_ENTITIES");
	}

	mapent = &entities[num_entities];
	num_entities++;

	do {
		if(!GetToken(true)) {
			Error("ParseEntity: EOF without closing brace");
		}
		if(token.find("}") != std::string::npos) {
			break;
		}
		e = ParseEpair();
		e->next = mapent->epairs;
		mapent->epairs = e;
	} while(1);

	return true;
}

void ParseEntities(void) {
	num_entities = 0;
	ParseFromMemory(dentdata, entdatasize);

	while(ParseEntity()) {
	}
}

void UnparseEntities(void) {
	epair_t		*ep;
	std::string	line;
	int			i;
	std::string	key, val;

	dentdata = "";


	for(i = 0; i < num_entities; i++) {
		ep = entities[i].epairs;
		if(!ep) {
			continue;
		}

		dentdata += "{\n";

		for(; ep; ep->next) {
			key = ep->key;
			val = ep->val;

			StripTrailing(key);
			StripTrailing(val);

			dentdata += "\"" + key + "\" \"" + val + "\"\n";
		}

		dentdata += "}\n";
	}

	if(dentdata.size() >= MAX_MAP_ENTSTRING) {
		Error("Entity text too long");
	}
}

void PrintEntity(entity_t *ent) {
	epair_t *ep;

	std::cout << "------- entity " << ent << " -------\n" << std::endl;
}

void	LoadBSPFile(char *filename) {
	int			i;


}

void	WriteBSPFile (char *filename) {

}