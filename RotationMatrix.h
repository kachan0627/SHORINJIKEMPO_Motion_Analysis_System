#ifndef __ROTATIONMATRIX
#define __ROTATIONMATRIX
#include "math.h"
#include "stdlib.h"
#include"Calculation.h"
#include "PositionVector.h"
class RotationMatrix{
	//��]�s��
public:
	RotationMatrix();
	~RotationMatrix();
	void clear();
	void CreateRM(float*);//��]�s��̐���
	void CreateRM(float,float,float);//��]�s��̐���
	void XRotationMatrix(float);//x���܂��̉�]�s��
	void YRotationMatrix(float);//y���܂��̉�]�s��
	void ZRotationMatrix(float);//z���܂��̉�]�s��
	void rRotationMatrix(float,float,float);//��]�s��
	void CreateRV(PositionVector, PositionVector);
	void MatrixMultiplication(PositionVector, PositionVector);
	PositionVector r_Vector;
	PositionVector xr_Vector;
	PositionVector yr_Vector;
	PositionVector zr_Vector;
private:
	
	// ���_��\���x�N�g��
//	float original_vector[3];
	
	//float e_x[3] = { 1.0, 0.0, 0.0 };//x�̒P�ʃx�N�g��
	//float e_y[3] = { 0.0, 1.0, 0.0 };//y�̒P�ʃx�N�g��
	//float e_z[3] = { 0.0, 0.0, 1.0 };//z�̒P�ʃx�N�g��

};



#endif