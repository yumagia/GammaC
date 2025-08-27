
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

void AddBrushBevels(mapbrush_t *b) {
	int		axis, dir;
	int		i, j, k, l, order;
	side_t	sidetemp;
	brush_texture_t	tdtemp;
	side_t	*s, *s2;
	vec3_t	normal;
	float	dist;
	winding_t	*w, *w2;
	vec3_t	vec, vec2;
	float	d;

	// First, add the axial bevels
	order = 0;
	for(axis = 0; axis < 3; axis++) {
		for(dir = -1; dir <= 1; dir+=2, order++) {
			for(i = 0, s = b->original_sides; i < b->numsides; i++, s++) {
				if(mapplanes[s->planenum].normal[axis] == dir) {
					break;
				}
			}

			if(i == b->numsides) {
				if(nummapbrushsides == MAX_MAP_BRUSHSIDES) { 
					Error("MAX_MAP_BRUSHSIDES");
				}
				nummapbrushsides++;
				b->numsides++;
				VectorClear(normal);
				normal[axis] = dir;
				if(dir == 1) {
					dist = b->maxs[axis];
				}
				else {
					dist = -b->mins[axis];
				}
				s->planenum = FindFloatPlane(normal, dist);
				s->texinfo = b->original_sides[0].texinfo;
				s->contents = b->original_sides[0].contents;
				s->bevel = true;
				c_boxbevels++;
			}
			// Swap the plane if it is not in the canonical order
			if(i != order) {
				sidetemp = b->original_sides[order];
				b->original_sides[order] = b->original_sides[i];
				b->original_sides[i] = sidetemp;

				j = b->original_sides - brushsides;
				tdtemp = side_brushtextures[j+order];
				side_brushtextures[j+order] = side_brushtextures[j+i];
				side_brushtextures[j+i] = tdtemp;
			}
		}
	}

	// Add the edge bevels now
	if(b->numsides == 6) {
		return;		// Purely Axial
	}

	// Test the non-axial plane edges
	for(i = 6; i < b->numsides; i++) {
		s = b->original_sides + i;
		w = s->winding;
		if(!w) {
			continue;
		}
		for(j = 0; j < w->numpoints; j++) {
			k = (j+1)%w->numpoints;
			VectorSubtract(w->p[j], w->p[k], vec);
			if(VectorNormalize(vec, vec) < 0.5) {
				continue;
			}
			SnapVector(vec);
			for(k = 0; k < 3; k++) {
				if(vec[k] == -1 || vec[k] == 1) {
					break;	//axial
				}
			}
			if(k != 3) {
				continue;	// Test only non-axial ones
			}

			// Try the six possible slanted axials from this edge
			for(axis = 0; axis < 3; axis++) {
				for(dir = -1; dir <= 1; dir+=2) {
					// Construct a plane
					VectorClear(vec2);
					vec2[axis] = dir;
					CrossProduct(vec, vec2, normal);
					if(VectorNormalize(normal, normal) < 0.5) {
						continue;
					}
					dist = DotProduct(w->p[j], normal);

					// If all points on all sides are behind this
					// plane, it is a proper edge bevel
					for(k = 0; k < b->numsides; k++) {
						// If this plane has already been used, skip it
						if(PlaneEqual(&mapplanes[b->original_sides[k].planenum]
						, normal, dist)) {
							break;
						}

						w2 = b->original_sides[k].winding;
						if(!w2) {
							continue;
						}
						for(l = 0; l < w2->numpoints; l++) {
							d = DotProduct(w2->p[l], normal) - dist;
							if(d > 0.1) {
								break; // Point is in front
							}
							if(l != w2->numpoints) {
								break;
							}
						}

						if(k != b->numsides) {
							continue;	// It wasn't part of the outer hull
						}
						// Add this plane now
						if(nummapbrushes == MAX_MAP_BRUSHSIDES) {
							Error("MAX_MAP_BRUSHSIDES");
						}
						nummapbrushsides++;
						s2 = &b->original_sides[b->numsides];
						s2->planenum = FindFloatPlane(normal, dist);
						s2->texinfo = b->original_sides[0].texinfo;
						s2->contents = b->original_sides[0].contents;
						s2->bevel = true;
						c_edgebevels++;
						b->numsides++;
					}
				}
			}
		}
	}
}

