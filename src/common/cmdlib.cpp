#include "cmdlib.h"

#define	BASEDIRNAME	"gammac"

void Error(const char *error, ...) {
	printf(error);
	printf("");
	exit(1);
}

std::string gdir = "";
std::string gamedir = "";

void SetGdirFromPath(fs::path inputpath) {
	fs::path::iterator	c;
	fs::path path = ExpandArg(inputpath);

	while(!G_stringcasecomp(path.filename(), BASEDIRNAME)) {
		path = path.parent_path();
		if(path == path.root_path()) {
			Error("SetGdirFromPath: no %s in %s", BASEDIRNAME, path);
		}
	}
	std::cout << "Gamedir: " << path << std::endl;
	gdir = path;
	// for(c = path.end(); *c != path; c--) {
	// 	if (*c == BASEDIRNAME) {
	// 		fs::path foundPath;
	// 		while(c != path.begin()) {
	// 			if(*c == path.root_path()) {
	// 				gdir = foundPath;
	// 				std::cout << "gamedir:" << gamedir;
	// 			}
	// 			foundPath /= *c;
	// 			c++;
	// 		}
	// 		Error("No gamedir in %s", path);
	// 		return;
	// 	}
	// 	Error("SetGdirFromPath: no %s in %s", BASEDIRNAME, path);
	// 	return;
	// }

}

std::string ExpandArg(fs::path inputpath) {
	fs::path path = inputpath;

	if(!(inputpath.is_absolute())) {
		path = fs::current_path() / inputpath;
	}

	return path;
}

std::string ExpandPath(fs::path inputpath) {
	if(gdir == "") {
		Error("ExpandPath called without gdir assignment");
	}
	if(inputpath.is_absolute()) {
		return inputpath;
	}
	return gdir / inputpath;
}

std::string G_stringtolower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(),
					[](unsigned char c){return std::tolower(c);});
	return s;
}

bool G_stringcasecomp(std::string a, std::string b) {
	return G_stringtolower(a) == G_stringtolower(b);
}

/**=============================================================================
 * 
 * 		MISC FUNCTIONS
 * 
 * =============================================================================
 */

int LoadFile(std::string filename, std::string &buffer) {
	std::ifstream file;
	int		length;
	std::string content;
	std::stringstream sstreambuffer;

	file.open(filename, std::ios::in);

	if(!file) {
		Error("Error opening file.");
	}
	
	length = 0;
	while(std::getline(file, content)) {
		length++;
	}
	file.clear();
	file.seekg(0, std::ios::beg);

	sstreambuffer.str("");
	sstreambuffer << file.rdbuf();
	buffer = sstreambuffer.str();
	return length;
}

void DefaultExtension(std::string& path, std::string extension) {
	std::string src;
	std::string b;

	src = path;

	while(b != "/" && src != path) {
		b = src.back();
		if(b == ".") {
			return;					//it has an extension
		}
		src.pop_back();
	}

	path += extension;
}

void	StripExtension(std::string path) {
	std::string temppath;

	temppath = path;
	while(temppath.length() > 0 && temppath.back() == '.') {
		if(temppath.back() == '/') {
			return;		// No extension
		}
	}
	if(temppath.length() > 0) {
		path = temppath;
	}
}