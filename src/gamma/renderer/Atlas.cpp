#include "Atlas.hpp"

#define ATLAS_DIMENSION 1024

namespace GammaEngine {
    Atlas::Atlas() {

    }

    Atlas::~Atlas() {
        if(atlas_ != nullptr) {
            delete[] atlas_;
        }

        atlas_ = nullptr;
    }

    void Atlas::Initialize() {
        atlas_ = new unsigned char[ATLAS_DIMENSION * ATLAS_DIMENSION];
    }
}