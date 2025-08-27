#include "gbsp.h"

int		c_active_portals;
int		c_peak_portals;
int		c_boundary;
int		c_boundary_sides;

portal_t *AllocPortal(void) {
	portal_t		*p;

	c_active_portals++;

	if(c_active_portals > c_peak_portals) {
		c_peak_portals = c_active_portals;
	}

	p = (portal_t*)calloc(sizeof(portal_t), 1);

	return p;
}

void FreePortal(portal_t *p) {
	if(p->winding) {
		FreeWinding(p->winding);
	}
	c_active_portals--;
	free(p);
}

int VisibleContents(int contents) {
	int		i;

	for(i = 1; i < LAST_VISIBLE_CONTENTS; i <<= 1) {
		if(contents & i) {
			return i;
		}
	}

	return 0;
}

int ClusterContents(node_t *node) {
	int		c1, c2, c;

	if(node->planenum == PLANENUM_LEAF) {
		return node->contents;
	}

	c1 = ClusterContents(node->children[0]);
	c2 = ClusterContents(node->children[1]);
	c = c1 | c2;

	// A cluster may include some solid detail areas, but still
	// permits seeing into
	if(!(c1 & CONTENTS_SOLID) || !(c2 & CONTENTS_SOLID)) {
		c &= ~CONTENTS_SOLID;
	}

	return c;
}

bool Portal_VisFlood(portal_t *p) {
	int     c1, c2;

	if(!p->onnode) {
		return false;       // To global outsideleaf
	}

	c1 = ClusterContents(p->nodes[0]);
	c2 = ClusterContents(p->nodes[1]);
	if(!VisibleContents(c1^c2)) {
		return true;
	}

	if(c1 & (CONTENTS_TRANSLUCENT | CONTENTS_DETAIL)) {
		c1 = 0;
	}
	if(c2 & (CONTENTS_TRANSLUCENT | CONTENTS_DETAIL)) {
		c2 = 0;
	}

	if((c1 | c2) & CONTENTS_SOLID) {
		return false;		// Can't see through solid
	}

	if(!(c1 ^ c2)) {
		return true;		// Identical on both sides
	}

	if(!VisibleContents(c1^c2)) {
		return true;
	}
	return false;
}

bool Portal_EntityFlood(portal_t *p, int s) {
	if(p->nodes[0]->planenum != PLANENUM_LEAF
		|| p->nodes[1]->planenum != PLANENUM_LEAF) {
		Error("Portal_EntityFlood: not a leaf");
	}

	// The algo can never cross to a solid
	if((p->nodes[0]->contents & CONTENTS_SOLID)
		|| (p->nodes[1]->contents & CONTENTS_SOLID)) {
		return false;
	}
	// But it can flood through everything else
	return true;
}

int		c_tinyportals;

void AddPortalToNodes(portal_t *p, node_t *front, node_t *back) {
	if(p->nodes[0] || p->nodes[1]) {
		Error("AddPortalToNode: already incuded");
	}

	p->nodes[0] = front;
	p->next[0] = front->portals;
	front->portals = p;

	p->nodes[1] = back;
	p->next[1] = back->portals;
	back->portals = p;
}

void RemovePortalFromNode(portal_t *portal, node_t *l) {
	portal_t	**pp, *t;

	// Remove reference to the current portal
	pp = &l->portals;
	while(1) {
		t = *pp;
		if(!t) {
			Error("RemovePortalFromNode: portal not in leaf");
		}

		if(t == portal) {
			break;
		}

		if(t->nodes[0] == l) {
			pp = &t->next[0];
		}
		else if(t->nodes[1] == l) {
			pp = &t->next[1];
		}
		else {
			Error("RemovePortalFromNode: portal not bounding leaf");
		}
	}

	if(portal->nodes[0] == l) {
		*pp = portal->next[0];
		portal->nodes[0] = NULL;
	}
	else if(portal->nodes[1] == l) {
		*pp = portal->next[1];
		portal->nodes[1] = NULL;
	}
}

