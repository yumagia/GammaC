#ifndef GBSP_WRITER_INCLUDED
#define GBSP_WRITER_INCLUDED

#include "Bsp.hpp"
#include "GammaFile.hpp"
#include "FileWriter.hpp"

#include <string>

class GbspWriter : public FileWriter {
public:
	GbspWriter() {}
	~GbspWriter();

	void WriteMap(const char *mapDir, const char *bspLevelName);
	
private:
	void BeginBspFile();
	void AddWorldModel(BspModel *model);
	void EndBspFile();

	int		EmitTree(BspNode *node);
	// -1 denotes air leaf, positive integers are solid
	int		EmitLeaf(BspNode *node);
	void	EmitFace(std::shared_ptr<BspFace> face);
	void	EmitPlanes();
	void	EmitVerts();

	std::vector<std::string> ParseArgsFromLine(std::string line);
	
private:
    int startLeaf;
    int startFaceVert;
    int startFace;
};

#endif