bool MakeBrushWindings(mapbrush_t *ob) {
	int			i, j;
	winding_t	*w;
	side_t		*side;
	plane_t		*plane;

	ClearBounds(ob->mins, ob->maxs);

	for(i = 0; i < ob->numsides; i++) {
		plane = &mapplanes[ob->original_sides[i].planenum];
		w = BaseWindingForPlane(plane->normal, plane->dist);
		for(j = 0; j < ob->numsides && w; j++) {
			if(i == j) {
				continue;
			}
			if(ob->original_sides[j].bevel) {
				continue;
			}
			plane = &mapplanes[ob->original_sides[j].planenum^1];
			ChopWindingInPlace(&w, plane->normal, plane->dist, 0);	//CLIP_EPSILON
		}

		side = &ob->original_sides[i];
		side->winding = w;
		if(w) {
			side->visible = true;
			for(j = 0; j < w->numpoints; j++) {
				AddPointToBounds(w->p[j], ob->mins, ob->maxs);
			}
		}
	}

	for(i = 0; i < 3; i++) {
		if(ob->mins[0] < -4096 || ob->maxs[0] > 4096) {
			printf("entity %i, brush %i: no visible sies on brush\n", ob->entitynum, ob->brushnum);
		}
	}

	return true;
}

void ParseBrush(entity_t *mapent) {
	mapbrush_t		*b;
	int			i, j, k;
	int			mt;
	side_t		*side, *s2;
	int			planenum;
	brush_texture_t td;
	int			planepts[3][3];

	if(nummapbrushes == MAX_MAP_BRUSHES) {
		Error("nummapbrushes == MAX_MAP_BRUSHES");
	}

	b = &mapbrushes[nummapbrushes];
	b->original_sides = &brushsides[nummapbrushsides];
	b->entitynum = num_entities-1;
	b->brushnum = nummapbrushes - mapent->firstbrush;

	do {
		if(!GetToken(true)) {
			break;
		}
		if(token == "}") {
			break;
		}

		if(nummapbrushsides == MAX_MAP_BRUSHSIDES) {
			Error("MAX_MAP_BRUSHSIDES");
		}
		side = &brushsides[nummapbrushsides];

		for(i = 0; i < 3; i++) {
			if(i != 0) {
				GetToken(true);
			}
			if(token != "(") {
				Error("parsing brush");
			}
			
			for(j = 0; j < 3; j++) {
				GetToken(false);
				planepts[i][j] = stoi(token);
			}
			
			GetToken(false);
			if (token != ")") {
				Error("parsing brush");
			}
		}

		GetToken(false);
		td.name = token;

		GetToken(false);
		td.shift[0] = stoi(token);
		GetToken(false);
		td.shift[1] = stoi(token);
		GetToken(false);
		td.rotate = stoi(token);
		GetToken(false);
		td.scale[0] = stoi(token);
		GetToken(false);
		td.scale[1] = stoi(token);

		//mt = FindMiptex(td.name);
		//td.flags = textureref[mt].flags;
		//td.value = textureref[mt].value;
		//side->contents = textureref[mt].contents;
		//side->surf = td.flags = textureref[mt].flags;

		if(TokenAvailable()) {
			GetToken(false);
			side->contents = stoi(token);
			GetToken(false);
			side->surf = td.flags = stoi(token);
			GetToken(false);
			td.value = stoi(token);
		}

		// Translucent objects are automatically classified as detail
		if(side->surf & (SURF_TRANS33|SURF_TRANS66)) {
			side->contents |= CONTENTS_DETAIL;
		}
		if(side->surf & (CONTENTS_PLAYERCLIP|CONTENTS_NPCCLIP)) {
			side->contents |= CONTENTS_DETAIL;
		}
		//if(fulldetail) {
		//	side->contents &= ~CONTENTS_DETAIL;
		//}
		if(!(side->contents & ((LAST_VISIBLE_CONTENTS-1) 
			| CONTENTS_PLAYERCLIP|CONTENTS_NPCCLIP|CONTENTS_MIST))) {
			side->contents |= CONTENTS_SOLID;
		}

		if(side->surf & (SURF_HINT|SURF_SKIP)) {
			side->contents = 0;
			side->surf &= ~CONTENTS_DETAIL;
		}

		planenum = PlaneFromPoints(planepts[0], planepts[1], planepts[2]);
		if(planenum == -1) {
			std::cout << "Entity " << b->entitynum << ", Brush " 
				<< b->brushnum << ": plane with no normal" << std::endl;
			continue;
		}

		for(k = 0; k < b->numsides; k++) {
			s2 = b->original_sides + k;
			if(s2->planenum == planenum) {
				std::cout << "Entity " << b->entitynum << ", Brush " 
					<< b->brushnum << ": duplicate plane" << std::endl;
				break;
			}
			if(s2->planenum == (planenum^1)) {
				std::cout << "Entity " << b->entitynum << ", Brush " 
					<< b->brushnum << ": mirrored plane" << std::endl;
				break;
			}
		}
		if(k != b->numsides) {
			continue;
		}

		side = b->original_sides + b->numsides;
		side->planenum = planenum;
		//side->texinfo = TexinfoForBrushTexture(&mapplanes[planenum],
		//	&td, vec3_origin);
		
		// Save the td off in case there is an origin brush and we
		// have to recalculate the texinfo
		//side_brushtextures[nummapbrushsides] = td;

		nummapbrushsides++;
		b->numsides++;
	} while (1);

	b->contents = BrushContents(b);

	//if(nodetail && (b->contents & CONTENTS_DETAIL)) {
	//	b->numsides = 0;
	//	return;
	//}

	//if(nowater && (b->contents & (CONTENTS_LAVA | CONTENTS_SLIME | CONTENTS_WATER))) {
	//	b->numsides = 0;
	//	return;
	//}

	MakeBrushWindings(b);

	if(b->contents & (CONTENTS_PLAYERCLIP|CONTENTS_NPCCLIP)) {
		c_clipbrushes++;
		for(i = 0; i < b->numsides; i++) {
			b->original_sides[i].texinfo = TEXINFO_NODE;
		}
	}

	if(b->contents & CONTENTS_ORIGIN) {
		std::string		string;
		vec3_t			origin;

		if(num_entities == 1) {
			Error("Entity %i, Brush %i: origin brushes not allowed in world"
				, b->entitynum, b->brushnum);
		}

		VectorAdd(b->mins, b->maxs, origin);
		VectorScale(origin, 0.5, origin);
		string = (int)origin[0] + " ";
		string += (int)origin[1] + " ";
		string += (int)origin[2];
		SetKeyValue(&entities[b->entitynum], "origin", string);

		VectorCopy(origin, entities[b->entitynum].origin);

		// Don't keep this brush
		b->numsides = 0;

		return;
	}

	AddBrushBevels(b);

	nummapbrushes++;
	mapent->numbrushes++;
}

