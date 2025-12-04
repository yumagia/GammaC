#ifndef LEVEL_WRITER_INCLUDED
#define LEVEL_WRITER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.hpp"
#include "FileWriter.hpp"

#include <string>

class GbspWriter : public FileWriter {
public:
	void BeginBspFile();
	void AddWorldModel(BspModel *model);
	void EndBspFile();
private:
	int		EmitTree(BspNode *node);
	// -1 denotes air leaf, positive integers are solid
	int		EmitLeaf(BspNode *node);
	void	EmitFace(std::shared_ptr<BspFace> face);
	void	EmitPlanes();
	void	EmitVerts();
	
private:
    int startLeaf;
    int startFaceVert;
    int startFace;
};

#endif