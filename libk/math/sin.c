#include <math.h>

#define PI 3.1415926535897932384650288

double sin (double x) {
	x*=180.0/PI;
	double sign=1;
	if (x<0){
    	sign=-1.0;
    	x=-x;
  	}
  	if (x>360) x -= (int)(x/360)*360;
  	x*=PI/180.0;
  	double res=0;
  	double term=x;
  	int k=1;
  	while (res+term!=res){
    	res+=term;
    	k+=2;
    	term*=-x*x/k/(k-1);
  	}

  	return sign*res;
}

float sinf (float x) {
	x*=180.0f/PI;
	float sign=1;
	if (x<0){
    	sign=-1.0;
    	x=-x;
  	}
  	if (x>360) x -= (int)(x/360)*360;
  	x*=PI/180.0f;
  	float res=0;
  	float term=x;
  	int k=1;
  	while (res+term!=res){
    	res+=term;
    	k+=2;
    	term*=-x*x/k/(k-1);
  	}

  	return sign*res;
}

long double sinl (long double x) {
	x*=180.0/PI;
	long double sign=1;
	if (x<0){
    	sign=-1.0;
    	x=-x;
  	}
  	if (x>360) x -= (int)(x/360)*360;
  	x*=PI/180.0;
  	long double res=0;
  	long double term=x;
  	int k=1;
  	while (res+term!=res){
    	res+=term;
    	k+=2;
    	term*=-x*x/k/(k-1);
  	}

  	return sign*res;
}
