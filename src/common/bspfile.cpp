#include "cmdlib.h"
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


void SwapBSPFile(bool todisk) {
	int				i, j;
	dmodel_t		*d;

    // models
	for (i = 0; i < nummodels; i++) {
		d = &dmodels[i];

		d->firstface = LittleLong(d->firstface);
		d->numfaces = LittleLong(d->numfaces);
		d->headnode = LittleLong(d->headnode);

		for (j = 0; j < 3; j++) {
			d->mins[j] = LittleFloat(d->mins[j]);
			d->maxs[j] = LittleFloat(d->maxs[j]);
			d->origin[j] = LittleFloat(d->origin[j]);
		}
	}

    //
    // vertexes
    //
	for (i = 0; i < numvertexes; i++) {
		for (j = 0; j < 3; j++) {
			dvertexes[i].point[j] = LittleFloat(dvertexes[i].point[j]);
        }
	}

    //
    // planes
    //
	for (i = 0; i < numplanes ; i++) {
		for (j = 0; j < 3; j++) {
			dplanes[i].normal[j] = LittleFloat(dplanes[i].normal[j]);
        }
        dplanes[i].dist = LittleFloat(dplanes[i].dist);
		dplanes[i].type = LittleLong(dplanes[i].type);
	}

    //
    // texinfos
    //
	for (i = 0; i < numtexinfo; i++)
	{
		for (j = 0; j < 8; j++) {
			texinfo[i].vecs[0][j] = LittleFloat(texinfo[i].vecs[0][j]);
        }
        texinfo[i].flags = LittleLong(texinfo[i].flags);
		texinfo[i].value = LittleLong(texinfo[i].value);
		texinfo[i].nexttexinfo = LittleLong(texinfo[i].nexttexinfo);
	}

    //
    // faces
    //
	for (i = 0; i < numfaces; i++) {
		dfaces[i].texinfo = LittleShort(dfaces[i].texinfo);
		dfaces[i].planenum = LittleShort(dfaces[i].planenum);
		dfaces[i].side = LittleShort(dfaces[i].side);
		dfaces[i].lightofs = LittleLong(dfaces[i].lightofs);
		dfaces[i].firstedge = LittleLong(dfaces[i].firstedge);
		dfaces[i].numedges = LittleShort(dfaces[i].numedges);
	}

    //
    // nodes
    //
	for (i = 0; i < numnodes; i++) {
		dnodes[i].planenum = LittleLong(dnodes[i].planenum);
		for (j=0 ; j<3 ; j++) {
			dnodes[i].mins[j] = LittleShort(dnodes[i].mins[j]);
			dnodes[i].maxs[j] = LittleShort(dnodes[i].maxs[j]);
		}
		dnodes[i].children[0] = LittleLong(dnodes[i].children[0]);
		dnodes[i].children[1] = LittleLong(dnodes[i].children[1]);
		dnodes[i].firstface = LittleShort(dnodes[i].firstface);
		dnodes[i].numfaces = LittleShort(dnodes[i].numfaces);
	}

    //
    // leafs
    //
	for (i=0 ; i<numleafs ; i++) {
		dleafs[i].contents = LittleLong(dleafs[i].contents);
		dleafs[i].cluster = LittleShort(dleafs[i].cluster);
		dleafs[i].area = LittleShort(dleafs[i].area);
		for (j=0 ; j<3 ; j++)
		{
			dleafs[i].mins[j] = LittleShort(dleafs[i].mins[j]);
			dleafs[i].maxs[j] = LittleShort(dleafs[i].maxs[j]);
		}

		dleafs[i].firstleafface = LittleShort(dleafs[i].firstleafface);
		dleafs[i].numleaffaces = LittleShort(dleafs[i].numleaffaces);
		dleafs[i].firstleafbrush = LittleShort(dleafs[i].firstleafbrush);
		dleafs[i].numleafbrushes = LittleShort(dleafs[i].numleafbrushes);
	}

	//
	// leaffaces
	//
	for (i=0 ; i<numleaffaces ; i++) {
		dleaffaces[i] = LittleShort(dleaffaces[i]);
	}

	//
	// leafbrushes
	//
	for (i=0 ; i<numleafbrushes ; i++) {
		dleafbrushes[i] = LittleShort(dleafbrushes[i]);
	}

	//
	// surfedges
	//
	for (i=0 ; i<numsurfedges ; i++) {
		dsurfedges[i] = LittleLong(dsurfedges[i]);
	}

	//
	// edges
	//
	for (i=0 ; i<numedges ; i++) {
		dedges[i].v[0] = LittleShort(dedges[i].v[0]);
		dedges[i].v[1] = LittleShort(dedges[i].v[1]);
	}

	//
	// brushes
	//
	for (i=0 ; i<numbrushes ; i++) {
		dbrushes[i].firstside = LittleLong(dbrushes[i].firstside);
		dbrushes[i].numsides = LittleLong(dbrushes[i].numsides);
		dbrushes[i].contents = LittleLong(dbrushes[i].contents);
	}

	//
	// areas
	//
	for (i=0 ; i<numareas ; i++) {
		dareas[i].numareaportals = LittleLong(dareas[i].numareaportals);
		dareas[i].firstareaportal = LittleLong(dareas[i].firstareaportal);
	}

	//
	// areasportals
	//
	for (i=0 ; i<numareaportals ; i++) {
		dareaportals[i].portalnum = LittleLong(dareaportals[i].portalnum);
		dareaportals[i].otherarea = LittleLong(dareaportals[i].otherarea);
	}

	//
	// brushsides
	//
	for (i=0 ; i<numbrushsides ; i++) {
		dbrushsides[i].planenum = LittleShort(dbrushsides[i].planenum);
		dbrushsides[i].texinfo = LittleShort(dbrushsides[i].texinfo);
	}

	//
	// visibility
	//
	if (todisk) {
		j = dvis->numclusters;
	}
	else {
		j = LittleLong(dvis->numclusters);
	}
	dvis->numclusters = LittleLong(dvis->numclusters);
	for (i=0 ; i<j ; i++) {
		dvis->bitofs[i][0] = LittleLong(dvis->bitofs[i][0]);
		dvis->bitofs[i][1] = LittleLong(dvis->bitofs[i][1]);
	}
}

