#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

char* strcpy(char* dest, const char* src) {
	if (sizeof(dest) < sizeof(src))
		return NULL;
	for (int i = 0; i < strlen(src); i++) {
		dest[i] = src[i];
	}

	return dest;
}

char* strtok(char* str, char* tokens) { 
	static char* temp;
	static char* old_temp;
   
    //If string passed to function is not null, copy it to our static variable
    if(str!=0)
    {
		if (temp!=0) { free(old_temp); temp = 0; }
        temp=(char*)malloc(strlen(str));
        strcpy(temp,str);
		old_temp = temp;
    }

    //If the string passed is NULL and even the copy is NULL, we are done and return NULL.
    else if(temp==0)
    	return 0;

    //If only the string passed is NULL and the copy still has data, work with it.
    else
    {
        str=temp;
    }

    int chars=0, len = strlen(tokens), flag=0;
    
    //Run the loop till we find a token or our copy is fully parsed.
    while(*temp)
    {
        for(int i=0;i<len;i++)
        {
            if(*temp==tokens[i])
            {
                if(chars==0)
                {
                    flag=1;
                    str++;
                }
                else
                {
                    temp++;
                    str[chars]='\0';
                    return str;
                }
            }
        }
        if(flag==0)
            chars++;
        temp++;
        flag=0;
    }
    str[chars]='\0';
    return str;
}
