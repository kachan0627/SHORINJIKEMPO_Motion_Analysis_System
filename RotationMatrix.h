#ifndef __ROTATIONMATRIX
#define __ROTATIONMATRIX
#include "math.h"
#include "stdlib.h"
#include"Calculation.h"
#include "PositionVector.h"
class RotationMatrix{
	//回転行列
public:
	RotationMatrix();
	~RotationMatrix();
	void clear();
	void CreateRM(float*);//回転行列の生成
	void CreateRM(float,float,float);//回転行列の生成
	void XRotationMatrix(float);//x軸まわりの回転行列
	void YRotationMatrix(float);//y軸まわりの回転行列
	void ZRotationMatrix(float);//z軸まわりの回転行列
	void rRotationMatrix(float,float,float);//回転行列
	void CreateRV(PositionVector, PositionVector);
	void MatrixMultiplication(PositionVector, PositionVector);
	PositionVector r_Vector;
	PositionVector xr_Vector;
	PositionVector yr_Vector;
	PositionVector zr_Vector;
private:
	
	// 原点を表すベクトル
//	float original_vector[3];
	
	//float e_x[3] = { 1.0, 0.0, 0.0 };//xの単位ベクトル
	//float e_y[3] = { 0.0, 1.0, 0.0 };//yの単位ベクトル
	//float e_z[3] = { 0.0, 0.0, 1.0 };//zの単位ベクトル

};



#endif