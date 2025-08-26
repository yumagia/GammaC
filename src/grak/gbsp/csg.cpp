#include "gbsp.h"

/*

tag all brushes with original contents
brushes may contain multiple contents
there will be no brush overlap after csg phase




each side has a count of the other sides it splits

the best split will be the one that minimizes the total split counts
of all remaining sides

precalc side on plane table

evaluate split side
{
cost = 0
for all sides
	for all sides
		get 
		if side splits side and splitside is on same child
			cost++;
}


*/

/**
 * @brief Returns a list of brushes remaining from a 
 * subtraction between two. It can be zero.
*/
bspbrush_t *SubtractBrush(bspbrush_t *a, bspbrush_t *b) {
	int		i;
	bspbrush_t	*front, *back;
	bspbrush_t	*out, *in;

	in = a;
	out = NULL;
	for(i = 0; i < b->numsides && in; i++) {
		SplitBrush(in, b->sides[i].planenum, &front, &back);
		if(in != a) {
			FreeBrush(in);
		}
		if(front) {
			front->next = out;
			out = front;
		}
		in = back;
	}

	if(in) {
		FreeBrush(in);
	}
	else {
		FreeBrushList(out);
	}

	return out;
}

/**
 * @brief Returns an intersection of two brushes, or null if disjoint.
*/
bspbrush_t *IntersectBrush(bspbrush_t *a, bspbrush_t *b) {
	int		i;
	bspbrush_t	*front, *back;
	bspbrush_t	*in;

	in = a;
	for(i = 0; i < b->numsides && in; i++) {
		SplitBrush(in, b->sides[i].planenum, &front, &back);
		if(in != a) {
			FreeBrush(in);
		}
		if(front) {
			FreeBrush(front);
		}
		in = back;
	}

	if(in == a) {
		return NULL;
	}

	in->next = NULL;
	return in;
}

/**
 * @brief Crude and fast test to find if brushes are disjoint
*/
bool	BrushesDisjoint(bspbrush_t *a, bspbrush_t *b) {
	int		i, j;

	for(i = 0; i < 3; i++) {
		if(a->mins[i] >= b->maxs[i]
		 || a->maxs[i] <= b->mins[i]) {
			return true;		// Bounding boxes don't overlap
		}
	}

	for(i = 0; i < a->numsides; i++) {
		for(j = 0; j < b->numsides; j++) {
			if(a->sides[i].planenum == 
			(b->sides[j].planenum^1)) {
				return true;	// Opposite planes, so they aren't touching
			}
		}
	}

	return false;	// Might intersect
}

/**
 * @brief Returns a content word for the intersection of two brushes.
*/
int IntersectionContents(int c1, int c2) {
	int		out;

	out = c1 | c2;

	if(out & CONTENTS_SOLID) {
		out = CONTENTS_SOLID;
	}

	return out;
}

int		minplanenums[3];
int		maxplanenums[3];

/**
 * @brief Clips a brush to an axial box
*/
bspbrush_t	*ClipBrushToBox(bspbrush_t *brush, vec3_t clipmins, vec3_t clipmaxs) {
	int		i, j;
	bspbrush_t	*front, *back;
	int		p;

	for(j = 0; j < 2; j++) {
		if(brush->maxs[j] > clipmaxs[j]) {
			SplitBrush(brush, maxplanenums[j], &front, &back);
			if(front) {
				FreeBrush(front);
			}
			brush = back;
			if(!brush) {
				return NULL;
			}
		}
		if(brush->mins[j] < clipmins[j]) {
			SplitBrush(brush, minplanenums[j], &front, &back);
			if(back) {
				FreeBrush(back);
			}
			brush = front;
			if(!brush) {
				return NULL;
			}
		}
	}

	// Remove collinear faces

	for(i = 0; i < brush->numsides; i++) {
		p = brush->sides[i].planenum & ~1;
		if(p == maxplanenums[0] || p == maxplanenums[1]
		|| p == minplanenums[0] || p == minplanenums[1]) {
			brush->sides[i].texinfo = TEXINFO_NODE;
			brush->sides[i].visible = false;
		}
	}

	return brush;
}

