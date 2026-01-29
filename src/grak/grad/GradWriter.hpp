#ifndef GRAD_WRITER_INCLUDED
#define GRAD_WRITER_INCLUDED

#include "GammaFile.hpp"
#include "FileWriter.hpp"

#include <string>
#include <map>

class GradWriter : public FileWriter {
public:
	GradWriter(BspFile *bspFile);

	void	SetNumLumels(int numLumels);
	
};

#endif