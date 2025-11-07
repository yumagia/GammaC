#ifndef BSP_FILE_INCLUDED
#define BSP_FILE_INCLUDED

#include "Math.h"

struct EntityPair {
    EntityPair  *next;
    const char *key, *value;
};

struct Entity {
    Vec3f       origin;
    int         firstBrush, numBrushes;
    EntityPair  *entityPairs;

    // Only used for func_areaportals
    int         areaPortaNum;
    int         portalAreas[2];
};

#endif