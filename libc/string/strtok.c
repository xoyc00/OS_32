#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

char* strtok(char* str, char* tokens) { 
	static char* temp;
	static char* old_temp;
	char* s;
   
    //If string passed to function is not null, copy it to our static variable
    if(str!=0)
    {
		if (temp!=0) { free(old_temp); temp = 0; }
        temp=(char*)malloc(strlen(str));
        strcpy(temp,str);
		old_temp = temp;
		s = malloc(strlen(str));
		strcpy(s, str);
    }

    //If the string passed is NULL and even the copy is NULL, we are done and return NULL.
    else if(temp==0)
    	return 0;

    //If only the string passed is NULL and the copy still has data, work with it.
    else
    {
        s=temp;
    }

	if (strlen(temp) == 0) {
		return 0;
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
                    s++;
                }
                else
                {
                    temp++;
                    s[chars]='\0';
                    return s;
                }
            }
        }
        if(flag==0)
            chars++;
        temp++;
        flag=0;
    }
    s[chars]='\0';
    return s;
}
