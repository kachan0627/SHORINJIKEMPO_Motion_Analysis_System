#pragma once
#include "math.h"
#include "stdlib.h"

//#define PI 3.14159265	//�~����

class CCalc
{
public:
	CCalc(void);
	~CCalc(void);
	// ���ς����߂�
	void GetJointRotateAngle(float  child_vector[3], float  rotate_angle[3]);
	void OuterProduct(float  va[3], float  vb[3], float  OP[3]);
	float GetAngle(float  va[3], float  vb[3], float  axis[3], float * sin, float * cos);
	float GetDistance(float  va[3], float  vb[3]);
	float InnerProduct(float  va[3], float  vb[3]);
private:
	// ���_��\���x�N�g��
	float original_vector[3];
	const double PI = 3.14159265;	//�~����
public:
	// �t�s������߂�
	void GetInverseMatrix(float* matrix, float* i_matrix);
	//�x�N�g���ƍs��̊|���Z
	void MultVectorMatrix(float* input_v, float* output_v, float* matrix);
	// ��]�ʂ����߂�
	float GetRotateAngle(float* va, float* vb);
	// �x�N�g���𐳋K������
	void NormalizeVector(float *vector);
	//�s��~�s��
	void MultMatrix(float* matrix_a, float* matrix_b, float* matrix_out);
	//cos�Ƃ�Ԃ�
	float GetCosAngle(float* va, float* vb);
	//�������̌v�Z
	float CalcCbRoot(float x);
	double GetStartValue(double x);
	double GetCbRoot(double x, double stv);
	//�s�񎮂̎Z�o(3���F�T���X�̕��@)
	float CalcSarrus(float *matrix);
	// RGB��HSV�ɕϊ�
	float GetAngle2(float va[3], float vb[3]);
	void RGB2HSV(double* H, double* S, double* V, double R, double G, double B);
	double max_color(double r, double g, double b);
	double min_color(double r, double g, double b);
	// HSV����RGB�ɕϊ�
	void HSV2RGB(float* rr, float* gg, float* bb, float H, float S, float V);
	// �x�N�g���̒��������߂ĕԂ�(�������ɑΉ�)
	float VectorLength(float * vector, int num);
	// �@���̎Z�o
	void CalcNormalVector(float* xyz1, float* xyz2, float* xyz3, float* norm);
};
