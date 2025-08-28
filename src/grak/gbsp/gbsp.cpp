#include "gbsp.h"

extern	float subdivide_size;

std::string		source;
std::string		name;

vec_t		microvolume = 1.0;
bool		noprune;
bool		vkview;
bool		nodetail;
bool		fulldetail;
bool		onlyents;
bool		nomerge;
bool		nowater;
bool		nofill;
bool		nocsg;
bool		noweld;
bool		noshare;
bool		nosubdiv;
bool		notjunc;
bool		noopt;
bool		leaktest;
bool		verboseentities;

char		outbase[32];

int			block_xl = -8, block_xh = 7, block_yl = -8, block_yh = 7;

int			entity_num;


node_t		*block_nodes[10][10];

/**
 * @brief Dice the tree, this is done before building it
 */
node_t	*BlockTree(int xl, int yl, int xh, int yh) {
	node_t	*node;
	vec3_t	normal;
	float	dist;
	int		mid;

	if (xl == xh && yl == yh) {
		node = block_nodes[xl+5][yl+5];
		if (!node) {	// return an empty leaf
			node = AllocNode();
			node->planenum = PLANENUM_LEAF;
			node->contents = 0; //CONTENTS_SOLID;
			return node;
		}
		return node;
	}

	// create a seperator along the largest axis
	node = AllocNode();

	if (xh - xl > yh - yl) {	// split x axis
		mid = xl + (xh - xl) / 2 + 1;
		normal[0] = 1;
		normal[1] = 0;
		normal[2] = 0;
		dist = mid * 1024;
		node->planenum = FindFloatPlane(normal, dist);
		node->children[0] = BlockTree(mid, yl, xh, yh);
		node->children[1] = BlockTree(xl, yl, mid - 1, yh);
	}
	else {
		mid = yl + (yh - yl) / 2 + 1;
		normal[0] = 0;
		normal[1] = 1;
		normal[2] = 0;
		dist = mid * 1024;
		node->planenum = FindFloatPlane(normal, dist);
		node->children[0] = BlockTree(xl, mid, xh, yh);
		node->children[1] = BlockTree(xl, yl, xh, mid - 1);
	}

	return node;
}

int			brush_start, brush_end;
void ProcessBlock_Thread(int blocknum) {
	int		xblock, yblock;
	vec3_t		mins, maxs;
	bspbrush_t	*brushes;
	tree_t		*tree;
	node_t		*node;

	yblock = block_yl + blocknum / (block_xh - block_xl + 1);
	xblock = block_xl + blocknum % (block_xh - block_xl + 1);

	std::cout << "############### block " << xblock << " " << yblock << " ###############" << std::endl;

	mins[0] = xblock * 1024;
	mins[1] = yblock * 1024;
	mins[2] = -4096;
	maxs[0] = (xblock + 1) * 1024;
	maxs[1] = (yblock + 1) * 1024;
	maxs[2] = 4096;

	// The makelist and chopbrushes could be cached between the passes...
	brushes = MakeBspBrushList(brush_start, brush_end, mins, maxs);
	if(!brushes) {
		node = AllocNode();
		node->planenum = PLANENUM_LEAF;
		node->contents = CONTENTS_SOLID;
		block_nodes[xblock + 5][yblock + 5] = node;
		return;
	}

	brushes = ChopBrushes(brushes);

	tree = BrushBSP(brushes, mins, maxs);

	block_nodes[xblock + 5][yblock + 5] = tree->headnode;
}


/**
 * @brief Process the world tree
 */
