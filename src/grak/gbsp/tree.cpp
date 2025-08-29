#include "gbsp.h"

extern	int	c_nodes;

void FreeTreePortals_r(node_t *node) {
	portal_t	*p, *nextp;
	int			s;

	// Free the children
	if(node->planenum != PLANENUM_LEAF) {
		FreeTreePortals_r(node->children[0]);
		FreeTreePortals_r(node->children[1]);
	}

	// Free portals
	for(p = node->portals; p; p = nextp) {
		s = (p->nodes[1] == node);
		nextp = p->next[s];

		RemovePortalFromNode(p, p->nodes[!s]);
		FreePortal(p);
	}
	
	node->portals = NULL;
}

void FreeTree_r(node_t *node) {
	face_t			*f, *nextf;

	// Free the children
	if(node->planenum != PLANENUM_LEAF) {
		FreeTree_r(node->children[0]);
		FreeTree_r(node->children[1]);
	}

	// Free bspbrushes
	FreeBrushList(node->brushlist);

	// Free faces
	for(f = node->faces; f; f = nextf) {
		nextf = f->next;
		FreeFace(f);
	}
	
	// Free the node
	if(node->volume) {
		FreeBrush(node->volume);
	}

	c_nodes--;
	
	free(node);
}

void FreeTree(tree_t *tree) {
	FreeTreePortals_r(tree->headnode);
	FreeTree_r(tree->headnode);
	free(tree);
}

void PrintTree_r(node_t *node, int depth) {
	int			i;
	plane_t		*plane;
	bspbrush_t	*bb;

	for(i = 0; i < depth; i++) {
		std::cout << "	" << std::endl;
	}
	if(node->planenum == PLANENUM_LEAF) {
		if(!node->brushlist) {
			std::cout << "NULL" << std::endl;
		}
		else {
			for(bb = node->brushlist; bb; bb = bb->next) {
				std::cout << bb->original->brushnum << std::endl;
			}
		}

		return;
	}

	plane = &mapplanes[node->planenum];
	std::cout << "#" << node->planenum << "(" 
		<< plane->normal[0] << ", "
		<< plane->normal[1] << ", "
		<< plane->normal[2] << ") "
		<< plane->dist << std::endl;
	PrintTree_r(node->children[0], depth + 1);
	PrintTree_r(node->children[0], depth + 1);
}

int	c_pruned;

void PruneNodes_r(node_t *node) {		// NODES THAT DON'T SEPERATE DIFFERENT 
	bspbrush_t		*b, *next;			// CONTENTS CAN BE PRUNED

	if(node->planenum == PLANENUM_LEAF) {
		return;
	}

	PruneNodes_r(node->children[0]);
	PruneNodes_r(node->children[1]);

	if((node->children[0]->contents & CONTENTS_SOLID)
	&& (node->children[1]->contents & CONTENTS_SOLID)) {
		if(node->faces) {
			Error("node-faces seperating CONTENTS_SOLID");
		}
		if(node->children[0] -> faces || node->children[1]->faces) {
			Error("!node->faces with children");
		}

		node->planenum = PLANENUM_LEAF;
		node->contents = CONTENTS_SOLID;
		node->detail_seperator = false;

		if(node->brushlist) {
			Error("PruneNodes: node->brushlist");
		}

		// Combine brush lists
		node->brushlist = node->children[1]->brushlist;

		for(b = node->children[0]->brushlist; b; b = next) {
			next = b->next;
			b->next = node->brushlist;
			node->brushlist = b;
		}

		c_pruned++;
	}
}

void PruneNodes(node_t *node) {
	std::cout << "--- PruneNodes ---" << std::endl;
	c_pruned = 0;
	PruneNodes_r(node);
	std::cout << c_pruned << " pruned nodes" << std::endl;
}