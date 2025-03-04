#pragma once

typedef struct epair_s {
	epair_t			*next;
	char			*key;
	char			*val;
} epair_t;

typedef struct entity_s {
	vec3_t			origin;
	int				firstbrush;
	int				numbrushes;
	epair_t			*epairs;
} entity_t;

#define	MAX_MAP_BRUSHES		8192

#define MAX_MAP_PLANES		65536
#define	MAX_MAP_NODES		65536
#define	MAX_MAP_BRUSHSIDES	65536
#define	MAX_MAP_LEAFS		65536
#define	MAX_MAP_VERTS		65536
#define	MAX_MAP_FACES		65536
#define	MAX_MAP_LEAFFACES	65536
#define	MAX_MAP_LEAFBRUSHES	65536
#define	MAX_MAP_PORTALS		65536
#define	MAX_MAP_EDGES		128000

#define	MAX_MAP_VISIBILITY	0x100000