void ProcessWorldModel(void) {
	entity_t	*e;
	tree_t		*tree;
	bool		leaked;
	bool		optimize;
	int			i;
	int			pass;

	e = &entities[entity_num];

	brush_start = e->firstbrush;
	brush_end = brush_start + e->numbrushes;
	leaked = false;

	//
	// perform per-block operations
	//
	if(block_xh * 1024 > map_maxs[0]) {
		block_xh = floor(map_maxs[0]/1024.0);
	}
	if((block_xl + 1) * 1024 < map_mins[0]) {
		block_xl = floor(map_mins[0]/1024.0);
	}
	if(block_yh * 1024 > map_maxs[1]) {
		block_yh = floor(map_maxs[1]/1024.0);
	}
	if((block_yl + 1) * 1024 < map_mins[1]) {
		block_yl = floor(map_mins[1]/1024.0);
	}

	if(block_xl <-4) {
		block_xl = -4;
	}
	if(block_yl <-4) {
		block_yl = -4;
	}
	if(block_xh > 3) {
		block_xh = 3;
	}
	if(block_yh > 3) {
		block_yh = 3;
	}

	optimize = false;
	for(pass = 0; pass < 2; pass++) {
		std::cout << "--------------------------------------------" << std::endl;

		for(i = 0; i <= (block_xh - block_xl + 1)*(block_yh - block_yl + 1); i++) {
			ProcessBlock_Thread(i);
		}
		//RunThreadsOnIndividual(block_xh - block_xl + 1)*(block_yh - block_yl + 1),
		//	!verbose, ProcessBlock_Thread);

		std::cout << "--------------------------------------------" << std::endl;

		tree = AllocTree();
		tree->headnode = BlockTree(block_xl - 1, block_yl - 1, block_xh + 1, block_yh + 1);

		tree->mins[0] = (block_xl) * 1024;
		tree->mins[1] = (block_yl) * 1024;
		tree->mins[2] = map_mins[2] - 8;

		tree->maxs[0] = (block_xh + 1) * 1024;
		tree->maxs[1] = (block_yh + 1) * 1024;
		tree->maxs[2] = map_maxs[2] + 8;

		MakeTreePortals(tree);

		//MarkVisibleSides(tree, brush_start, brush_end);
		optimize = !optimize;
	}
}

void ProcessSubModel(void) {
	entity_t	*e;
	int			start, end;
	tree_t		*tree;
	bspbrush_t	*list;
	vec3_t		mins, maxs;

	e = &entities[entity_num];

	start = e->firstbrush;
	end = start + e->numbrushes;

	mins[0] = mins[1] = mins[2] = -4096;
	maxs[0] = maxs[1] = maxs[2] = 4096;
	list = MakeBspBrushList(start, end, mins, maxs);
	list = ChopBrushes(list);
	tree = BrushBSP(list, mins, maxs);

	WriteBSP(tree->headnode);

}

void ProcessModels(void) {
	BeginBSPFile();

	for(entity_num = 0; entity_num < num_entities; entity_num++) {
		if(!entities[entity_num].numbrushes) {
			continue;
		}

		std::cout << "############### model " << nummodels << " ###############" << std::endl;
		BeginModel();
		if(entity_num == 0) {
			ProcessWorldModel();
		}
		else {
			ProcessSubModel();
		}
		EndModel();
	}

	EndBSPFile();
}

int main(int argc, char **argv) {
	int		i;
	char		path[1024];
	double		start, end;
	std::string s1 = "";

    std::cout << "RUNNING GBSP...\n";
	for(i = 1; i < argc; i++) {

		s1 = argv[i];

		if(s1 == "-vkview") {
			vkview = true;
		}
		else if (argv[i][0] == '-') {
			Error("Unknown argument: %s\n", argv[i]);
		}
		else {
			break;
		}
	}

	if(i != argc - 1) {
		Error("Usage: gbsp [arguments] mapfile\n");
	}

	std::cout << argv[i] << std::endl;
	
	SetGdirFromPath(argv[i]);

	source = ExpandArg(argv[i]);
	StripExtension(source);

	name = ExpandArg(argv[i]);
	DefaultExtension(name, ".map");

	LoadMapFile(name);
	SetModelNumbers();
	SetLightStyles();

	ProcessModels();
}