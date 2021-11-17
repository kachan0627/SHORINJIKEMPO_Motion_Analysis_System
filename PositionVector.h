#ifndef __POSITIONVECTOR
#define __POSITIONVECTOR
#include "math.h"
#include "stdlib.h"
#include"Calculation.h"
//#include"RotationMatrix.h"
class PositionVector
{
public:

	struct Vector
	{
		float x; float y;float z;//(x,y,z)

	};

	struct VectorMatrix
	{
		float xa; float xb; float xc;
		float ya; float yb; float yc;
		float za; float zb; float zc;
		/*(xa,xb,xc)
		  (ya,yb,yc)
		  (za,zb,zc)*/
	};
	PositionVector();
	~PositionVector();
	void CreatePV(float,float,float);
	void CreateVMx(float);
	void CreateVMy(float);
	void CreateVMz(float);
	void CreateVM(Vector);//�x�N�g������
	void MatrixMultiplication(VectorMatrix,VectorMatrix,VectorMatrix);//�s��̊|���Z
	
	// ���_��\���x�N�g��
//	float original_vector[3];
	
	
	
	Vector V;
	VectorMatrix VM;
};
#endif
