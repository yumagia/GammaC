#include "gbsp.h"

int		c_nofaces;
int		c_facenodes;

// Only save out planes that are actually used as nodes!

int		planeused[MAX_MAP_PLANES];



/**=============================================
 * 
 * 
 * =============================================
 */

void SetModelNumbers(void) {
	int		i;
	int		models;
	std::string		val;

	models = 1;
	for(i = 1; i < num_entities; i++) {
		if(entities[i].numbrushes) {
			std::cout << val << " " << models << std::endl;
			models++;
			SetKeyValue(&entities[i], "model", val);
		}
	}
}

#define MAX_SWITCHED_LIGHTS 32
void SetLightStyles(void) {
	int		stylenum;
	std::string		t;
	entity_t		*e;
	int		i, j;
	std::string		val;
	std::vector<std::string>	lighttargets;

	stylenum = 0;
	for(i = 1; i < num_entities; i++) {
		e = &entities[i];

		t = ValueForKey(e, "classname");
		if(G_stringcasecomp(t, "light")) {
			continue;
		}
		t = ValueForKey(e, "targetname");
		if(t == "") {
			continue;
		}
	}
}


 /**=============================================
 * FILE WRITING
 * 
 * =============================================
 */

void BeginBSPFile(void) {
	// These values may actually be initialized
	// if the file existed when loaded, so clear them explicitly
	nummodels = 0;
	numfaces = 0;
	numnodes = 0;
	numbrushsides = 0;
	numvertexes = 0;
	numleaffaces = 0;
	numleafbrushes = 0;
	numsurfedges = 0;

	// Edge 0 is not used, because 0 can't be negated
	numedges = 1;

	// Leave vertex 0 as an error
	numvertexes = 1;

	// Leave leaf 0 as an error
	numleafs = 1;
	dleafs[0].contents = CONTENTS_SOLID;
}

void EndBSPFile(void) {
	std::string		path;
	int		len;

	//EmitBrushes();
	//EmitPlanes();
	UnparseEntities();

	path = source + ".bsp";
	std::cout << path << std::endl;
	std::cout << "Writing " << path << std::endl;
	WriteBSPFile(path);
}

int	firstmodelleaf;
extern	int firstmodeledge;
extern	int	firstmodelface;
void BeginModel(void) {
	dmodel_t	*model;
	int			start, end;
	mapbrush_t	*b;
	int			j;
	entity_t	*e;
	vec3_t		mins, maxs;

	if(nummodels == MAX_MAP_MODELS) {
		Error("Error: Reached maximum map models");
	}
	model = &dmodels[nummodels];

	model->firstface = numfaces;

	firstmodelleaf = numleafs;
	firstmodeledge = numedges;
	firstmodelface = numfaces;

	// Bound the entities
	e = &entities[entity_num];

	start = e->firstbrush;
	end = start + e->numbrushes;
	ClearBounds(mins, maxs);

	for(j = start; j < end; j++) {
		b = &mapbrushes[j];
		if(!b->numsides) {
			continue;		// Not a real brush (origin brush)
		}
		AddPointToBounds(b->mins, mins, maxs);
		AddPointToBounds(b->maxs, mins, maxs);
	}

	VectorCopy(mins, model->mins);
	VectorCopy(maxs, model->maxs);
}

void EndModel(void) {
	dmodel_t	*model;

	model = &dmodels[nummodels];

	model->numfaces = numfaces - model->firstface;

	nummodels++;
}