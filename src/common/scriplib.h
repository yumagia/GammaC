#define	MAXTOKEN	1024

extern	char	token[MAXTOKEN];
extern	char	*scriptbuffer,*script_p,*scriptend_p;
extern	int		grabbed;
extern	int		scriptline;
extern	bool	endofscript;

void LoadScriptFile(char *filename);
void ParseFromMemory(char *buffer, int size);

bool GetToken(bool crossline);
void UnGetToken(void);
bool TokenAvailable(void);