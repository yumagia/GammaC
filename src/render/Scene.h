#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#define MAX_FACES 10000
#define MAX_VERTS 100000

#include "../mapping/Bsp.h"

#include <vector>
#include <map>

class Scene {
public:
	Scene() {}
	~Scene() {}

	void AddBspModel(BspModel *model);


private:
	BspModel *sceneRoot = NULL;
};

#endif