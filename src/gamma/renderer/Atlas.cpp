#include "Atlas.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>

#define ATLAS_DIMENSIONS 1024
#define STATIC_LIGHTMAP_HEIGHT 512

namespace GammaEngine {
	Atlas::Atlas() {

	}

	Atlas::~Atlas() {

	}

	void Atlas::Initialize() {
		dimensions_ = ATLAS_DIMENSIONS;

		atlas_.reserve(ATLAS_DIMENSIONS * ATLAS_DIMENSIONS * 4);
		
		allocatedHeight_.reserve(ATLAS_DIMENSIONS);
		for(int i = 0; i < ATLAS_DIMENSIONS; i++) {
			allocatedHeight_.push_back(0);
		}
	}

	void Atlas::WriteImageFile() {
		std::cout << "Writing png for lightmap atlas..." << std::endl;

		if(stbi_write_png("atlas.png", ATLAS_DIMENSIONS, ATLAS_DIMENSIONS, 4, atlas_.data(), ATLAS_DIMENSIONS * 4)) {
			std::cout << "\tSuccessfully wrote png file" << std::endl;
			return;
		}

		std::cout << "\tWarning: Could not successfully write png file" << std::endl;
		return;
	}

	void Atlas::RemapUv(float& u, float& v) {
		u = ((u + allocatedX_) / ATLAS_DIMENSIONS);
		v = ((v + allocatedY_) / ATLAS_DIMENSIONS);
	}

	// Plays a game of tetris to fit the texture onto the atlas.
	// Returns true upon a succesful allocation, false otherwise
	bool Atlas::UploadTexture(unsigned int width, unsigned int height, unsigned char *texture) {
		allocatedX_ = allocatedY_ = 0;
		unsigned int bestHeight = STATIC_LIGHTMAP_HEIGHT;

		for(int i = 0; i < (ATLAS_DIMENSIONS - width); i++) {
			int j = 0;
			unsigned int tentativeHeight = 0;
			while(allocatedHeight_[i + j] < bestHeight && j < width) {
				if(allocatedHeight_[i + j] >= tentativeHeight) {
					tentativeHeight = allocatedHeight_[i + j];
				}

				j++;
			}

			if(j == width) {
				allocatedX_ = i;
				allocatedY_ = bestHeight = tentativeHeight;
			}
		}

		if((bestHeight + height) > STATIC_LIGHTMAP_HEIGHT) {
			std::cerr << "WARNING: Could not successfully upload texture to lightmap atlas..." << std::endl;
			return false;
		}
		
		for(int i = 0; i < width; i++) {
			allocatedHeight_[allocatedX_ + i] = allocatedY_ + height;
		}

		for(int j = 0; j < height; j++) {
			for(int i = 0; i < width; i++) {
				unsigned int atlasLoc = ((allocatedX_ + i) + (allocatedY_ + j) * ATLAS_DIMENSIONS) * 4;
				unsigned int texLoc = (i + (j * width)) * 4;
				atlas_[atlasLoc] = texture[texLoc];
				atlas_[atlasLoc + 1] = texture[texLoc + 1];
				atlas_[atlasLoc + 2] = texture[texLoc + 2];
				atlas_[atlasLoc + 3] = texture[texLoc + 3];
			}
		}

		return true;
	}

	unsigned int Atlas::GetDimensions() {
		return dimensions_;
	}

	unsigned char *Atlas::GetData() {
		return atlas_.data();
	}
}