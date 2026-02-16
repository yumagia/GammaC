#include "Patch.hpp"
#include "lighttrace/Trace.hpp"

#include <iostream>

void Patch::NudgePosition(BspFile *bspFile) {
	Trace trace(bspFile);
	FileFace *face = &bspFile->fileFaces[faceIndex];

	if(!trace.PositionSolid(position)) {		// Patch is not stuck in solid
		return;
	}

	// Test nudges in this order
	float testUs[24] = {	-0.5, 0.5, 0, 0, 
							-0.5, 0.5, -0.5, 0.5,
							-1, 1, 0, 0, 
							-1, 1, -1, 1,
							-1.5, 1.5, 0, 0,
							-1.5, 1.5, -1.5, 1.5	};
	float testVs[24] = {	0, 0, -0.5, 0.5, 
							-0.5, 0.5, 0.5, -0.5,
							0, 0, -1, 1, 
							-1, 1, 1, -1,
							0, 0, -1.5, 1.5,
							-1.5, 1.5, 1.5, -1.5	};

	Vec3f sVec = PATCH_SIZE * Vec3f(face->lightMapS[0], face->lightMapS[1], face->lightMapS[2]);
	Vec3f tVec = PATCH_SIZE * Vec3f(face->lightMapT[0], face->lightMapT[1], face->lightMapT[2]);
	for(int i = 0; i < 24; i++) {
		Vec3f testPosition = position + testUs[i] * sVec + testVs[i] * tVec;

		if(!trace.PositionSolid(testPosition)) {		// Patch is no longer stuck in solid, set new position
			position = testPosition;
			return;
		}
	}

	// TODO: Find out what's going on here
	std::cout << "A Lumel was nudged but remains stuck" << std::endl;
}

void Patch::CalcTransfersForpatch(int numPatches, Patch *patchList, BspFile *bspFile) {
	if(!legal) {
		return;
	}

	numTransfers = 0;
	transfers = new Transfer[MAX_MAP_PATCHES];
	float total = 0;

	for(int i = 0; i < numPatches; i++) {
		Patch *otherPatch = &patchList[i];

		if(this == otherPatch) {
			continue;
		}

		FileFace *face = &bspFile->fileFaces[this->faceIndex];
		FileFace *face2 = &bspFile->fileFaces[otherPatch->faceIndex];

		FilePlane *plane = &bspFile->filePlanes[face->planeNum];
		FilePlane *plane2 = &bspFile->filePlanes[face2->planeNum];

		Vec3f patchNormal = Vec3f(plane->normal[0], plane->normal[1], plane->normal[2]);
		Vec3f patchNormal2 = Vec3f(plane2->normal[0], plane2->normal[1], plane2->normal[2]);
		
		Vec3f ray = otherPatch->position - this->position;
		float dist = ray.Normalize();

		// Basically lambert
		float intensity = ray.Dot(patchNormal);
		intensity *= -ray.Dot(patchNormal2);
		if(intensity <= 0) {
			continue;
		}

		// Occlusion check
		Trace trace(bspFile);
		if(trace.LineStab(this->position, otherPatch->position)) {
			continue;
		}

		float transferValue = intensity * (PATCH_SIZE * PATCH_SIZE) / (dist * dist);

		if(transferValue < 0) {
			transferValue = 0;
		}

		if(transferValue > 0) {
			Transfer *transfer = &transfers[numTransfers];
			transfer->transfer = transferValue;
			transfer->patch = i;
			total += transferValue;
			numTransfers++;
		}
	}

	for(int i = 0; i < numTransfers; i++) {
		Transfer *transfer = &transfers[i];

		transfer->transfer = (1 - escapeRate) * (transfer->transfer / total);
	}
}

void Patch::ShootLight(Color *collected) {
	if(!legal) {
		return;
	}

	for(int i = 0; i < numTransfers; i++) {
		Transfer *transfer = &transfers[i];
		collected[transfer->patch] = collected[transfer->patch] + transfer->transfer * sampledLight;
	}
}

void Patch::CollectLight(Color light) {
	if(!legal) {
		return;
	}
	sampledLight = light * diffuse;

	accumulatedLight = accumulatedLight + sampledLight;
}

void Patch::FreeTransfers() {
	delete[] transfers;
}