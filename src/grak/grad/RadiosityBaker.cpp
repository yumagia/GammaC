#include "RadiosityBaker.hpp"

#include <iostream>
#include <cstdlib>

#define TRACE_PUSH_DIST		1.0f
#define TRACE_MAX_DIST		10000
#define NUM_COLLECT_SAMPLES	1000
#define NUM_BOUNCES			6

RadiosityBaker::RadiosityBaker() {
	numLumels = 0;
}

Vec3f RadiosityBaker::SampleUnitSphere() {
	Vec3f point(SampleNormalDistribution(), SampleNormalDistribution(), SampleNormalDistribution());
	point.Normalize();

	return point;
}

float RadiosityBaker::SampleNormalDistribution() {
	float theta = 2 * PI * (rand() / ((float) RAND_MAX));
	float rho = sqrt(-2 * log(rand() / ((float) RAND_MAX)));

	return rho * cos(theta);
}

bool RadiosityBaker::SampleIsLegal(Vec3f samplePosition, FileFace *face) {
	FilePlane *facePlane = &bspFile->filePlanes[face->planeNum];

	// Determine the major axis
	int major = (facePlane->type) % 3;

	float sampleX, sampleY;
	if(major == 0) {
		sampleX = samplePosition.z;
		sampleY = samplePosition.y;
	}
	else if(major == 1) {
		sampleX = samplePosition.x;
		sampleY = samplePosition.z;
	}
	else {
		sampleX = samplePosition.y;
		sampleY = samplePosition.x;
	}
	
	int numPos, numNeg;
	numPos = numNeg = 0;
	float xv1, yv1, xv2, yv2;
	FileVert currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert + (face->numVerts - 1)]];
	xv2 = currVert.point[(major + 2) % 3];
	yv2 = currVert.point[(major + 1) % 3];
	for(int i = 0; i < face->numVerts; i++) {
		currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert + i]];
		xv1 = xv2;
		yv1 = yv2;
		xv2 = currVert.point[(major + 2) % 3];
		yv2 = currVert.point[(major + 1) % 3];

		// Edge Vector
		float ex = -yv2 + yv1;
		float ey = xv2 - xv1;

		// Sample vector
		float esx = sampleX - xv1;
		float esy = sampleY - yv1;

		if((ex * esx) + (ey * esy) > 0) {
			numPos++;
		}
		else {
			numNeg++;
		}
	}

	return !(numPos > 0 && numNeg > 0);
}

// TODO: Get this working
bool RadiosityBaker::SquareSampleIsLegal(Vec3f samplePosition, float extent, FileFace *face) {
	FilePlane *facePlane = &bspFile->filePlanes[face->planeNum];

	// Determine the major axis
	int major = (facePlane->type) % 3;

	float sampleX, sampleY;
	if(major == 0) {
		sampleX = samplePosition.z;
		sampleY = samplePosition.y;
	}
	else if(major == 1) {
		sampleX = samplePosition.x;
		sampleY = samplePosition.z;
	}
	else {
		sampleX = samplePosition.y;
		sampleY = samplePosition.x;
	}
	
	int numPos, numNeg;
	numPos = numNeg = 0;
	float xv1, yv1, xv2, yv2;
	FileVert currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert + (face->numVerts - 1)]];
	xv2 = currVert.point[(major + 2) % 3];
	yv2 = currVert.point[(major + 1) % 3];
	for(int i = 0; i < face->numVerts; i++) {
		currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert + i]];
		xv1 = xv2;
		yv1 = yv2;
		xv2 = currVert.point[(major + 2) % 3];
		yv2 = currVert.point[(major + 1) % 3];

		// Edge normal vector
		float eX = -yv2 + yv1;
		float eY = xv2 - xv1;

		float mag = sqrt(eX * eX + eY * eY);
		eX = eX / mag;
		eY = eY / mag;

		float extX = eX * extent;
		float extY = eY * extent;
		float dist = extX * extX + extY * extY;
		
		// Sample vector
		float esX = sampleX - xv1;
		float esY = sampleY - yv1;

		if((eX * esX + eY * esY) > -dist) {
			numPos++;
		}
		else if((eX * esX + eY * esY) < dist) {
			numNeg++;
		}
	}

	return !(numPos > 0 && numNeg > 0);
}

