#define	MAXTOKEN	1024

extern	std::string		token;
extern	char	*scriptbuffer,*script_p,*scriptend_p;
extern	int		grabbed;
extern	int		scriptline;
extern	bool	endofscript;

void LoadScriptFile(fs::path inputpath);
void ParseFromMemory(const std::string& buffer, int size);

bool GetToken(bool crossline);
void UnGetToken(void);
bool TokenAvailable(void);