int CopyLump(int lump, void *dest, int size) {
	int		length, ofs;

	length = header->lumps[lump].filelen;
	ofs = header->lumps[lump].fileofs;

	if (length % size) {
		Error("LoadBSPFile: odd lump size");
    }

	memcpy(dest, (unsigned char *)header + ofs, length);

	return length / size;
}

void	LoadBSPFile(char *filename) {
	int			i;

    //
    // load the file header
    //
	LoadFile(filename, (void **)&header);

    // swap the header
	for (i=0 ; i< sizeof(dheader_t)/4 ; i++) {
		((int *)header)[i] = LittleLong(((int *)header)[i]);
    }

	if (header->ident != IDBSPHEADER) {
		Error ("%s is not a IBSP file", filename);
    }
	if (header->version != BSPVERSION) {
		Error ("%s is version %i, not %i", filename, header->version, BSPVERSION);
    }

	nummodels = CopyLump(LUMP_MODELS, dmodels, sizeof(dmodel_t));
	numvertexes = CopyLump(LUMP_VERTEXES, dvertexes, sizeof(dvertex_t));
	numplanes = CopyLump(LUMP_PLANES, dplanes, sizeof(dplane_t));
	numleafs = CopyLump(LUMP_LEAFS, dleafs, sizeof(dleaf_t));
	numnodes = CopyLump(LUMP_NODES, dnodes, sizeof(dnode_t));
	numtexinfo = CopyLump(LUMP_TEXINFO, texinfo, sizeof(texinfo_t));
	numfaces = CopyLump(LUMP_FACES, dfaces, sizeof(dface_t));
	numleaffaces = CopyLump(LUMP_LEAFFACES, dleaffaces, sizeof(dleaffaces[0]));
	numleafbrushes = CopyLump(LUMP_LEAFBRUSHES, dleafbrushes, sizeof(dleafbrushes[0]));
	numsurfedges = CopyLump(LUMP_SURFEDGES, dsurfedges, sizeof(dsurfedges[0]));
	numedges = CopyLump(LUMP_EDGES, dedges, sizeof(dedge_t));
	numbrushes = CopyLump(LUMP_BRUSHES, dbrushes, sizeof(dbrush_t));
	numbrushsides = CopyLump(LUMP_BRUSHSIDES, dbrushsides, sizeof(dbrushside_t));
	numareas = CopyLump(LUMP_AREAS, dareas, sizeof(darea_t));
	numareaportals = CopyLump(LUMP_AREAPORTALS, dareaportals, sizeof(dareaportal_t));

	visdatasize = CopyLump(LUMP_VISIBILITY, dvisdata, 1);
	lightdatasize = CopyLump(LUMP_LIGHTING, dlightdata, 1);
	entdatasize = CopyLump(LUMP_ENTITIES, dentdata, 1);

	CopyLump(LUMP_POP, dpop, 1);

	free(header);		// everything has been copied out

	//
	// swap everything
	//
	SwapBSPFile(false);
}