bool	ParseMapEntity(void) {
	entity_t	*mapent;
	epair_t		*e;
	side_t		*s;
	int			i, j;
	int			startbrush, startsides;
	vec_t		newdist;
	mapbrush_t	*b;

	if(!GetToken(true)) {
		return false;
	}
	if(token != "{") {
		Error("ParseEntity: { not found");
	}

	if(num_entities == MAX_MAP_ENTITIES) {
		Error("num_entities == MAX_MAP_ENTITIES");
	}

	startbrush = nummapbrushes;
	startsides = nummapbrushsides;

	mapent = &entities[num_entities];
	num_entities++;
	memset(mapent, 0, sizeof(entity_t));
	mapent->firstbrush = nummapbrushes;
	mapent->numbrushes = 0;

	do {
		if(!GetToken(true)) {
			Error("ParseEntity: EOF without closing brace");
		}
		if(token == "}") {
			break;
		}
		if(token == "{") {
			ParseBrush(mapent);
		}
		else {
			e = ParseEpair();
			e->next = mapent->epairs;
			mapent->epairs = e;
		}
	} while(1);

	// Offest all planes and texinfo by the origin brush, if it exists
	GetVectorForKey(mapent, "origin", mapent->origin);
	if (mapent->origin[0] || mapent->origin[1] || mapent->origin[2]) {
		for(i = 0; i < mapent->numbrushes; i++) {
			b = &mapbrushes[mapent->firstbrush + i];
			for(j = 0; j < b->numsides; j++) {
				s = &b->original_sides[j];
				newdist = mapplanes[s->planenum].dist -
					DotProduct(mapplanes[s->planenum].normal, mapent->origin);
				s->planenum = FindFloatPlane(mapplanes[s->planenum].normal, newdist);
				//s->texinfo = TexinfoForBrushTexture(&mapplanes[s->planenum],
				//	&side_brushtextures[s-brushsides], mapent->origin);
			}
			MakeBrushWindings(b);
		}
	}


	return true;
}

void LoadMapFile(fs::path inputpath) {
	int		i;

	std::cout << "LOADING MAP FILE...\n";

	LoadScriptFile(inputpath);

	nummapbrushsides = 0;
	num_entities = 0;

	while(ParseMapEntity()) {
	}

	ClearBounds(map_mins, map_maxs);
	for(i = 0; i < entities[0].numbrushes; i++) {
		if(mapbrushes[i].mins[0] > 4096) {
			continue;		//no valid points
		}
		AddPointToBounds(mapbrushes[i].mins, map_mins, map_maxs);
		AddPointToBounds(mapbrushes[i].maxs, map_mins, map_maxs);
	}

	std::cout << nummapbrushes << " brushes" << std::endl;
	std::cout << c_clipbrushes << " clipbrushes" << std::endl;
	std::cout << nummapbrushsides << " total sides" << std::endl;
	std::cout << c_boxbevels << " boxbevels" << std::endl;
	std::cout << c_edgebevels << " edgebevels" << std::endl;
	std::cout << num_entities << " entities" << std::endl;
	std::cout << nummapplanes << " planes" << std::endl;
	std::cout << c_areaportals << " areaportals" << std::endl;
	std::cout << "Map size: " << map_mins[0] << ", " << map_mins[1] << ", " << map_mins[2] << " to " << map_maxs[0] << ", " << map_maxs[1] << ", " << map_maxs[2] << std::endl;
}

