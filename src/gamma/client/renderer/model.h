#include "shared.h"

#include <string>

typedef struct vert_s {
	vec3_t		position;
} vert_t;

typedef struct poly_s {
	struct poly_s	*chain;
	int				numverts;
	vert_t			*vert;
} poly_t;

typedef struct surface_s {
	plane_t			*plane;
	int				flags;

	poly_t			*poly;
} surface_t;

typedef struct node_s {
	// Common with leaf
	int			contents;

	vec3_t		mins, maxs;

	struct node_s	*parent;

	// node specific
	plane_t		*plane;
	struct node_s	*children[2];

	int				firstsurface;
	int				numsurfaces;
} node_t;

typedef struct leaf_s {
	// Common with node
	int			contents;

	vec3_t		mins, maxs;

	struct node_s *parent;

	// leaf specific
	int			cluster;
	int			area;

	surface_t	**firstmarksurface;
	int			nummarksurfaces;
} leaf_t;

typedef struct {
	vec3_t		mins, maxs;
	vec3_t		origin;
	float		radius;
	int			headnode;
	int			visleafs;
	int			firstface, numfaces;
} model_t;




/**=============================================
 * World model defs
 * 
 * =============================================
 */

typedef struct level_s {
	std::string		name;

	vec3_t			mins, maxs;
	float			radius;
} level_t;