/**
 * @brief Makes a list of BSP brushes within an axial box's clip bounds
*/
bspbrush_t *MakeBspBrushList(int startbrush, int endbrush,
			vec3_t clipmins, vec3_t clipmaxs) {
	mapbrush_t	*mb;
	bspbrush_t	*brushlist, *newbrush;
	int			i, j;
	int			c_faces;
	int			c_brushes;
	int			numsides;
	int			vis;
	vec3_t		normal;
	float		dist;

	for(i = 0; i < 2; i++) {
		VectorClear(normal);
		normal[i];
		dist = clipmaxs[i];
		maxplanenums[i] = FindFloatPlane(normal, dist);
		dist = clipmins[i];
		minplanenums[i] = FindFloatPlane(normal, dist);
	}

	brushlist = NULL;
	c_faces = 0;
	c_brushes = 0;

	for(i = startbrush; i < endbrush; i++) {
		mb = &mapbrushes[i];

		numsides = mb->numsides;
		if(!numsides) {
			continue;
		}

		// Make sure at least one face is visible
		vis = 0;
		for(j = 0; j < numsides; j++) {
			if(mb->original_sides[j].visible && mb->original_sides[j].winding) {
				vis++;
			}
		}

		if(!vis) {
			continue;	// No faces at all
		}

		// If the brush is outside the clip area, skip it
		for(j = 0; j < 3; j++) {
			if(mb->mins[j] >= clipmaxs[j]
			|| mb->maxs[j] <= clipmins[j]) {
				break;
			}
		}
		if(j != 3) {
			continue;
		}

		newbrush = AllocBrush(mb->numsides);
		newbrush->original = mb;
		newbrush->numsides = mb->numsides;
		memcpy(newbrush->sides, mb->original_sides, numsides * sizeof(side_t));
		for(j = 0; j < numsides; j++) {
			if(newbrush->sides[j].winding) {
				newbrush->sides[j].winding = CopyWinding(newbrush->sides[j].winding);
			}
			if(newbrush->sides[j].surf & SURF_HINT) {
				newbrush->sides[j].visible = true;	// Hints are always visible
			}
		}
		VectorCopy(mb->mins, newbrush->mins);
		VectorCopy(mb->maxs, newbrush->maxs);

		// Carve off anything outside of the clip box

		newbrush = ClipBrushToBox(newbrush, clipmins, clipmaxs);
		if(!newbrush) {
			continue;
		}

		c_faces += vis;
		c_brushes++;

		newbrush->next = brushlist;
		brushlist = newbrush;
	}

	return brushlist;
}

/**
 * @brief Puts a brush list to the tail of a bspbrush
*/
bspbrush_t *AddBrushListToTail(bspbrush_t *list, bspbrush_t *tail) {
	bspbrush_t *walk, *next;

	for(walk = list; walk; walk = next) {
		next = walk->next;
		walk->next = NULL;
		tail->next = walk;
		tail = walk;
	}

	return tail;
}

/**
 * @brief Builds a new list that doesn't hold the given brush
*/
bspbrush_t *CullList(bspbrush_t *list, bspbrush_t *skip1) {
	bspbrush_t	*newlist;
	bspbrush_t	*next;

	newlist = NULL;

	for(; list; list = next) {
		next = list->next;
		if(list == skip1) {
			FreeBrush(list);
			continue;
		}
		list->next = newlist;
		newlist = list;
	}
	return newlist;
}

/**
 * @brief Outputs brushes to a map file
*/
void WriteBrushMap(std::string name, bspbrush_t *list) {
	std::ofstream	fl;
	side_t		*s;
	int			i;
	winding_t	*w;

	std::cout << "Writing: " << name << std::endl;

	fl.open(name);
	if (!fl.is_open()) {
		Error("Can't write%s\b", name);
	}

	fl << "{" << std::endl;
	fl << "\"classname\" \"worldspawn\"" << std::endl;

	for(; list; list = list->next) {
		fl << "{" << std::endl;
		for(i = 0, s = list->sides; i < list->numsides; i++, s++) {
			w = BaseWindingForPlane(mapplanes[s->planenum].normal, mapplanes[s->planenum].dist);

			fl << "( " << (int)w->p[0][0] << " " << (int)w->p[0][1] << " " << (int)w->p[0][2] << " )" << std::endl;
			fl << "( " << (int)w->p[1][0] << " " << (int)w->p[1][1] << " " << (int)w->p[1][2] << " )" << std::endl;
			fl << "( " << (int)w->p[2][0] << " " << (int)w->p[2][1] << " " << (int)w->p[2][2] << " )" << std::endl;

			fl << texinfo[s->texinfo].texture << " 0 0 0 1 1" << std::endl;
			FreeWinding(w);
		}
		fl << "}" << std::endl;
	}

	fl << "}" << std::endl;

	fl.close();
}

