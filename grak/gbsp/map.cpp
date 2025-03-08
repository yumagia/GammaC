
#include "gbsp.h"

int			nummapbrushes;
mapbrush_t	mapbrushes[MAX_MAP_BRUSHES];

int			nummapbrushsides;
side_t		brushsides[MAX_MAP_SIDES];
brush_texture_t	side_brushtextures[MAX_MAP_SIDES];

int			nummapplanes;
plane_t		mapplanes[MAX_MAP_PLANES];

#define	PLANE_HASHES	1024
plane_t		*planehash[PLANE_HASHES];

vec3_t		map_mins, map_maxs;



int		c_boxbevels;
int		c_edgebevels;

int		c_areaportals;

int		c_clipbrushes;

/**=============================================
 * PLANE FINDING
 * 
 * =============================================
 */

/**
 * @brief Used to figure out how we want to deal with our planes
 */
int	PlaneTypeForNormal(vec3_t normal) {
	vec_t	ax, ay, az;
	
    #define	HASHESPACING    1.0 // Yes, the epsilon should be 1.0
    if (normal[0] == HASHESPACING || normal[0] == -HASHESPACING) {
		return PLANE_X;
    }
	if (normal[1] == HASHESPACING || normal[1] == -HASHESPACING) {
		return PLANE_Y;
    }
	if (normal[2] == HASHESPACING || normal[2] == -HASHESPACING) {
		return PLANE_Z;
    }
		
	ax = fabs(normal[0]);
	ay = fabs(normal[1]);
	az = fabs(normal[2]);
	
	if (ax >= ay && ax >= az) {
		return PLANE_ANYX;
    }
	if (ay >= ax && ay >= az) {
		return PLANE_ANYY;
    }
	return PLANE_ANYZ;
}

    
#define	NORMAL_EPSILON	0.00001
#define	DIST_EPSILON	0.01

/**
 * @brief Finds if a plane is considered to have the same equation
 */
bool    PlaneEqual(plane_t *p, vec3_t normal, vec_t dist) {
	if (
	   fabs(p->normal[0] - normal[0]) < NORMAL_EPSILON
	&& fabs(p->normal[1] - normal[1]) < NORMAL_EPSILON
	&& fabs(p->normal[2] - normal[2]) < NORMAL_EPSILON
	&& fabs(p->dist - dist) < DIST_EPSILON) {
		return true;
    }
	return false;
}


/**
 * @brief Puts a plane into its respective spatial hash
 */
void	AddPlaneToHash(plane_t *p) {
	int		hash;

	hash = (int)fabs(p->dist) / 8;
	hash &= (PLANE_HASHES - 1);

	p->hash_chain = planehash[hash];
	planehash[hash] = p;
}

/**
 * @brief Generates a new arbitrary plane
*/
int CreateNewFloatPlane(vec3_t normal, vec_t dist) {
	plane_t	*p, temp;

	if (VectorLength(normal) < 0.5) {
		Error("FloatPlane: bad normal");
    }
	// create a new plane
	if (nummapplanes+2 > MAX_MAP_PLANES) {
		Error ("MAX_MAP_PLANES");
    }

	p = &mapplanes[nummapplanes];
	VectorCopy(normal, p->normal);
	p->dist = dist;
	p->type = (p+1)->type = PlaneTypeForNormal(p->normal);

	VectorSubtract(vec3_origin, normal, (p+1)->normal);
	(p+1)->dist = -dist;

	nummapplanes += 2;

	// Always put axial planes facing positive first
	if (p->type < 3) {
		if (p->normal[0] < 0 || p->normal[1] < 0 || p->normal[2] < 0) {
			// Flip order
			temp = *p;
			*p = *(p+1);
			*(p+1) = temp;

			AddPlaneToHash(p);
			AddPlaneToHash(p+1);
			return nummapplanes - 1;
		}
	}

	AddPlaneToHash(p);
	AddPlaneToHash(p+1);
	return nummapplanes - 2;
}

/**
 * @brief Snaps a vector to its respective spatial hash
 */
void	SnapVector(vec3_t normal) {
	int		i;

	for (i = 0; i < 3; i++) {
		if (fabs(normal[i] - 1) < NORMAL_EPSILON ) {
			VectorClear(normal);
			normal[i] = 1;
			break;
		}
		if (fabs(normal[i] - -1) < NORMAL_EPSILON ) {
			VectorClear(normal);
			normal[i] = -1;
			break;
		}
	}
}

/**
 * @brief Use snapvector to spatially hash a plane
 */
void	SnapPlane(vec3_t normal, vec_t *dist) {
	SnapVector(normal);

	if (fabs(*dist-rint(*dist)) < DIST_EPSILON)
		*dist = rint(*dist);
}

/**
 * @brief Uses hashing to find a plane, or create a new one
 */
int		FindFloatPlane(vec3_t normal, vec_t dist) {
	int		i;
	plane_t	*p;

	SnapPlane(normal, &dist);
	for (i = 0, p=mapplanes; i < nummapplanes; i++, p++) {
		if (PlaneEqual(p, normal, dist))
			return i;
	}

	return CreateNewFloatPlane(normal, dist);
}

/**
 * @brief Uses the cross product to obtain the plane which defines three points
 */
int PlaneFromPoints(int *p0, int *p1, int *p2) {
	vec3_t	t1, t2, normal;
	vec_t	dist;

	VectorSubtract(p0, p1, t1);
	VectorSubtract(p2, p1, t2);
	CrossProduct(t1, t2, normal);
	VectorNormalize(normal, normal);

	dist = DotProduct(p0, normal);

	return FindFloatPlane(normal, dist);
}

/**=============================================
 * BRUSH STUFF
 * 
 * =============================================
 */

/*
*    / |        ,-.
*   (^  _ `7     \ \
*     `    | \    | |
*     |    ~     / /
*    [][]  f_,  )/
*
*/

 /**
  * @brief Finds what a brush is made of
  */
 int	BrushContents(mapbrush_t *b) {
	int			contents;
	side_t		*s;
	int			i;
	int			trans;

	s = &b->original_sides[0];
	contents = s->contents;
	trans = texinfo[s->texinfo].flags;
	for (i = 1; i < b->numsides; i++, s++) {
		s = &b->original_sides[i];
		trans |= texinfo[s->texinfo].flags;
		if (s->contents != contents) {
			printf("Entity %i, Brush %i: mixed face contents\n"
				, b->entitynum, b->brushnum);
			break;
		}
	}

	// If any side is translucent, mark the contents
	// and change solid to window

	if ( trans & (SURF_TRANS33|SURF_TRANS66) )
	{
		contents |= CONTENTS_TRANSLUCENT;
		if (contents & CONTENTS_SOLID)
		{
			contents &= ~CONTENTS_SOLID;
			contents |= CONTENTS_WINDOW;
		}
	}

	return contents;
}

