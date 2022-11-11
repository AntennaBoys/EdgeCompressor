#ifndef MATLIB
#define MATLIB
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct Mat{
	double* entries;
	int row;
	int col;
} Mat;

void showmat(Mat* A);


Mat* newmat(int r,int c,double d);

void freemat(Mat* A);
double get(Mat* M,int r,int c);
void set(Mat* M,int r,int c,double d);

Mat* submat(Mat* A,int r1,int r2,int c1,int c2);

Mat* multiply(Mat* A,Mat* B);
Mat* removerow(Mat* A,int r);

void removecol2(Mat* A,Mat* B,int c);
Mat* transpose(Mat* A);
double det(Mat* M);


Mat* inverse(Mat* A);

Mat* copyvalue(Mat* A);
#endif