void PrintPortal(portal_t *p) {
	int			i;
	winding_t	*w;

	w = p->winding;
	for(i = 0; i < w->numpoints; i++) {
		std::cout << 
		"(" 
		<< w->p[i][0] 
		<< ", " << w->p[i][1] 
		<< ", " << w->p[i][2] 
		<< ")" 
		<< std::endl;
	}
}

#define SIDESPACE	8
void MakeHeadnodePortals(tree_t *tree) {
	vec3_t		bounds[2];
	int			i, j, n;
	portal_t	*p, *portals[6];
	plane_t		bplanes[6], *pl;
	node_t *node;

	node = tree->headnode;

	// Pad with some space so there will never be null volume leafs
	for(i = 0; i < 3; i++) {
		bounds[0][i] = tree->mins[i] - SIDESPACE;
		bounds[1][i] = tree->maxs[i] + SIDESPACE;
	}

	tree->outside_node.planenum = PLANENUM_LEAF;
	tree->outside_node.brushlist = NULL;
	tree->outside_node.portals = NULL;
	tree->outside_node.contents = 0;

	for(i = 0; i < 3; i++) {
		for(j = 0; j < 2; j++) {
			n = j * 3 + i;

			p = AllocPortal();
			portals[n] = p;

			pl = &bplanes[n];
			memset(pl, 0, sizeof(*pl));
			if(j) {
				pl->normal[i] = -1;
				pl->dist = -bounds[j][i];
			}
			else {
				pl->normal[i] = 1;
				pl->dist = bounds[j][i];
			}
			p->plane = *pl;
			p->winding = BaseWindingForPlane(pl->normal, pl->dist);

			AddPortalToNodes(p, node, &tree->outside_node);
		}
	}

	// Clip the basewindings by all the other planes
	for(i = 0; i < 6; i++) {
		for(j = 0; j < 6; j++) {
			if(j == i) {
				continue;
			}
			ChopWindingInPlace(&portals[i]->winding, bplanes[j].normal, bplanes[j].dist, ON_EPSILON);
		}
	}
}

#define	BASE_WINDING_EPSILON	0.001
#define	SPLIT_WINDING_EPSILON	0.001

winding_t	*BaseWindingForNode(node_t *node) {
	winding_t	*w;
	node_t		*n;
	plane_t		*plane;
	vec3_t		normal;
	vec_t		dist;

	w = BaseWindingForPlane(mapplanes[node->planenum].normal
		, mapplanes[node->planenum].dist);

	// Clip by all the parents
	for(n = node->parent; n && w; ) {
		plane = &mapplanes[n->planenum];

		if(n->children[0] == node) {	// Take the front
			ChopWindingInPlace(&w, plane->normal, plane->dist, BASE_WINDING_EPSILON);
		}
		else {							// Take the back
			VectorSubtract(vec3_origin, plane->normal, normal);
			dist = -plane->dist;
			ChopWindingInPlace(&w, normal, dist, BASE_WINDING_EPSILON);
		}
		node = n;
		n = n->parent;
	}

	return w;
}

bool WindingIsTiny(winding_t *w);

void MakeNodePortal(node_t *node) {
	portal_t	*new_portal, *p;
	winding_t	*w;
	vec3_t		normal;
	float		dist;
	int			side;

	w = BaseWindingForNode(node);

	// Clip the portal by all the other portals in the node
	for(p = node->portals; p && w; p = p->next[side]) {
		if(p->nodes[0] == node) {
			side = 0;
			VectorCopy(p->plane.normal, normal);
			dist = p->plane.dist;
		}
		else if(p->nodes[1] == node) {
			side = 1;
			VectorSubtract(vec3_origin, p->plane.normal, normal);
			dist = -p->plane.dist;
		}
		else {
			Error("CutNodePortals_r: mislinked portal");
		}

		ChopWindingInPlace(&w, normal, dist, 0.1);
	}

	if(!w) {
		return;
	}

	if(WindingIsTiny(w)) {
		c_tinyportals++;
		FreeWinding(w);
		return;
	}

	new_portal = AllocPortal();
	new_portal->plane = mapplanes[node->planenum];
	new_portal->onnode = node;
	new_portal->winding = w;	
	AddPortalToNodes(new_portal, node->children[0], node->children[1]);
}