void RadiosityBaker::PatchesForFace(FileFace *face) {
	FilePlane *facePlane = &bspFile->filePlanes[face->planeNum];
	int major;

	face->lightMapOffset = numLumels;

	// Determine the major axis
	major = facePlane->type;
	if(facePlane->type > 2) {
		major = facePlane->type - 3;
	}

	// Find min and max bounds of projected polygon
	FileVert currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert]];
	Vec3f min(currVert.point[0], currVert.point[1], currVert.point[2]);
	Vec3f max = Vec3f(min);
	for(int i = 1; i < face->numVerts; i++) {
		currVert = bspFile->fileVerts[bspFile->fileFaceVerts[face->firstMeshVert + i]];

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
		cz1 = (facePlane->normal[1] * min.y + facePlane->normal[2] * min.z + facePlane->dist);
		min.x = cz1 / facePlane->normal[0];

		cz2 = (facePlane->normal[1] * max.y + facePlane->normal[2] * max.z + facePlane->dist);
		max.x = cz2 / facePlane->normal[0];
	}
	else if(major == 1) {
		cz1 = (facePlane->normal[2] * min.z + facePlane->normal[0] * min.x + facePlane->dist);
		min.y = cz1 / facePlane->normal[1];

		cz2 = (facePlane->normal[2] * max.z + facePlane->normal[0] * max.x + facePlane->dist);
		max.y = cz2 / facePlane->normal[1];
	}
	else {
		cz1 = (facePlane->normal[0] * min.x + facePlane->normal[1] * min.y + facePlane->dist);
		min.z = cz1 / facePlane->normal[2];

		cz2 = (facePlane->normal[0] * max.x + facePlane->normal[1] * max.y + facePlane->dist);
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
	face->lightMapS[major] = (	face->lightMapOrigin[(major + 1) % 3] * facePlane->normal[(major + 1) % 3]		// Solution for the unknown component of S
								+ farVert[(major + 2) % 3] * facePlane->normal[(major + 2) % 3]	) / facePlane->normal[major];
	face->lightMapT[(major + 2) % 3] = 0;
	face->lightMapT[major] = (	face->lightMapOrigin[(major + 1) % 3] * facePlane->normal[(major + 1) % 3]		// Same thing, but for T
								+ farVert[(major + 2) % 3] * facePlane->normal[(major + 2) % 3]	) / facePlane->normal[major];
	
	float sExtent = face->lightMapS[(major + 2) % 3];
	float tExtent = face->lightMapT[(major + 1) % 3];
	face->lightMapWidth = ceil(sExtent / PATCH_SIZE) + 1;
	face->lightMapHeight = ceil(tExtent / PATCH_SIZE) + 1;
	
	float magS = sqrt(face->lightMapS[0] * face->lightMapS[0] + face->lightMapS[1] * face->lightMapS[1] + face->lightMapS[2] * face->lightMapS[2]);
	face->lightMapS[0] /= magS;
	face->lightMapS[1] /= magS;
	face->lightMapS[2] /= magS;
	float magT = sqrt(face->lightMapT[0] * face->lightMapT[0] + face->lightMapT[1] * face->lightMapT[1] + face->lightMapT[2] * face->lightMapT[2]);
	face->lightMapT[0] /= magT;
	face->lightMapT[1] /= magT;
	face->lightMapT[2] /= magT;

	// Create the actual patches
	Vec3f sVec = Vec3f(face->lightMapS[0], face->lightMapS[1], face->lightMapS[2]);
	Vec3f tVec = Vec3f(face->lightMapT[0], face->lightMapT[1], face->lightMapT[2]);
	Vec3f lmOrigin = Vec3f(face->lightMapOrigin[0], face->lightMapOrigin[1], face->lightMapOrigin[2]);
	for(int tStep = 0; tStep < face->lightMapHeight; tStep++) {
		for(int sStep = 0; sStep < face->lightMapWidth; sStep++) {
			Vec3f s = sVec * (sStep * PATCH_SIZE);
			Vec3f t = tVec * (tStep * PATCH_SIZE);

			Vec3f samplePosition = s + t + lmOrigin;

			if(numLumels > MAX_MAP_LUMELS) {
				std::cerr << "Reached MAX_MAP_LUMELS: " << MAX_MAP_LUMELS << std::endl;
			}

			Patch *patch = &patchList[numLumels];
			patch->legal = SquareSampleIsLegal(samplePosition, PATCH_SIZE / 2, face);
			patch->faceIndex = face - bspFile->fileFaces;
			patch->position = samplePosition;
			patch->NudgePosition(bspFile);

			numLumels++;

		}
	}

	// Shift back origin to cover smaller faces
	Vec3f shiftOrigin(face->lightMapOrigin[0], face->lightMapOrigin[1], face->lightMapOrigin[2]);
	if(face->lightMapWidth == 1) {
		shiftOrigin = shiftOrigin 
						+ ((0.5 * (sExtent - PATCH_SIZE)) * sVec);
	}
	if(face->lightMapHeight == 1) {
		shiftOrigin = shiftOrigin 
						+ ((0.5 * (tExtent - PATCH_SIZE))* tVec);
	}
	face->lightMapOrigin[0] = shiftOrigin.x;
	face->lightMapOrigin[1] = shiftOrigin.y;
	face->lightMapOrigin[2] = shiftOrigin.z;
	
	// Generate vertex lightmap UVS
	for(int i = 0; i < face->numVerts; i++) {
		FileVert* vert = &bspFile->fileVerts[face->firstVert + i];
		vert->lightMapUV[0] = (vert->point[(major + 2) % 3] - face->lightMapOrigin[(major + 2) % 3]) / PATCH_SIZE;
		vert->lightMapUV[1] = (vert->point[(major + 1) % 3] - face->lightMapOrigin[(major + 1) % 3]) / PATCH_SIZE;
	}
}

