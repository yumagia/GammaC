#include "cmdlib.h"
#include "scriplib.h"

/**=============================================
 * PARSING STUFF
 * 
 * =============================================
 */

typedef struct {
	char	filename[1024];
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

void AddScriptToStack(char *filename) {
	int				size;

	script++;
	if(script == &scriptstack[MAX_INCLUDES]) {
		Error("Script file exceeded MAX_INCLUDES");
	}
	std::string (script->filename) = ExpandPath(filename);
}

void LoadScriptFile(char *filename) {
	script = scriptstack;
	AddScriptToStack(filename);

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