void SplitNodePortals(node_t *node) {
	portal_t	*p, *next_portal, *new_portal;
	node_t		*f, *b, *other_node;
	int			side;
	plane_t		*plane;
	winding_t	*frontwinding, *backwinding;

	plane = &mapplanes[node->planenum];
	f = node->children[0];
	b = node->children[1];

	for(p = node->portals; p; p = next_portal) {
		if(p->nodes[0] == node) {
			side = 0;
		}
		else if(p->nodes[1] == node) {
			side = 1;
		}
		else {
			Error("CutNodePortals_r: mislinked portal");
		}
		next_portal = p->next[side];

		other_node = p->nodes[!side];
		RemovePortalFromNode (p, p->nodes[0]);
		RemovePortalFromNode (p, p->nodes[1]);

		// Split the portal along the cut plane

		ClipWindingEpsilon(p->winding, plane->normal, plane->dist,
				SPLIT_WINDING_EPSILON, &frontwinding, &backwinding);

		if(frontwinding && WindingIsTiny(frontwinding)) {
			FreeWinding(frontwinding);
			frontwinding = NULL;
			c_tinyportals++;
		}

		if(backwinding && WindingIsTiny(backwinding)) {
			FreeWinding(backwinding);
			backwinding = NULL;
			c_tinyportals++;
		}

		if(!frontwinding && !backwinding) {	// We have tiny windings on both sides
			continue;
		}

		if (!frontwinding) {
			FreeWinding(backwinding);
			if(side == 0) {
				AddPortalToNodes (p, b, other_node);
			}
			else {
				AddPortalToNodes (p, other_node, b);
			}
			continue;
		}
		if (!backwinding) {
			FreeWinding(frontwinding);
			if(side == 0) {
				AddPortalToNodes (p, f, other_node);
			}
			else {
				AddPortalToNodes (p, other_node, f);
			}
			continue;
		}

		// The winding is split
		new_portal = AllocPortal();
		*new_portal = *p;
		new_portal->winding = backwinding;
		FreeWinding(p->winding);
		p->winding = frontwinding;

		if(side == 0) {
			AddPortalToNodes(p, f, other_node);
			AddPortalToNodes(new_portal, b, other_node);
		}
		else {
			AddPortalToNodes(p, other_node, f);
			AddPortalToNodes(new_portal, other_node, b);
		}
	}
	
	node->portals = NULL;
}

void CalcNodeBounds(node_t *node) {
	portal_t	*p;
	int			s;
	int			i;

	// Calc mins/maxs for both leafs and nodes
	ClearBounds(node->mins, node->maxs);
	for(p = node->portals; p; p = p->next[s]) {
		s = (p->nodes[1] == node);
		for(i = 0; i < p->winding->numpoints; i++) {
			AddPointToBounds(p->winding->p[i], node->mins, node->maxs);
		}
	}
}

void MakeTreePortals_r(node_t *node) {
	int		i;

	CalcNodeBounds(node);
	if(node->mins[0] >= node->maxs[0]) {
		std::cout << "WARNING: node without a volume" << std::endl;
	}

	for(i = 0; i < 3; i++) {
		if(node->mins[i] < -8000 || node->maxs[i] > 8000) {
			std::cout << "WARNING: node without unbounded volume" << std::endl;
			break;
		}
	}
	if(node->planenum == PLANENUM_LEAF) {
		return;
	}

	MakeNodePortal(node);
	SplitNodePortals(node);

	MakeTreePortals_r(node->children[0]);
	MakeTreePortals_r(node->children[1]);
}

void MakeTreePortals(tree_t *tree) {
	MakeHeadnodePortals(tree); 
	MakeTreePortals_r(tree->headnode);
}