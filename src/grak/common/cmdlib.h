#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>
#include <cstddef>

#include <algorithm>
#include <exception>
#include <iostream>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <string>


void    Error(const char *error, ...);


extern std::string gdir;
extern std::string gamedir;
void SetGdirFromPath(fs::path inputpath);
std::string ExpandArg(fs::path inputpath);
std::string ExpandPath(fs::path inputpath);

std::string G_stringtolower(std::string s);
bool G_stringcasecomp(std::string a, std::string b);

void DefaultExtension(std::string& path, std::string extension);
void	StripExtension(std::string path);
int LoadFile(std::string filename, std::string &buffer);