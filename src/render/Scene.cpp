
#include "Scene.h"

#include <iostream>

void Scene::AddBspModel(BspModel *model) {
    if(!sceneRoot) {
        if(!model->solid) {
            std::cout << "Warning: Cannot create a scene using a subtractive model" << std::endl;
            return;
        }
        sceneRoot = model;
    }
    else {
        // TODO: Combine bsp trees
    }
}
