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

