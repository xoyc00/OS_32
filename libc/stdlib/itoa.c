#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/* A utility function to reverse a string  */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

char* itoa(int value, char* str, int base) {
	int i = 0; 
    bool isNegative = false; 
  
    /* Handle 0 explicitely, otherwise empty string is printed for 0 */
    if (value == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
  
    // In standard itoa(), negative numbers are handled only with  
    // base 10. Otherwise numbers are considered unsigned. 
    if (value < 0 && base == 10) 
    { 
        isNegative = true; 
        value = -value; 
    } 
  
    // Process individual digits 
    while (value != 0) 
    { 
        int rem = value % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        value = value/base; 
    } 
  
    // If number is negative, append '-' 
    if (isNegative) 
        str[i++] = '-'; 
  
    str[i] = '\0'; // Append string terminator 
  
    // Reverse the string 
    reverse(str); 
  
    return str; 
}
