#pragma once
#include "math.h"
#include "stdlib.h"

//#define PI 3.14159265	//円周率

class CCalc
{
public:
	CCalc(void);
	~CCalc(void);
	// 内積を求める
	void GetJointRotateAngle(float  child_vector[3], float  rotate_angle[3]);
	void OuterProduct(float  va[3], float  vb[3], float  OP[3]);
	float GetAngle(float  va[3], float  vb[3], float  axis[3], float * sin, float * cos);
	float GetDistance(float  va[3], float  vb[3]);
	float InnerProduct(float  va[3], float  vb[3]);
private:
	// 原点を表すベクトル
	float original_vector[3];
	const double PI = 3.14159265;	//円周率
public:
	// 逆行列を求める
	void GetInverseMatrix(float* matrix, float* i_matrix);
	//ベクトルと行列の掛け算
	void MultVectorMatrix(float* input_v, float* output_v, float* matrix);
	// 回転量を求める
	float GetRotateAngle(float* va, float* vb);
	// ベクトルを正規化する
	void NormalizeVector(float *vector);
	//行列×行列
	void MultMatrix(float* matrix_a, float* matrix_b, float* matrix_out);
	//cosθを返す
	float GetCosAngle(float* va, float* vb);
	//立方根の計算
	float CalcCbRoot(float x);
	double GetStartValue(double x);
	double GetCbRoot(double x, double stv);
	//行列式の算出(3次：サラスの方法)
	float CalcSarrus(float *matrix);
	// RGBをHSVに変換
	float GetAngle2(float va[3], float vb[3]);
	void RGB2HSV(double* H, double* S, double* V, double R, double G, double B);
	double max_color(double r, double g, double b);
	double min_color(double r, double g, double b);
	// HSVからRGBに変換
	void HSV2RGB(float* rr, float* gg, float* bb, float H, float S, float V);
	// ベクトルの長さを求めて返す(多次元に対応)
	float VectorLength(float * vector, int num);
	// 法線の算出
	void CalcNormalVector(float* xyz1, float* xyz2, float* xyz3, float* norm);
};
