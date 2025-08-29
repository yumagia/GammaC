#include "gbsp.h"

int		c_nofaces;
int		c_facenodes;

// Only save out planes that are actually used as nodes!

int		planeused[MAX_MAP_PLANES];

/**=============================================
 * Emit Functions
 * 
 * =============================================
 */

void EmitPlanes(void) {
	int			i;
	dplane_t	*dp;
	plane_t		*mp;
	int		planetranslate[MAX_MAP_PLANES];

	mp = mapplanes;
	for(i = 0; i < nummapplanes; i++, mp++) {
		dp = &dplanes[numplanes];
		planetranslate[i] = numplanes;
		VectorCopy(mp->normal, dp->normal);
		dp->dist = mp->dist;
		dp->type = mp->type;
		numplanes++;
	}
}

void EmitBrushes(void) {
	int			i, j, bnum, s, x;
	dbrush_t	*db;
	mapbrush_t		*b;
	dbrushside_t	*cp;
	vec3_t		normal;
	vec_t		dist;
	int			planenum;

	numbrushsides = 0;
	numbrushes = nummapbrushes;
	for(bnum = 0; bnum < nummapbrushes; bnum++) {
		b = &mapbrushes[bnum];
		db = &dbrushes[bnum];

		db->contents = b->contents;
		db->firstside = numbrushsides;
		db->numsides = b->numsides;
		for(j = 0; j < b->numsides; j++) {
			if(numbrushsides == MAX_MAP_BRUSHSIDES) {
				Error("MAX_MAP_BRUSHSIDES");
			}
			cp = &dbrushsides[numbrushsides];
			numbrushsides++;
			cp->planenum = b->original_sides[j].planenum;
			cp->texinfo = b->original_sides[j].texinfo;
		}

		// Add any axis planes not contained in the brush
		// to bevel off corners
		for(x = 0; x < 3; x++) {
			for(s = -1; s <= 1; s += 2) {
				// Add the plane
				VectorCopy(vec3_origin, normal);
				normal[x] = s;
				if(s == -1) {
					dist = -b->mins[x];
				}
				else {
					dist = b->maxs[x];
				}
				planenum = FindFloatPlane(normal, dist);
				for(i = 0; i < b->numsides; i++) {
					if(b->original_sides[i].planenum == planenum) {
						break;
					}
				}
				if(i == b->numsides) {
					if(numbrushsides >= MAX_MAP_BRUSHSIDES) {
						Error("MAX_MAP_BRUSHSIDES");
					}

					dbrushsides[numbrushsides].planenum = planenum;
					dbrushsides[numbrushsides].texinfo =
						dbrushsides[numbrushsides - 1].texinfo;
					numbrushsides++;
					db->numsides++;
				}
			}
		}
	}
}

void EmitMarkFace(dleaf_t *leaf_p, face_t *f) {
	int			i;
	int			facenum;

	while(f->merged) {
		f = f->merged;
	}

	if(f->split[0]) {
		EmitMarkFace(leaf_p, f->split[0]);
		EmitMarkFace(leaf_p, f->split[1]);
		return;
	}

	facenum = f->outputnumber;
	if(facenum == -1) {					// Degenerate face
		return;
	}
	if(facenum < 0 || facenum >= numfaces) {
		Error("Bad leafface");
	}
	for(i = leaf_p->firstleafface; i<numleaffaces; i++) {
		if(dleaffaces[i] == facenum) {	// Merged out face
			break;
		}
	}
	if(i == numleaffaces) {
		if(numleaffaces >= MAX_MAP_LEAFFACES) {
			Error("MAX_MAP_LEAFFACES");
		}

		dleaffaces[numleaffaces] =  facenum;
		numleaffaces++;
	}
}

