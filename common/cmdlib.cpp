#pragma once

#include "cmdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <windows.h>



void Error(char *error, ...) {
	va_list                 argptr;
	char	                text[1024];
	char	                text2[1024];
	int		                err;

	err = GetLastError();

	va_start(argptr,error);
	vsprintf(text, error,argptr);
	va_end(argptr);

	sprintf(text2, "%s\nGetLastError() = %i", text, err);
        //MessageBox(NULL, text2, "Error", 0 /* MB_OK */);

	exit(1);
}

/**
 * =============================================================================
 * 
 * 		MISC FUNCTIONS
 * 
 * =============================================================================
 */

/**
 * @brief Like pretty much all of GBSP's functions, this one is directly from Quake
 */
int Q_filelength(FILE *f) {
	int		pos;
	int		end;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}


FILE *SafeOpenWrite(char *filename)
{
	FILE	*f;

	f = fopen(filename, "wb");

	if (!f) {
		Error ("Error opening %s: %s",filename,strerror(errno));
	}

	return f;
}


FILE	*SafeOpenRead(char *filename) {
	FILE	*f;

	f = fopen(filename, "rb");

	if (!f) {
		Error ("Error opening %s: %s",filename,strerror(errno));
	}

	return f;
}


void	SafeRead(FILE *f, void *buffer, int count)
{
	if (fread(buffer, 1, count, f) != (size_t)count) {
		Error ("File read failure");
	}
}


void	SafeWrite(FILE *f, void *buffer, int count) {
	if (fwrite(buffer, 1, count, f) != (size_t)count) {
		Error("File write failure");
	}
}


bool	FileExists(char *filename) {
	FILE	*f;

	f = fopen(filename, "r");
	if (!f) {
		return false;
	}
	fclose (f);
	return true;
}


int    LoadFile(char *filename, void **bufferptr) {
	FILE	*f;
	int    length;
	void    *buffer;

	f = SafeOpenRead(filename);
	length = Q_filelength(f);
	buffer = malloc(length+1);
	((char *)buffer)[length] = 0;
	SafeRead(f, buffer, length);
	fclose(f);

	*bufferptr = buffer;
	return length;
}


/**
 * @brief Allows failure
 */
int    TryLoadFile(char *filename, void **bufferptr) {
	FILE	*f;
	int    length;
	void    *buffer;

	*bufferptr = NULL;

	f = fopen(filename, "rb");
	if (!f) {
		return -1;
	}
	length = Q_filelength(f);
	buffer = malloc(length+1);
	((char *)buffer)[length] = 0;
	SafeRead(f, buffer, length);
	fclose(f);

	*bufferptr = buffer;
	return length;
}


void    SaveFile(char *filename, void *buffer, int count)
{
	FILE	*f;

	f = SafeOpenWrite(filename);
	SafeWrite(f, buffer, count);
	fclose(f);
}

/**
 * =============================================================================
 * 
 * 		BYTE ORDER FUNCTIONS
 * 
 * =============================================================================
 */