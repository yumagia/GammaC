#include "cmdlib.h"

#define	BASEDIRNAME	"gammac"

void Error(const char *error, ...) {
	printf(error);
	printf("");
	exit(1);
}

std::string gdir;
std::string gamedir;

void SetGdirFromPath(fs::path inputpath) {
	fs::path::iterator	c;
	fs::path path = inputpath;

	if(!(inputpath.is_absolute())) {
		path = fs::current_path() / inputpath;
	}

	while(!G_stringcasecomp(path.filename(), BASEDIRNAME)) {
		path = path.parent_path();
		if(path == path.root_path()) {
			Error("SetGdirFromPath: no %s in %s", BASEDIRNAME, path);
		}
	}
	std::cout << path << std::endl;
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

std::string ExpandPath(char *path) {

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

int LoadFile(char *filename, void **bufferptr) {
	int		length;
	std::ifstream file(filename);

	if(file.is_open()) {
		std::streampos fileSize = file.tellg();
		length = static_cast<int>(fileSize);
	} else {
		Error("Error opening file.");
	}

	return length;
}