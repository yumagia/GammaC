#pragma once

#include "cmdlib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <windows.h>

/*
=================
Error
=================
*/
void Error (char *error, ...)
{
	va_list                 argptr;
	char	                text[1024];
	char	                text2[1024];
	int		                err;

	err = GetLastError ();

	va_start(argptr,error);
	vsprintf(text, error,argptr);
	va_end(argptr);

	sprintf(text2, "%s\nGetLastError() = %i", text, err);
        //MessageBox(NULL, text2, "Error", 0 /* MB_OK */);

	exit(1);
}