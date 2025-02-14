#include "polylib.h"
#include "cmdlib.h"

void pwinding(winding_t *w) {
        int                     i;
        for (i = 0; i < w->numpoints; i++) {
                printf ("(%f, %f, %f)\n", w->p[i][0], w->p[i][1],w->p[i][2]);
        }
}

winding_t *winding_pool[MAX_POINTS_ON_WINDING+4];

/*
================
AllocWinding
================
*/

winding_t *AllocWinding(int points) {
        winding_t               *w;

        if (winding_pool[points]) {
                w = winding_pool[points];
                winding_pool[points] = w->next;
        }
        else {
                w = (winding_t *)malloc(sizeof(*w));
                w->p = (vec3_t *)calloc(points, sizeof(vec3_t));
        }

        w->numpoints = 0;
        w->maxpoints = points;
        w->next = NULL;
        return w;
}

void FreeWinding(winding_t *w) {
        if (w->numpoints == 0xdeaddead)
                printf("FreeWinding: freed a freed winding");
        
        w->numpoints = 0xdeaddead; //tombstone flag
        w->next = winding_pool[w->maxpoints];
        winding_pool[w->maxpoints] = w;
}

void RemoveColinearPoints (winding_t *w) {

}