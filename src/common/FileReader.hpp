#ifndef FILE_READER_INCLUDED
#define FILE_READER_INCLUDED

#include "GammaFile.hpp"

#include <string>

class FileReader {
public:
	void ReadFile(std::string fileName);
};

#endif