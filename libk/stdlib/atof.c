#include <stdlib.h>

double atof(const char *arr){
    int i,j,flag;
    double val;
    char c;
	int sign = 1;

	if (*arr == '-') {
		sign = -1;
		arr++;
	}

    i=0;
    j=0;
    val=0;
    flag=0;
    while ((c = *(arr+i))!='\0'){
//      if ((c<'0')||(c>'9')) return 0;
        if (c!='.'){
            val =(val*10)+(c-'0');
            if (flag == 1){
                --j;
            }
        }
        if (c=='.'){ if (flag == 1) return 0; flag=1;}
        ++i;
    }
	double p = 1;
	for (i = 0; i > j; i--) {
		p /= 10;
	}
    val = val*p;
    return sign * val;
}
