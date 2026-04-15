//-----------------------------------------------------------------------------
//	2d platform game engine and level editor
//		email: 
//
//	DESCRIPTION:	text file utilites
//-----------------------------------------------------------------------------

#include "textfile.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

char* textFileRead(const char *fn)
{
    FILE *file;
    char *content = NULL;

    int count = 0;

    if (fn != NULL)
    {
        const std::string resolvedPath = resolveResourcePath(fn);
        file = fopen(resolvedPath.c_str(), "rt");

        if (file != NULL)
        {
            fseek(file, 0, SEEK_END);
            count = ftell(file);
            rewind(file);

            if (count > 0)
            {
                content = (char *)malloc(sizeof(char) * (count + 1));
                count = static_cast<int>(fread(content, sizeof(char), count, file));
                content[count] = '\0';
            }
            fclose(file);
        }
    }
    return content;
}

int textFileWrite(const char *fn, const char *s)
{
    FILE *file;
    int status = 0;

    if (fn != NULL)
    {
        file = fopen(fn, "wt");

        if (file != NULL)
        {
            if (fwrite(s, sizeof(char), strlen(s), file) == strlen(s))
                status = 1;
            fclose(file);
        }
    }
    return(status);
}