void RadiosityBaker::InitLightMaps() {
	std::cout << "--- Initializing Lightmaps ---" << std::endl;
	int numFaces = bspFile->fileHeader.lumps[LUMP_FACES].length;
	for(int i = 0; i < numFaces; i++) {
		PatchesForFace(&bspFile->fileFaces[i]);
	}

	std::cout << "Successfully generated " << numLumels << " number of patches for face" << std::endl;
}

void RadiosityBaker::InitialLightingPass() {
	std::cout << "--- Direct Lighting Pass ---" << std::endl;

	std::cout << "Collecting lighting for all faces..." << std::endl;
	int numFaces = bspFile->fileHeader.lumps[LUMP_FACES].length;
	for(int i = 0; i < numFaces; i++) {
		CollectLightingForFace(&bspFile->fileFaces[i]);
	}

	std::cout << "Finished!" << std::endl;
}

void RadiosityBaker::CollectLightingForFace(FileFace *face) {
	int		lmWidth, lmHeight, lmOffset;
	Vec3f	lmOrigin;

	FileMaterial *faceMaterial = &bspFile->fileMaterials[face->material];
	Color faceDiffuse = Color(faceMaterial->diffuse[0], faceMaterial->diffuse[1], faceMaterial->diffuse[2]);

	lmOffset = face->lightMapOffset;
	lmWidth = face->lightMapWidth;
	lmHeight = face->lightMapHeight;
	lmOrigin = Vec3f(face->lightMapOrigin[0], face->lightMapOrigin[1], face->lightMapOrigin[2]);

	Vec3f sVec = Vec3f(face->lightMapS[0], face->lightMapS[1], face->lightMapS[2]);
	Vec3f tVec = Vec3f(face->lightMapT[0], face->lightMapT[1], face->lightMapT[2]);
	for(int tStep = 0; tStep < lmHeight; tStep++) {
		for(int sStep = 0; sStep < lmWidth; sStep++) {
			Vec3f s = sVec * ((sStep + 0.5f) * PATCH_SIZE);
			Vec3f t = tVec * ((tStep + 0.5f) * PATCH_SIZE);

			Vec3f samplePosition = lmOrigin + s + t;
			
			Patch *patch = &patchList[lmOffset + (tStep * lmWidth) + sStep];

			if(patch->legal) {
				CollectLightingForPatch(patch, samplePosition);
			}
		}
	}

	for(int j = 0; j < lmHeight; j++) {
		for(int i = 0; i < lmWidth; i++) {
			Patch *patch = &patchList[lmOffset + (j * lmWidth) + i];
			if(!(patch->legal)) {
				int validNeighbors = 0;
				Color neighborSum = Color();
				Patch *neighborPatch = NULL;

				neighborPatch = &patchList[lmOffset + ((j - 1) * lmWidth) + (i - 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + ((j - 1) * lmWidth) + i];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + ((j - 1) * lmWidth) + (i + 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}

				neighborPatch = &patchList[lmOffset + (j * lmWidth) + (i - 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + (j * lmWidth) + i];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + (j * lmWidth) + (i + 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}

				neighborPatch = &patchList[lmOffset + ((j + 1) * lmWidth) + (i - 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + ((j + 1) * lmWidth) + i];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}
				neighborPatch = &patchList[lmOffset + ((j + 1) * lmWidth) + (i + 1)];
				if(neighborPatch->legal && (&bspFile->fileFaces[neighborPatch->faceIndex] == face)) {
					neighborSum = neighborSum + neighborPatch->diffuse;
					validNeighbors++;
				}

				if(validNeighbors > 0) {
					Color averageColor = neighborSum / validNeighbors;

					patch->sampledLight = averageColor * faceDiffuse;
					patch->accumulatedLight = patch->sampledLight;
				}

			}
		}
	}
}

void RadiosityBaker::CollectLightingForPatch(Patch *patch, Vec3f samplePosition) {
	Color	collectedLighting, averageLighting;
	int		samplesCollected = 0;
	int		escapes = 0;

	FileFace *patchFace = &bspFile->fileFaces[patch->faceIndex];
	FileMaterial *lumelMaterial = &bspFile->fileMaterials[patchFace->material];
	FilePlane *lumelPlane = &bspFile->filePlanes[patchFace->planeNum];
	Vec3f normal(lumelPlane->normal[0], lumelPlane->normal[1], lumelPlane->normal[2]);
	Trace trace(bspFile);
	Vec3f pos(samplePosition + (TRACE_PUSH_DIST * normal));

	Color skyColorSample = Color(0.6, 0.8, 1);

	for(int i = 0; i < NUM_COLLECT_SAMPLES; i++) {
		Vec3f cosineWeighted = (normal + SampleUnitSphere());
		cosineWeighted.Normalize();
		Vec3f disp = TRACE_MAX_DIST * cosineWeighted;
		if(trace.TraceLine(pos, pos + disp)) {			// Hit a surface, now find which face we struck
			FileNode *hitNode = &bspFile->fileNodes[trace.hitNodeIdx];
			FilePlane *hitPlane = &bspFile->filePlanes[hitNode->planeNum];
			Vec3f hitNormal(hitPlane->normal[0], hitPlane->normal[1], hitPlane->normal[2]);

			float t = -(hitNormal.Dot(pos) - hitPlane->dist) / hitNormal.Dot(disp);
			Vec3f hitPoint(pos + (disp * t));

			int hitFaceIndex = FindStruckFace(hitNode, hitPoint);
			if(hitFaceIndex == -1) {	// A miss, common and important to acknowledge
			}
			else {
				FileMaterial *hitMaterial = &bspFile->fileMaterials[bspFile->fileFaces[hitFaceIndex].material];
				Color materialEmmisive(hitMaterial->emissive[0], hitMaterial->emissive[1], hitMaterial->emissive[2]);

				if(materialEmmisive.SquareMagnitude() > 0.001f) {
					collectedLighting = collectedLighting + materialEmmisive;
				}

				samplesCollected++;
			}
		}
		else {		// Escaped to sky, sample it
			collectedLighting = collectedLighting + skyColorSample;

			samplesCollected++;
			escapes++;
		}
	}

	if(samplesCollected) {
		averageLighting = collectedLighting / samplesCollected;
	}

	Color diffuse(lumelMaterial->diffuse[0], lumelMaterial->diffuse[1], lumelMaterial->diffuse[2]);
	Color emissive(lumelMaterial->emissive[0], lumelMaterial->emissive[1], lumelMaterial->emissive[2]);


	patch->sampledLight = averageLighting * diffuse;
	Color patchColor = averageLighting + emissive;

	patch->diffuse = diffuse;
	patch->accumulatedLight = patchColor;
	patch->escapeRate = 1;
	if(!(samplesCollected == 0)) {
		patch->escapeRate = escapes / (float) samplesCollected;
	}

	if(patchColor.Normalize() > 1.f) {
		patch->accumulatedLight = patchColor;
	}
}


// Find the index of the node face containing the given point
int RadiosityBaker::FindStruckFace(FileNode *node, Vec3f position) {
	int firstFaceIdx = node->firstFace;
	for(int i = 0; i < node->numFaces; i++) {
		FileFace *face = &bspFile->fileFaces[firstFaceIdx + i];
		if(SampleIsLegal(position, face)) {
			return firstFaceIdx + i;
		}
	}

	return -1;
}

// Find the lumel associated closest to a position on a node, return its index
int RadiosityBaker::FindNodeLumel(FileNode *node, Vec3f position) {
	int firstFaceIdx = node->firstFace;
	for(int i = 0; i < node->numFaces; i++) {
		FileFace *face = &bspFile->fileFaces[firstFaceIdx + i];
		int patchIdx = FindFaceLumel(face, position);
		if(patchIdx >= 0) {
			return patchIdx;
		}
	}

	return -1;
}

// Returns an index, -1 means no valid or legal lumel was found
int RadiosityBaker::FindFaceLumel(FileFace *face, Vec3f position) {
	FilePlane *facePlane = &bspFile->filePlanes[face->planeNum];

	float minBound[2], mappedPos[2];

	int major = (facePlane->type) % 3;
	if(major == 0) {
		minBound[0] = face->lightMapOrigin[2];
		minBound[1] = face->lightMapOrigin[1];

		mappedPos[0] = position.z;
		mappedPos[1] = position.y;
	}
	else if(major == 1) {
		minBound[0] = face->lightMapOrigin[0];
		minBound[1] = face->lightMapOrigin[2];

		mappedPos[0] = position.x;
		mappedPos[1] = position.z;
	}
	else {
		minBound[0] = face->lightMapOrigin[1];
		minBound[1] = face->lightMapOrigin[0];

		mappedPos[0] = position.y;
		mappedPos[1] = position.x;
	}

	float s = (mappedPos[0] - minBound[0]) / PATCH_SIZE;
	float t = (mappedPos[1] - minBound[1]) / PATCH_SIZE;

	// Quick and simple check to see if a point falls in-bounds of a lightmap
	if(s < 0 || t < 0) {
		return -1;
	}
	if(s > face->lightMapWidth || t > face->lightMapHeight) {
		return -1;
	}

	int patchX = s + 0.5;
	int patchY = t + 0.5;
	int patchIdx = face->lightMapOffset + (patchY * face->lightMapWidth) + patchX;

	// Might as well just check if its legal also
	if(patchList[patchIdx].legal) {
		return patchIdx;
	}
	
	return -1;
}

void RadiosityBaker::CreatePatchTransfers() {
	std::cout << "--- CreatePatchTransfers ---" << std::endl;

	std::cout << "Calculating patch transfers..." << std::endl;

	int progress = 0;
	int divisor = 12;

	for(int i = 0; i < numLumels; i++) {
		Patch *patch = &patchList[i];

		patch->CalcTransfersForpatch(numLumels, patchList, bspFile);

		if(i > (progress * (numLumels / (float) divisor))) {
			progress++;

			std::cout << "[";
			for(int j = 0; j < progress; j++) {
				std::cout << "###";
			}
			for(int j = progress; j < divisor; j++) {
				std::cout << "   ";
			}
			std::cout << "]" << std::endl;
		}
	}

	std::cout << "Finished creating transfers" << std::endl;
}

void RadiosityBaker::BounceLight() {
	std::cout << "--- BounceLight ---" << std::endl;

	std::cout << "Shooting light..." << std::endl;

	Color *collectedLight = new Color[numLumels];

	for(int bounces = 0; bounces < NUM_BOUNCES; bounces++) {
		std::cout << "Bounce " << bounces + 1 << " out of " << NUM_BOUNCES << std::endl;

		for(int i = 0; i < numLumels; i++) {
			collectedLight[i] = Color(0, 0, 0);
		}

		for(int i = 0; i < numLumels; i++) {
			Patch *patch = &patchList[i];

			patch->ShootLight(collectedLight);
		}

		for(int i = 0; i < numLumels; i++) {
			Patch *patch = &patchList[i];

			patch->CollectLight(collectedLight[i]);
		}
	}

	delete[] collectedLight;
}

void RadiosityBaker::FreePatchTransfers() {
	std::cout << "--- Free Transfers ---" << std::endl;

	for(int i = 0; i < numLumels; i++) {
		Patch *patch = &patchList[i];

		if(patch->legal) {
			patch->FreeTransfers();
		}
	}

	std::cout << "Freed all transfers" << std::endl;
}

int RadiosityBaker::BakeRad(BspFile *bspFile) {
	patchList = new Patch[MAX_MAP_LUMELS];
	this->bspFile = bspFile;
	numLumels = 0;

	InitLightMaps();
	InitialLightingPass();
	CreatePatchTransfers();
	BounceLight();
	FreePatchTransfers();

	// Write the patch lighting into the lumel lump
	for(int i = 0; i < numLumels; i++) {
		FileLumel *lumel = &(this->bspFile)->fileLightmaps[i];
		Patch *patch = &patchList[i];

		// Clamp it real quick again
		Color patchColor = patch->accumulatedLight;
		if(patchColor.Normalize() > 1) {
			patch->accumulatedLight = patchColor;
		}

		lumel->color[0] = patch->accumulatedLight.r;
		lumel->color[1] = patch->accumulatedLight.g;
		lumel->color[2] = patch->accumulatedLight.b;

		lumel->faceIndex = patch->faceIndex;
		lumel->legal = patch->legal;
	}

	// Set numLumels
	bspFile->fileHeader.lumps[LUMP_LUMELS].length = numLumels;

	delete[] patchList;

	return numLumels;
}

int RadiosityBaker::GetNumLumels() {
	return numLumels;
}
