#include "RadiosityBaker.hpp"

#include "LightingBasis.hpp"

#include <iostream>

RadiosityBaker::RadiosityBaker() {
	numLumel = 0;
}

void RadiosityBaker::PatchesForFace(FileFace *face) {
	face->lightMapOffset = numLumel;

	FilePlane *facePlane = &bspFile->filePlanes[face->planeNum];

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
	int currVertIndex = face->firstVert;
	FileVert currVert = bspFile->fileVerts[bspFile->fileFaceVerts[currVertIndex]];
	Vec3f min(currVert.point[0], currVert.point[1], currVert.point[2]);
	Vec3f max = Vec3f(min);
	for(int i = 1; i < face->numVerts; i++) {
		currVertIndex++;
		FileVert currVert = bspFile->fileVerts[bspFile->fileFaceVerts[currVertIndex]];

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
	else if(major == 1) {
		cz1 = -(facePlane->normal[2] * min.z + facePlane->normal[0] * min.x + facePlane->dist);
		min.y = cz1 / facePlane->normal[1];

		cz2 = -(facePlane->normal[2] * max.z + facePlane->normal[0] * max.x + facePlane->dist);
		max.y = cz2 / facePlane->normal[1];
	}
	else if(major == 2) {
		cz1 = -(facePlane->normal[0] * min.x + facePlane->normal[1] * min.y + facePlane->dist);
		min.z = cz1 / facePlane->normal[2];

		cz2 = -(facePlane->normal[0] * max.z + facePlane->normal[1] * max.x + facePlane->dist);
		max.z = cz2 / facePlane->normal[2];
	}

	// Use these to find texture plane origin, lightmap dimensions, and uv axes
	face->lightMapOrigin[0] = min.x;
	face->lightMapOrigin[1] = min.y;
	face->lightMapOrigin[2] = min.z;

	float farVert[3];
	farVert[0] = max.x;
	farVert[1] = max.y;
	farVert[2] = max.z;

	face->lightMapS[0] = face->lightMapT[0] = max.x - min.x;
	face->lightMapS[1] = face->lightMapT[1] = max.y - min.y;
	face->lightMapS[2] = face->lightMapT[2] = max.z - min.z;

	face->lightMapS[(major + 1) % 3] = 0;
	face->lightMapS[major] = -(	face->lightMapOrigin[(major + 1) % 3] * facePlane->normal[(major + 1) % 3]		// Solution for the unknown component of S
								+ farVert[(major + 2) % 3] * facePlane->normal[(major + 2) % 3]	) / facePlane->normal[major];
	face->lightMapT[(major + 2) % 3] = 0;
	face->lightMapT[major] = -(	face->lightMapOrigin[(major + 1) % 3] * facePlane->normal[(major + 1) % 3]		// Same thing, but for T
								+ farVert[(major + 2) % 3] * facePlane->normal[(major + 2) % 3]	) / facePlane->normal[major];
	
	float magS = sqrt(face->lightMapS[0] * face->lightMapS[0] + face->lightMapS[1] * face->lightMapS[1] + face->lightMapS[2] * face->lightMapS[2]);
	face->lightMapS[0] /= magS;
	face->lightMapS[1] /= magS;
	face->lightMapS[2] /= magS;
	float magT = sqrt(face->lightMapT[0] * face->lightMapT[0] + face->lightMapT[1] * face->lightMapT[1] + face->lightMapT[2] * face->lightMapT[2]);
	face->lightMapT[0] /= magT;
	face->lightMapT[1] /= magT;
	face->lightMapT[2] /= magT;

	face->lightMapWidth = ceil(face->lightMapS[(major + 2) % 3] / PATCH_SIZE);
	face->lightMapHeight = ceil(face->lightMapT[(major + 1) % 3] / PATCH_SIZE);

	// Create the actual patches
	Vec3f sVec = Vec3f(face->lightMapS[0], face->lightMapS[1], face->lightMapS[2]);
	Vec3f tVec = Vec3f(face->lightMapT[0], face->lightMapT[1], face->lightMapT[2]);
	for(int tStep; tStep < face->lightMapHeight; tStep++) {
		for(int sStep; sStep < face->lightMapWidth; sStep++) {
			Vec3f s = sVec * ((sStep + 0.5) * PATCH_SIZE);
			Vec3f t = tVec * ((tStep + 0.5) * PATCH_SIZE);

			numLumel++;
		}
	}
}

void RadiosityBaker::InitLightMaps() {
	int numFaces = bspFile->fileHeader.lumps[LUMP_FACES].length;
	for(int i = 0; i < numFaces; i++) {
		PatchesForFace(&bspFile->fileFaces[i]);
	}
}

void RadiosityBaker::InitialLightingPass() {

}

void RadiosityBaker::BakeRad(BspFile *bspFile) {
	this->bspFile = bspFile;
	numLumel = 0;

	InitLightMaps();
	InitialLightingPass();
}