void EmitLeaf(node_t *node) {
	dleaf_t		*leaf_p;
	portal_t	*p;
	int			s;
	face_t		*f;
	bspbrush_t	*b;
	int			i;
	int			brushnum;

	// Emit a leaf
	if(numleafs >= MAX_MAP_LEAFS) {
		Error("MAX_MAP_LEAFS");
	}

	leaf_p = &dleafs[numleafs];
	numleafs++;

	leaf_p->contents = node->contents;
	leaf_p->cluster = node->cluster;
	leaf_p->area = node->area;

	// Write bounding box info	
	VectorCopy(node->mins, leaf_p->mins);
	VectorCopy(node->maxs, leaf_p->maxs);
	
	// Write the leafbrushes
	leaf_p->firstleafbrush = numleafbrushes;
	for(b = node->brushlist; b; b = b->next) {
		if(numleafbrushes >= MAX_MAP_LEAFBRUSHES) {
			Error("MAX_MAP_LEAFBRUSHES");
		}

		brushnum = b->original - mapbrushes;
		for(i = leaf_p->firstleafbrush; i < numleafbrushes; i++) {
			if(dleafbrushes[i] == brushnum) {
				break;
			}
		}
		if(i == numleafbrushes) {
			dleafbrushes[numleafbrushes] = brushnum;
			numleafbrushes++;
		}
	}
	leaf_p->numleafbrushes = numleafbrushes - leaf_p->firstleafbrush;

	// Write the leaffaces
	if(leaf_p->contents & CONTENTS_SOLID) {
		return;			// No leaffaces in solids
	}

	leaf_p->firstleafface = numleaffaces;

	for(p = node->portals; p; p = p->next[s]) {
		s = (p->nodes[1] == node);
		f = p->face[s];
		if(!f) {
			continue;	// not a visible portal
		}

		EmitMarkFace(leaf_p, f);
	}
	
	leaf_p->numleaffaces = numleaffaces - leaf_p->firstleafface;

}

void EmitFace(face_t *f) {
	dface_t	*df;
	int		i;
	int		e;

	f->outputnumber = -1;

	if(f->numpoints < 3) {
		return;		// Degenerated
	}
	if(f->merged || f->split[0] || f->split[1]) {
		return;		// Not a final face
	}

	// Save output number so leaffaces can use
	f->outputnumber = numfaces;

	if(numfaces >= MAX_MAP_FACES) {
		Error ("numfaces == MAX_MAP_FACES");
	}
	df = &dfaces[numfaces];
	numfaces++;

	// Planenum is used by light but not the game
	df->planenum = f->planenum & (~1);
	df->side = f->planenum & 1;

	df->firstedge = numsurfedges;
	df->numedges = f->numpoints;
	df->texinfo = f->texinfo;
	for(i = 0; i < f->numpoints; i++) {
		e = GetEdge2(f->vertexnums[i], f->vertexnums[(i + 1)%f->numpoints], f);
		if(numsurfedges >= MAX_MAP_SURFEDGES) {
			Error ("numsurfedges == MAX_MAP_SURFEDGES");
		}
		dsurfedges[numsurfedges] = e;
		numsurfedges++;
	}
}

int EmitDrawNode_r(node_t *node) {
	dnode_t	*n;
	face_t	*f;
	int		i;

	if(node->planenum == PLANENUM_LEAF) {
		EmitLeaf(node);
		return -numleafs;
	}

	// Emit a node	
	if(numnodes == MAX_MAP_NODES) {
		Error("MAX_MAP_NODES");
	}
	n = &dnodes[numnodes];
	numnodes++;

	VectorCopy(node->mins, n->mins);
	VectorCopy(node->maxs, n->maxs);

	planeused[node->planenum]++;
	planeused[node->planenum^1]++;

	if(node->planenum & 1) {
		Error("WriteDrawNodes_r: odd planenum");
	}
	n->planenum = node->planenum;
	n->firstface = numfaces;

	if(!node->faces) {
		c_nofaces++;
	}
	else {
		c_facenodes++;
	}

	for(f = node->faces; f; f = f->next) {
		EmitFace(f);
	}

	n->numfaces = numfaces - n->firstface;


	// Recursively output the other nodes
	for(i = 0; i < 2; i++) {
		if(node->children[i]->planenum == PLANENUM_LEAF) {
			n->children[i] = -(numleafs + 1);
			EmitLeaf(node->children[i]);
		}
		else {
			n->children[i] = numnodes;	
			EmitDrawNode_r(node->children[i]);
		}
	}

	return n - dnodes;
}


void WriteBSP(node_t *headnode) {
	int		oldfaces;

	c_nofaces = 0;
	c_facenodes = 0;

	std::cout << "--- WriteBSP ---" << std::endl;

	oldfaces = numfaces;
	dmodels[nummodels].headnode = EmitDrawNode_r(headnode);
	EmitAreaPortals(headnode);

	std::cout << c_facenodes << " nodes with faces" << std::endl;
	std::cout << c_nofaces << " nodes without faces" << std::endl;
	std::cout << numfaces - oldfaces << " faces" << std::endl;
}

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

	EmitBrushes();
	EmitPlanes();
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