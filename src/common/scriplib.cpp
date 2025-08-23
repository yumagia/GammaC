#include "cmdlib.h"
#include "scriplib.h"

/**=============================================
 * PARSING STUFF
 * 
 * =============================================
 */

typedef struct {
	std::string			filename;
	char	*buffer,*script_p,*end_p;
	int		line;
} script_t;

#define MAX_INCLUDES	8
script_t	scriptstack[MAX_INCLUDES];
script_t	*script;
int		scriptline;

char	token[MAXTOKEN];
bool	endofscript;
bool	tokenready;					// only true if UnGetToken was just called

void AddScriptToStack(fs::path inputpath) {
	int				size;

	script++;
	if(script == &scriptstack[MAX_INCLUDES]) {
		Error("Script file exceeded MAX_INCLUDES");
	}
	script->filename = ExpandPath(inputpath);
}

void LoadScriptFile(fs::path inputpath) {
	script = scriptstack;
	AddScriptToStack(inputpath);

	endofscript = false;
	tokenready = false;
}

void ParseFromMemory(char *buffer, int size) {
	script = scriptstack;
	script++;
	if(script == &scriptstack[MAX_INCLUDES]) {
		Error("script file exceeded MAX_INCLUDES");
	}
	std::string (script->filename) = "memory buffer";

	script->buffer = buffer;
	script->line = 1;
	script->script_p = script->buffer;
	script->end_p = script->buffer + size;

	endofscript = false;
	tokenready = false;
}

void UnGetToken(void) {
	tokenready = true;
}

bool EndOfScript(bool crossline) {
	if(!crossline) {
		Error("Line %i is incomplete\n",scriptline);
	}
	if(script->filename == "memory buffer") {
		endofscript = true;
		return false;
	}

	free(script->buffer);
	if(script == scriptstack + 1) {
		endofscript = true;
		return false;
	}
	script--;
	scriptline = script->line;
	std::cout << "Returning to " << script->filename << std::endl;
	return GetToken(crossline);
}

bool GetToken(bool crossline) {
	char	*token_p;

	if(tokenready) {				// It was already waiting perhaps
		tokenready = false;
		return true;
	}

	if(script->script_p >= script->end_p) {
		return EndOfScript(crossline);
	}

	skipspace:
		while(*script->script_p <= 32) {
			
		}
}