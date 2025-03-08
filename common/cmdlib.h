
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

void    Error(char *error, ...);

FILE	*SafeOpenWrite(char *filename);
FILE	*SafeOpenRead(char *filename);
void	SafeRead(FILE *f, void *buffer, int count);
void	SafeWrite(FILE *f, void *buffer, int count);

int		LoadFile(char *filename, void **bufferptr);
int		TryLoadFile(char *filename, void **bufferptr);
void	SaveFile(char *filename, void *buffer, int count);
bool	FileExists(char *filename);

short	BigShort (short l);
short	LittleShort (short l);
int		BigLong (int l);
int		LittleLong (int l);
float	BigFloat (float l);
float	LittleFloat (float l);