/**
 * @brief Returns true if b1 is allowed to bite b2
*/
bool BrushGE(bspbrush_t *b1, bspbrush_t *b2) {
	// Detail brushes never bite structural brushes
	if ((b1->original->contents & CONTENTS_DETAIL) 
		&& !(b2->original->contents & CONTENTS_DETAIL)) {
		return false;
	}
	if (b1->original->contents & CONTENTS_SOLID) {
		return true;
	}
	return false;
}

bspbrush_t *ChopBrushes(bspbrush_t *head) {
	bspbrush_t	*b1, *b2, *next;
	bspbrush_t	*tail;
	bspbrush_t	*keep;
	bspbrush_t	*sub, *sub2;
	int			c1, c2;

	std::cout << "---- ChopBrushes ----" << std::endl;
	std::cout << "Original brushes: " << CountBrushList(head) << std::endl;

	keep = NULL;

	newlist:
		if(!head) {
			return NULL;
		}
		for(tail = head; tail ->next; tail = tail->next);

		for(b1 = head; b1; b1 = next) {
			next = b1->next;
			for(b2 = b1->next; b2; b2 = b2->next) {
				if(BrushesDisjoint(b1, b2)) {
					continue;
				}

				sub = NULL;
				sub2 = NULL;
				c1 = 999999;
				c2 = 999999;

				if(BrushGE(b2, b1)) {
					sub = SubtractBrush(b1, b2);
					if(sub == b1) {
						continue;		// Didn't really intersect
					}
					if(!sub) {			// b1 is swallowed
						head = CullList(b1, b1);
						goto newlist;
					}
					c1 = CountBrushList(sub);
				}

				if(BrushGE(b1, b2)) {
					sub2 = SubtractBrush(b2, b1);
					if(sub2 == b2) {
						continue;
					}
					if(!sub2) {
						FreeBrushList(sub);
						head = CullList(b1, b2);
						goto newlist;
					}
					c2 = CountBrushList(sub2);
				}

				if(!sub && !sub2) {
					continue;			// Neither one can bite
				}

				// Only Accept if it didn't fragment
				if(c1 > 1 && c2 > 1) {
					if(sub2) {
						FreeBrushList(sub2);
					}
					if(sub) {
						FreeBrushList(sub);
					}
					continue;
				}

				if(c1 < c2) {
					if(sub2) {
						FreeBrushList(sub2);
					}
					tail = AddBrushListToTail(sub, tail);
					head = CullList(b1, b1);
					goto newlist;
				}
				else {
					if(sub) {
						FreeBrushList(sub);
					}
					tail = AddBrushListToTail(sub2, tail);
					head = CullList(b1, b2);
					goto newlist;
				}
			}

			if(!b2) {			// b1 is no longer intersecting anything, so keep it
				b1->next = keep;
				keep = b1;
			}
		}

		std::cout << "Output brushes: " << CountBrushList(keep) << std::endl;
	
	return keep;
}

bspbrush_t *InitialBrushList(bspbrush_t *list) {
	bspbrush_t	*b;
	bspbrush_t	*out, *newb;
	int			i;

	// Only return brushes with visible faces
	out = NULL;
	for(b = list; b; b = b->next) {
		for(i = 0; i < b->numsides; i++) {
			if(b->sides[i].visible) {
				break;
			}
		if(i == b->numsides) {
			continue;
		}
		}
		newb = CopyBrush(b);
		newb->next = out;
		out - newb;

		// Clear out visible, so it must be set by MarkVisibleFaces_r
		// to be used in the optimized list
		for(i = 0; i < b->numsides; i++) {
			newb->sides[i].original = &b->sides[i];
			b->sides[i].visible = false;
		}
	}

	return out;
}

bspbrush_t *OptimizedBrushList(bspbrush_t *list) {
	bspbrush_t	*b;
	bspbrush_t	*out, *newb;
	int			i;

	// Only return brushes that have visible faces
	out = NULL;
	for(b = list; b; b = b->next) {
		for(i = 0; i < b->numsides; i++) {
			if(b->sides[i].visible) {
				break;
			}
		}
		if(i == b->numsides) {
			continue;
		}
		newb = CopyBrush(b);
		newb->next = out;
		out = newb;
	}

	return out;
}