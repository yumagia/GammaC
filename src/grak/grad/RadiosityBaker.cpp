#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

RadiosityBaker::RadiosityBaker() {
	numLumel = 0;
}

void RadiosityBaker::PatchesForFace(FileFace &face) {
	face.lightMapOffset = numLumel;

	FilePlane *facePlane = &bspFile->filePlanes[face.planeNum];
	

	// Determine the major axis
	int maxAxis = fabs(facePlane->normal[0]);
	int major = 0;
	if(maxAxis < fabs(facePlane->normal[1])) {
		maxAxis = fabs(facePlane->normal[1]);
		major = 1;
	}
	if(maxAxis < fabs(facePlane->normal[2])) {
		maxAxis = fabs(facePlane->normal[2]);
		major = 2;
	}

	// Find min and max bounds of projected polygon
	int currVertIndex = face.firstVert;
	FileVert currVert = bspFile->fileVerts[currVertIndex];
	Vec3f min(currVert.point[0], currVert.point[2], currVert.point[1]);
	Vec3f max = min;
	for(int i = 1; i < face.numVerts; i++) {
		FileVert currVert = bspFile->fileVerts[currVertIndex];

		if(min.x > currVert.point[0]) {
			min.x = currVert.point[0];
		}
		if(min.y > currVert.point[1]) {
			min.y = currVert.point[1];
		}
		if(min.z > currVert.point[2]) {
			min.z = currVert.point[2];
		}
		if(max.x < currVert.point[0]) {
			max.x = currVert.point[0];
		}
		if(max.y < currVert.point[1]) {
			max.y = currVert.point[1];
		}
		if(max.z < currVert.point[2]) {
			max.z = currVert.point[2];
		}
	}

	// Project min and max downwards
	float cz1, cz2;
	if(major == 0) {
		cz1 = -(facePlane->normal[1] * min.y + facePlane->normal[2] * min.z + facePlane->dist);
		min.x = cz1 / facePlane->normal[0];

		cz2 = -(facePlane->normal[1] * max.y + facePlane->normal[2] * max.z + facePlane->dist);
		max.x = cz2 / facePlane->normal[0];
	}
	if(major == 1) {
		cz1 = -(facePlane->normal[2] * min.z + facePlane->normal[0] * min.x + facePlane->dist);
		min.y = cz1 / facePlane->normal[1];

		cz2 = -(facePlane->normal[2] * max.z + facePlane->normal[0] * max.x + facePlane->dist);
		max.y = cz2 / facePlane->normal[1];
	}
	if(major == 2) {
		cz1 = -(facePlane->normal[0] * min.x + facePlane->normal[1] * min.y + facePlane->dist);
		min.z = cz1 / facePlane->normal[2];

		cz2 = -(facePlane->normal[2] * max.z + facePlane->normal[0] * max.x + facePlane->dist);
		max.y = cz2 / facePlane->normal[1];
	}

	// Use these to find texture plane origin, and uv axes
	face.lightMapOrigin[0] = min.x;
	face.lightMapOrigin[1] = min.y;
	face.lightMapOrigin[2] = min.z;

	face.lightMapS[0];


	Vec3f normal(facePlane->normal[0], facePlane->normal[1], facePlane->normal[2]);
	
}

void RadiosityBaker::InitLightMaps() {
	for(FileFace face: bspFile->fileFaces) {
		PatchesForFace(face);
	}
}

void RadiosityBaker::InitialLightingPass() {

}

void RadiosityBaker::BakeRad(BspFile *bspFile) {
	this->bspFile = bspFile;

	InitLightMaps();
	InitialLightingPass();
}