#include "gbsp.h"

#define USE_HASHING

#define	INTEGRAL_EPSILON	0.01
#define	POINT_EPSILON		0.5
#define	OFF_EPSILON			0.5

int c_merge;
int	c_subdivide;

int	c_totalverts;
int	c_uniqueverts;
int	c_degenerate;
int	c_tjunctions;
int	c_faceoverflows;
int	c_facecollapse;
int	c_badstartverts;

#define MAX_SUPERVERTS	512
int	superverts[MAX_SUPERVERTS];
int	numsuperverts;

face_t		*edgefaces[MAX_MAP_EDGES][2];
int		firstmodeledge = 1;
int		firstmodelface;

int	c_tryedges;

vec3_t	edge_dir;
vec3_t	edge_start;
vec_t	edge_len;

int		num_edge_verts;
int		edge_verts[MAX_MAP_VERTS];

float	subdivide_size = 240;