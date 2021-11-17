#pragma once
//#include<gl/gl.h>
#include <GL/glut.h>
class CQuaternion
{
public:
	CQuaternion(void);
	~CQuaternion(void);
	// 回転を表すクォータニオン
	float Quaternion_U[4];
private:
	// 初期化関数
	void InitQuaternion(void);
	const double PI = 3.14159265;	//円周率
public:
	// クォータニオンのコンストラクタ
	CQuaternion(float angle, float* axis);
	// 回転行列
	float rotate_matrix[16];
	// 回転行列をつくる
	void creat_matrix(void);
	void SetValue(float in_real, float in_x, float in_y, float in_z);
	// ４×４の行列を３×３に置きかえる
	void Relocation_Matrix(float* re_matrix);
private:
	// 共役クォータニオン
	float Quaternion_Conj[4];
public:
	// クォータにオンの逆元
	float Quaternion_Inverse[4];
	// クォータニオンの逆元を求める
	void GetInverseQuaternion(void);
private:
	// 共役クォータニオンを求める
	void SetQuaternion_Conj(void);
	// クォータニオンのノルム（大きさ）の2乗を求める
	float GetQuaternionNorm(void);
public:
	// クォータニオンの掛け算(Quaternion_a * Quaternion_b)
	void Multiplication_Quaternion(float Quaternion_a[4], float Quaternion_b[4], float Quaternion_m[4]);
	// q_base/q_emotionより生成されるクォータニオンクラスを生成するコンストラクタ
	CQuaternion(CQuaternion q_base, CQuaternion q_emotion);
	//クォータニオンの一時利用に使用
	void SetQuaternion(float angle, float* axis);
	// 新規作成
	void InitAllClass(void);
	// 回転行列をクォータニオンに変換してセットする
	void QuatConvertFromMatrix(float in_matrix[16]);
	// 2つのクォータニオンをin_weightで補間する
	void QuatSlerp(CQuaternion in_qua1, CQuaternion in_qua2, float in_weight);
	void MakeQuat(float r[3]);
	// 行列からクォータニオンへの変換
	void MatrixToQuaternion(float * matrix, float * quaternion);
	// 球面線形補間
	void SLERPwithQuaternion(float * quaternion_A, float * quaternion_B, float * out_quaternion, float weight);
	// クォータニオンの正規化
	void NormalizeQuaternion(float * quaternion);
	// クオータニオンから行列への変換
	void QuaternionToMatrix(float * quaternion, float * matrix);
};
