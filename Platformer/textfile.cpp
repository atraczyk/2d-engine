//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) 
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	text file utilites 
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* textFileRead(char *fn) 
{
	FILE *file;
	char *content = NULL;

	int count=0;

	if (fn != NULL) 
	{
		fopen_s(&file, fn, "rt");

		if (file != NULL) 
		{

			fseek(file, 0, SEEK_END);
			count = ftell(file);
			rewind(file);

			if (count > 0) 
			{
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,file);
				content[count] = '\0';
			}
			fclose(file);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s) 
{
	FILE *file;
	int status = 0;

	if (fn != NULL) 
	{
		fopen_s(&file, fn, "rt");

		if (file != NULL) 
		{

			if (fwrite(s,sizeof(char),strlen(s),file) == strlen(s))
				status = 1;
			fclose(file);
		}
	}
	return(status);
}







