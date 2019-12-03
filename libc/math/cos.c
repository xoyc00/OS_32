#include <math.h>

#define PI 3.1415926535897932384650288

double cos (double x) {
	x*=180.0/PI;
	if (x<0) x=-x;
  	if (x>360) x -= (int)(x/360)*360;
	x*=PI/180.0;
	double res=0;
	double term=1;
	int k=0;
	while (res+term!=res){
		res+=term;
		k+=2;
		term*=-x*x/k/(k-1);
	}  
	return res;
}

float cosf (float x) {
	x*=180.0f/PI;
	if (x<0) x=-x;
  	if (x>360) x -= (int)(x/360)*360;
	x*=PI/180.0f;
	float res=0;
	float term=1;
	int k=0;
	while (res+term!=res){
		res+=term;
		k+=2;
		term*=-x*x/k/(k-1);
	}  
	return res;
}

long double cosl (long double x) {
	x*=180.0/PI;
	if (x<0) x=-x;
  	if (x>360) x -= (int)(x/360)*360;
	x*=PI/180.0;
	long double res=0;
	long double term=1;
	int k=0;
	while (res+term!=res){
		res+=term;
		k+=2;
		term*=-x*x/k/(k-1);
	}  
	return res;
}
