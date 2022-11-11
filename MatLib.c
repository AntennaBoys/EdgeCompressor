#include "MatLib.h"
void showmat(Mat* A){
	if(A->row>0&&A->col>0){
		int k=0;
		printf("[");
		for(int i=1;i<=A->row;i++){
			for (int j=1;j<=A->col;j++){
				if(j<A->col){
					printf("%.20f\t",A->entries[k++]);
				}else{
					printf("%.20f",A->entries[k++]);
				}
			}
			if(i<A->row){
				printf("\n");
			}else{
				printf("]\n");
			}
		}	
		printf("\n");
	}else{
		printf("[]");
	}
}
Mat* newmat(int r,int c,double d){
	Mat* M=(Mat*)malloc(sizeof(Mat));			
	M->row=r;M->col=c;
	M->entries=(double*)malloc(sizeof(double)*r*c);
	int k=0;
	for(int i=1;i<=M->row;i++){
		for(int j=1;j<=M->col;j++){
			M->entries[k++]=d;
		}
	}
	return M;
}
void freemat(Mat* A){
	free(A->entries);
	free(A);
}
double get(Mat* M,int r,int c){
	double d=M->entries[(r-1)*M->col+c-1];
	return d;
}
void set(Mat* M,int r,int c,double d){
	M->entries[(r-1)*M->col+c-1]=d;
}



Mat* submat(Mat* A,int r1,int r2,int c1,int c2){
	Mat* B=newmat(r2-r1+1,c2-c1+1,0);
	int k=0;
	for(int i=r1;i<=r2;i++){
		for(int j=c1;j<=c2;j++){
			B->entries[k++]=A->entries[(i-1)*A->col+j-1];
		}
	}
	return B;
}
Mat* scalermultiply(Mat* M,double c){	
	Mat* B=newmat(M->row,M->col,0);
	int k=0;
	for(int i=0;i<M->row;i++){
		for(int j=0;j<M->col;j++){
			B->entries[k]=M->entries[k]*c;
			k+=1;
		}
	}
	return B;
}
Mat* multiply(Mat* A,Mat* B){
	int r1=A->row;
	int r2=B->row;
	int c1=A->col;
	int c2=B->col;
	if (r1==1&&c1==1){
		Mat* C=scalermultiply(B,A->entries[0]);
		return C;
	}else if (r2==1&&c2==1){
		Mat* C=scalermultiply(A,B->entries[0]);
		return C;
	}
	Mat* C=newmat(r1,c2,0);
	for(int i=1;i<=r1;i++){
		for(int j=1;j<=c2;j++){
			double de=0;
			for(int k=1;k<=r2;k++){
				de+=A->entries[(i-1)*A->col+k-1]*B->entries[(k-1)*B->col+j-1];
			}
			C->entries[(i-1)*C->col+j-1]=de;
		}
	}
	return C;
}
Mat* removerow(Mat* A,int r){
	Mat* B=newmat(A->row-1,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k]=A->entries[(i-1)*A->col+j-1];
				k+=1;
			}
		}
	}
	return B;
}

void removecol2(Mat* A,Mat* B,int c){	
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(j!=c){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];				
			}
		}
	}	
}
Mat* transpose(Mat* A){
	Mat* B=newmat(A->col,A->row,0);
	int k=0;
	for(int i=1;i<=A->col;i++){
		for(int j=1;j<=A->row;j++){
			B->entries[k]=A->entries[(j-1)*A->col+i-1];
			k+=1;
		}
	}
	return B;
}
double det(Mat* M){
	int r=M->row;
	int c=M->col;
	if(r==1&&c==1){
		double d=M->entries[0];
		return d;
	}
	Mat* M1=removerow(M,1);
	Mat* M2=newmat(M->row-1,M->col-1,0);
	double d=0, si=+1;
	for(int j=1;j<=M->col;j++){
		double c=M->entries[j-1];		
		removecol2(M1,M2,j);				
		d+=si*det(M2)*c;
		si*=-1;
	}
	freemat(M1);
	freemat(M2);
	return d;
}
void removerow2(Mat* A,Mat* B,int r){	
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];				
			}
		}
	}	
}
Mat* adjoint(Mat* A){
	Mat* B=newmat(A->row,A->col,0);	
	Mat* A1=newmat(A->row-1,A->col,0);
	Mat* A2=newmat(A->row-1,A->col-1,0);
	for(int i=1;i<=A->row;i++){
		removerow2(A,A1,i);
		for(int j=1;j<=A->col;j++){			
			removecol2(A1,A2,j);
			double si=pow(-1,(double)(i+j));
			B->entries[(i-1)*B->col+j-1]=det(A2)*si;			
		}		
	}
	Mat* C=transpose(B);
	freemat(A1);
	freemat(A2);
	freemat(B);
	return C;
}
Mat* inverse(Mat* A){
	Mat* B=adjoint(A);
	double de=det(A);
	Mat* C=scalermultiply(B,1/de);
	freemat(B);
	return C;
}

Mat* copyvalue(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			B->entries[k]=A->entries[k];
			k++;
		}
	}
	return B;
}