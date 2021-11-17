#include"Calculation.h"
float  InnerProduct(float* vector_a, float* vector_b){
	//aÅEb=ax*bx+ay*by+az*bz
	return vector_a[0] * vector_b[0] + vector_a[1] * vector_b[1] + vector_a[2] * vector_b[2];
}
float* CrossProduct(float* vector_a, float* vector_b){
	/*aÅ~b=ay*bz-az*by,
		   az*bx-ax*bz,
		   ax*by-ay*bx*/
	float * OutPutVector = NULL;
	OutPutVector[0] = vector_a[1] * vector_b[2] - vector_a[2] * vector_b[1];
	OutPutVector[1] = vector_a[2] * vector_b[0] - vector_a[0] * vector_b[2];
	OutPutVector[2] = vector_a[0] * vector_b[1] - vector_a[1] * vector_b[0];
	return OutPutVector;
}
float  Norm(float *vector){
	//|a|^2=ax^2+ay^2+az^2
	return sqrt(pow(vector[0], 2) + pow(vector[1], 2)+pow(vector[2], 2));
}
