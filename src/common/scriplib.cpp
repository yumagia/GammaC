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

void AddScriptToStack (char *filename) {
	int				size;

	script++;
	if(script == &scriptstack[MAX_INCLUDES]) {
		Error("Script file exceeded MAX_INCLUDES");
	}
}

void LoadScriptFile(char *filename) {
	script = scriptstack;
	
}