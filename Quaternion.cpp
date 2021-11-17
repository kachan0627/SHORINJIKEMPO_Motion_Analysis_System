//#include "StdAfx.h"
#include ".\quaternion.h"
#include "math.h"
#include "Calc.h"
#include <GL/glut.h>

CQuaternion::CQuaternion(void)
{
}

CQuaternion::~CQuaternion(void)
{
}

// 初期化関数
void CQuaternion::InitQuaternion(void)
{
	for (int i = 0; i < 4; i++){
		Quaternion_U[i] = 0.0;
	}
	for (int j = 0; j < 16; j++){
		rotate_matrix[j] = 0.0;
	}
	for (int k = 0; k < 4; k++){
		Quaternion_Conj[k] = 0.0;
	}
	for (int l = 0; l < 4; l++){
		Quaternion_Inverse[l] = 0.0;
	}
}

// クォータニオンのコンストラクタ
CQuaternion::CQuaternion(float angle, float* axis)
{
	InitQuaternion();	//初期化

	CCalc calc;
	double sin_angle;

	angle = angle / 2;
	sin_angle = sin((double)angle);	//sinを求める

	if (axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0){
		angle = 0.0;
	}

	if (angle != 0.0){
		calc.NormalizeVector(axis);	//回転軸の正規化
	}

	//実数部
	Quaternion_U[0] = (float)cos((double)angle);
	//虚数部
	for (int i = 0; i < 3; i++){
		Quaternion_U[i + 1] = (float)(axis[i] * sin_angle);
	}
}

// 回転行列をつくる
void CQuaternion::creat_matrix(void)
{
	//回転なし⇒単位行列
	if (Quaternion_U[0] == 0.0){

		glPushMatrix();
		glLoadIdentity();
		glGetFloatv(GL_MODELVIEW_MATRIX, rotate_matrix);
		glPopMatrix();

	}
	else{
		rotate_matrix[0] = (GLfloat)(1.0 - 2.0 * (Quaternion_U[2] * Quaternion_U[2] + Quaternion_U[3] * Quaternion_U[3]));
		rotate_matrix[1] = (GLfloat)(2.0 * (Quaternion_U[1] * Quaternion_U[2] + Quaternion_U[3] * Quaternion_U[0]));
		rotate_matrix[2] = (GLfloat)(2.0 * (Quaternion_U[3] * Quaternion_U[1] - Quaternion_U[0] * Quaternion_U[2]));
		rotate_matrix[3] = 0.0;

		rotate_matrix[4] = (GLfloat)(2.0 * (Quaternion_U[1] * Quaternion_U[2] - Quaternion_U[3] * Quaternion_U[0]));
		rotate_matrix[5] = (GLfloat)(1.0 - 2.0 * (Quaternion_U[3] * Quaternion_U[3] + Quaternion_U[1] * Quaternion_U[1]));
		rotate_matrix[6] = (GLfloat)(2.0 * (Quaternion_U[2] * Quaternion_U[3] + Quaternion_U[0] * Quaternion_U[1]));
		rotate_matrix[7] = 0.0;

		rotate_matrix[8] = (GLfloat)(2.0 * (Quaternion_U[3] * Quaternion_U[1] + Quaternion_U[0] * Quaternion_U[2]));
		rotate_matrix[9] = (GLfloat)(2.0 * (Quaternion_U[2] * Quaternion_U[3] - Quaternion_U[1] * Quaternion_U[0]));
		rotate_matrix[10] = (GLfloat)(1.0 - 2.0 * (Quaternion_U[2] * Quaternion_U[2] + Quaternion_U[1] * Quaternion_U[1]));
		rotate_matrix[11] = 0.0;

		rotate_matrix[12] = 0.0;
		rotate_matrix[13] = 0.0;
		rotate_matrix[14] = 0.0;
		rotate_matrix[15] = 1.0;
	}

}

//すでにクォータニオンの値が求まっている場合に使用する
void CQuaternion::SetValue(float in_real, float in_x, float in_y, float in_z)
{
	Quaternion_U[0] = in_real;
	Quaternion_U[1] = in_x;
	Quaternion_U[2] = in_y;
	Quaternion_U[3] = in_z;
}

// ４×４の行列を３×３に置きかえる
void CQuaternion::Relocation_Matrix(float* re_matrix)
{
	re_matrix[0] = rotate_matrix[0];
	re_matrix[1] = rotate_matrix[1];
	re_matrix[2] = rotate_matrix[2];
	re_matrix[3] = rotate_matrix[4];
	re_matrix[4] = rotate_matrix[5];
	re_matrix[5] = rotate_matrix[6];
	re_matrix[6] = rotate_matrix[8];
	re_matrix[7] = rotate_matrix[9];
	re_matrix[8] = rotate_matrix[10];
}

// 共役クォータニオンを求める
void CQuaternion::SetQuaternion_Conj(void)
{
	Quaternion_Conj[0] = Quaternion_U[0];
	Quaternion_Conj[1] = Quaternion_U[1] * -1;
	Quaternion_Conj[2] = Quaternion_U[2] * -1;
	Quaternion_Conj[3] = Quaternion_U[3] * -1;
}

// クォータニオンのノルム（大きさ）の2乗を求める
float CQuaternion::GetQuaternionNorm(void)
{
	float norm = 0.0;
	norm = (Quaternion_U[0] * Quaternion_U[0]) + (Quaternion_U[1] * Quaternion_U[1]) + (Quaternion_U[2] * Quaternion_U[2]) + (Quaternion_U[3] * Quaternion_U[3]);
	return norm;
}

// クォータニオンの逆元を求める
void CQuaternion::GetInverseQuaternion(void)
{
	float q_norm = 0.0;
	q_norm = GetQuaternionNorm();	//ノルムの2乗を求める
	SetQuaternion_Conj();	//共役クォータニオンを求める

	//逆元の計算(共役クォータニオン/ノルムの2乗)
	for (int i = 0; i < 4; i++){
		Quaternion_Inverse[i] = Quaternion_Conj[i] / q_norm;
	}
}

// クォータニオンの掛け算(Quaternion_a * Quaternion_b)
void CQuaternion::Multiplication_Quaternion(float Quaternion_a[4], float Quaternion_b[4], float Quaternion_m[4])
{
	Quaternion_m[0] = (Quaternion_a[0] * Quaternion_b[0]) - (Quaternion_a[1] * Quaternion_b[1]) - (Quaternion_a[2] * Quaternion_b[2]) - (Quaternion_a[3] * Quaternion_b[3]);
	Quaternion_m[1] = (Quaternion_a[0] * Quaternion_b[1]) + (Quaternion_a[1] * Quaternion_b[0]) + (Quaternion_a[2] * Quaternion_b[3]) - (Quaternion_a[3] * Quaternion_b[2]);
	Quaternion_m[2] = (Quaternion_a[0] * Quaternion_b[2]) - (Quaternion_a[1] * Quaternion_b[3]) + (Quaternion_a[2] * Quaternion_b[0]) + (Quaternion_a[3] * Quaternion_b[1]);
	Quaternion_m[3] = (Quaternion_a[0] * Quaternion_b[3]) + (Quaternion_a[1] * Quaternion_b[2]) - (Quaternion_a[2] * Quaternion_b[1]) + (Quaternion_a[3] * Quaternion_b[0]);
}

// q_base/q_emotionより生成されるクォータニオンクラスを生成するコンストラクタ
CQuaternion::CQuaternion(CQuaternion q_base, CQuaternion q_emotion)
{
	InitQuaternion();	//初期化

	Multiplication_Quaternion(q_emotion.Quaternion_U, q_base.Quaternion_Inverse, Quaternion_U);	//q_emorion * q_base-1をクォータニオンとして格納

}

//クォータニオンの一時利用に使用
void CQuaternion::SetQuaternion(float angle, float* axis)
{
	CCalc calc;
	double sin_angle;

	angle = angle / 2;
	sin_angle = sin((double)angle);	//sinを求める

	if (axis[0] == 0.0 && axis[1] == 0.0 && axis[2] == 0.0){
		angle = 0.0;
	}

	if (angle != 0.0){
		calc.NormalizeVector(axis);	//回転軸の正規化
	}

	//実数部
	Quaternion_U[0] = (float)cos((double)angle);
	//虚数部
	for (int i = 0; i < 3; i++){
		Quaternion_U[i + 1] = (float)(axis[i] * sin_angle);
	}
}

// 新規作成
void CQuaternion::InitAllClass(void)
{
	InitQuaternion();

}

void CQuaternion::QuatConvertFromMatrix(float in_matrix[16])
{

	float s;
	float tr = in_matrix[0] + in_matrix[5] + in_matrix[10] + (float)1.0;

	if (tr >= 1.0) {

		s = (float) 0.5 / sqrt(tr);

		Quaternion_U[0] = (float) 0.25 / s;
		Quaternion_U[1] = (in_matrix[6] - in_matrix[9]) * s;
		Quaternion_U[2] = (in_matrix[8] - in_matrix[2]) * s;
		Quaternion_U[3] = (in_matrix[1] - in_matrix[4]) * s;

	}
	else {

		float max;

		if (in_matrix[5] > in_matrix[10]){
			max = in_matrix[5];
		}
		else{
			max = in_matrix[10];
		}

		if (max < in_matrix[0]) {
			s = sqrt(in_matrix[0] - (in_matrix[5] + in_matrix[10]) + (float)1.0);

			float tmp_x = s * (float)0.5;

			s = (float)0.5 / s;

			Quaternion_U[1] = tmp_x;
			Quaternion_U[2] = (in_matrix[1] + in_matrix[4]) * s;
			Quaternion_U[3] = (in_matrix[8] + in_matrix[2]) * s;
			Quaternion_U[0] = (in_matrix[6] - in_matrix[9]) * s;

		}
		else if (max == in_matrix[5]) {

			s = sqrt(in_matrix[5] - (in_matrix[10] + in_matrix[0]) + (float)1.0);

			float tmp_y = s * (float)0.5;

			s = (float)0.5 / s;

			Quaternion_U[1] = (in_matrix[1] + in_matrix[4]) * s;
			Quaternion_U[2] = tmp_y;
			Quaternion_U[3] = (in_matrix[6] + in_matrix[9]) * s;
			Quaternion_U[0] = (in_matrix[8] - in_matrix[2]) * s;

		}
		else {
			s = sqrt(in_matrix[10] - (in_matrix[0] + in_matrix[5]) + (float)1.0);

			float tmp_z = s * (float)0.5;

			s = (float)0.5 / s;

			Quaternion_U[1] = (in_matrix[8] + in_matrix[2]) * s;
			Quaternion_U[2] = (in_matrix[6] + in_matrix[9]) * s;
			Quaternion_U[3] = tmp_z;
			Quaternion_U[0] = (in_matrix[1] - in_matrix[4]) * s;

		}
	}
}

// 2つのクォータニオンをin_weightで補間してセットする
void CQuaternion::QuatSlerp(CQuaternion in_qua1, CQuaternion in_qua2, float in_weight)
{
	//in_qua1とin_qua2が同じクォータニオンの時
	if (in_qua1.Quaternion_U[0] == in_qua2.Quaternion_U[0] && in_qua1.Quaternion_U[1] == in_qua2.Quaternion_U[1] && in_qua1.Quaternion_U[2] == in_qua2.Quaternion_U[2] && in_qua1.Quaternion_U[3] == in_qua2.Quaternion_U[3]){

		//同じクォータニオンの値をセット
		for (int i = 0; i < 4; i++){
			Quaternion_U[i] = in_qua1.Quaternion_U[i];
		}
	}

	float qr = in_qua1.Quaternion_U[0] * in_qua2.Quaternion_U[0] + in_qua1.Quaternion_U[1] * in_qua2.Quaternion_U[1] + in_qua1.Quaternion_U[2] * in_qua2.Quaternion_U[2] + in_qua1.Quaternion_U[3] * in_qua2.Quaternion_U[3];
	float ss = (float)1.0 - (qr * qr);

	if (qr > 1.0){

		//同じクォータニオンの値をセット
		for (int i = 0; i < 4; i++){
			Quaternion_U[i] = in_qua1.Quaternion_U[i];
		}

	}
	else if (ss == 0.0){

		//同じクォータニオンの値をセット
		for (int i = 0; i < 4; i++){
			Quaternion_U[i] = in_qua1.Quaternion_U[i];
		}

	}
	else{

		float ph = acos(qr);	//角度

		if (qr < 0.0 && ph > PI / 2.0){	//90度以上

			float s1, s2;
			qr = -in_qua1.Quaternion_U[0] * in_qua2.Quaternion_U[0] - in_qua1.Quaternion_U[1] * in_qua2.Quaternion_U[1] - in_qua1.Quaternion_U[2] * in_qua2.Quaternion_U[2] - in_qua1.Quaternion_U[3] * in_qua2.Quaternion_U[3];
			ph = acos(qr);

			s1 = (float)sin(ph * (1.0 - in_weight)) / sin(ph);
			s2 = (float)sin(ph * in_weight) / sin(ph);

			Quaternion_U[0] = in_qua1.Quaternion_U[0] * s1 - in_qua2.Quaternion_U[0] * s2;
			Quaternion_U[1] = in_qua1.Quaternion_U[1] * s1 - in_qua2.Quaternion_U[1] * s2;
			Quaternion_U[2] = in_qua1.Quaternion_U[2] * s1 - in_qua2.Quaternion_U[2] * s2;
			Quaternion_U[3] = in_qua1.Quaternion_U[3] * s1 - in_qua2.Quaternion_U[3] * s2;

		}
		else{
			float s1, s2;

			s1 = sin(ph * ((float)1.0 - in_weight)) / sin(ph);
			s2 = sin(ph * in_weight) / sin(ph);

			Quaternion_U[0] = in_qua1.Quaternion_U[0] * s1 + in_qua2.Quaternion_U[0] * s2;
			Quaternion_U[1] = in_qua1.Quaternion_U[1] * s1 + in_qua2.Quaternion_U[1] * s2;
			Quaternion_U[2] = in_qua1.Quaternion_U[2] * s1 + in_qua2.Quaternion_U[2] * s2;
			Quaternion_U[3] = in_qua1.Quaternion_U[3] * s1 + in_qua2.Quaternion_U[3] * s2;
		}
	}
}

//オイラー角からクォータニオン
void CQuaternion::MakeQuat(float r[3])
{
	InitQuaternion();	//初期化

	CCalc calc;
	float Qx[4], Qy[4], Qz[4], tmp_Q[4];

	Qx[0] = cos(r[0] / 2);	Qx[1] = sin(r[0] / 2);	Qx[2] = 0;	Qx[3] = 0;
	Qy[0] = cos(r[1] / 2);	Qy[1] = 0;	Qy[2] = sin(r[1] / 2);	Qy[3] = 0;
	Qz[0] = cos(r[2] / 2);	Qz[1] = 0;	Qz[2] = 0;	Qz[3] = sin(r[2] / 2);

	Multiplication_Quaternion(Qz, Qy, tmp_Q);
	Multiplication_Quaternion(tmp_Q, Qx, Qz);

	//実数部
	Quaternion_U[0] = Qz[0];
	//虚数部
	for (int i = 1; i < 4; i++){
		Quaternion_U[i] = Qz[i];
	}
	//	Qx = [ cos(a/2), (sin(a/2), 0, 0)]
	//	Qy = [ cos(b/2), (0, sin(b/2), 0)]
	//	Qz = [ cos(c/2), (0, 0, sin(c/2))]
}



// 行列からクォータニオンへの変換      ここ凄く重要
void CQuaternion::MatrixToQuaternion(float * matrix, float * quaternion)
{
	float trace, temp, max;

	trace = matrix[0] + matrix[5] + matrix[10] + matrix[15];

	if (trace >= 1.0) {

		quaternion[0] = 0.5f * sqrt(trace);
		temp = 4.0f * quaternion[0];
		quaternion[1] = (matrix[6] - matrix[9]) / temp;
		quaternion[2] = (matrix[8] - matrix[2]) / temp;
		quaternion[3] = (matrix[1] - matrix[4]) / temp;

	}
	else {

		if (matrix[5] > matrix[10]) {
			max = matrix[5];
		}
		else {
			max = matrix[10];
		}

		if (max < matrix[0]) {

			quaternion[1] = 0.5f * sqrt(matrix[0] - matrix[5] - matrix[10] + matrix[15]);
			temp = 4.0f * quaternion[1];
			quaternion[2] = (matrix[1] + matrix[4]) / temp;
			quaternion[3] = (matrix[2] + matrix[8]) / temp;
			quaternion[0] = (matrix[6] - matrix[9]) / temp;

		}
		else if (max == matrix[5]) {

			quaternion[2] = 0.5f * sqrt(matrix[5] - matrix[0] - matrix[10] + matrix[15]);
			temp = 4.0f * quaternion[2];
			quaternion[0] = (matrix[8] - matrix[2]) / temp;
			quaternion[1] = (matrix[1] + matrix[4]) / temp;
			quaternion[3] = (matrix[6] + matrix[9]) / temp;

		}
		else if (max == matrix[10]) {

			quaternion[3] = 0.5f * sqrt(matrix[10] - matrix[0] - matrix[5] + matrix[15]);
			temp = 4.0f * quaternion[3];
			quaternion[0] = (matrix[1] - matrix[4]) / temp;
			quaternion[1] = (matrix[2] + matrix[8]) / temp;
			quaternion[2] = (matrix[6] + matrix[9]) / temp;

		}
	}
}


// 球面線形補間
void CQuaternion::SLERPwithQuaternion(float * quaternion_A, float * quaternion_B, float * out_quaternion, float weight)
{
	float inner;	//クォータニオンの内積（差分角度算出用）
	float sin_t;	//分母

	inner = quaternion_A[0] * quaternion_B[0] + quaternion_A[1] * quaternion_B[1] + quaternion_A[2] * quaternion_B[2] + quaternion_A[3] * quaternion_B[3];

	if (inner > 1.0) {
		inner = 1.0;
	}
	else if (inner < -1.0) {
		inner = -1.0;
	}

	sin_t = 1.0f - inner * inner;
	//	sin_t = 1.0f - inner;

	if (sin_t <= 0.00001) {

		out_quaternion[0] = quaternion_A[0];
		out_quaternion[1] = quaternion_A[1];
		out_quaternion[2] = quaternion_A[2];
		out_quaternion[3] = quaternion_A[3];

	}
	else if (inner < 0.0) {
		float sin1, sin2, acos_t;

		inner = -quaternion_A[0] * quaternion_B[0] - quaternion_A[1] * quaternion_B[1] - quaternion_A[2] * quaternion_B[2] - quaternion_A[3] * quaternion_B[3];
		acos_t = acos(inner);
		sin_t = 1.0f - inner * inner;
		sin_t = sqrt(sin_t);
		sin1 = sin((1.0f - weight) * acos_t);
		sin2 = sin(weight * acos_t);

		out_quaternion[0] = (quaternion_A[0] * sin1 - quaternion_B[0] * sin2) / sin_t;
		out_quaternion[1] = (quaternion_A[1] * sin1 - quaternion_B[1] * sin2) / sin_t;
		out_quaternion[2] = (quaternion_A[2] * sin1 - quaternion_B[2] * sin2) / sin_t;
		out_quaternion[3] = (quaternion_A[3] * sin1 - quaternion_B[3] * sin2) / sin_t;
	}
	else {
		float sin1, sin2, acos_t;

		acos_t = acos(inner);
		sin_t = sqrt(sin_t);
		sin1 = sin((1.0f - weight) * acos_t);
		sin2 = sin(weight * acos_t);

		out_quaternion[0] = (quaternion_A[0] * sin1 + quaternion_B[0] * sin2) / sin_t;
		out_quaternion[1] = (quaternion_A[1] * sin1 + quaternion_B[1] * sin2) / sin_t;
		out_quaternion[2] = (quaternion_A[2] * sin1 + quaternion_B[2] * sin2) / sin_t;
		out_quaternion[3] = (quaternion_A[3] * sin1 + quaternion_B[3] * sin2) / sin_t;
	}
}

// クォータニオンの正規化
void CQuaternion::NormalizeQuaternion(float * quaternion)
{
	float size;
	CCalc calc;

	size = calc.VectorLength(quaternion, 4);

	quaternion[0] = quaternion[0] / size;
	quaternion[1] = quaternion[1] / size;
	quaternion[2] = quaternion[2] / size;
	quaternion[3] = quaternion[3] / size;
}


// クオータニオンから行列への変換
void CQuaternion::QuaternionToMatrix(float * quaternion, float * matrix)
{
	matrix[0] = 1 - 2 * quaternion[2] * quaternion[2] - 2 * quaternion[3] * quaternion[3];
	matrix[1] = 2 * quaternion[1] * quaternion[2] + 2 * quaternion[3] * quaternion[0];
	matrix[2] = 2 * quaternion[3] * quaternion[1] - 2 * quaternion[0] * quaternion[2];
	matrix[3] = 0.0f;

	matrix[4] = 2 * quaternion[1] * quaternion[2] - 2 * quaternion[3] * quaternion[0];
	matrix[5] = 1 - 2 * quaternion[3] * quaternion[3] - 2 * quaternion[1] * quaternion[1];
	matrix[6] = 2 * quaternion[2] * quaternion[3] + 2 * quaternion[1] * quaternion[0];
	matrix[7] = 0.0f;

	matrix[8] = 2 * quaternion[3] * quaternion[1] + 2 * quaternion[0] * quaternion[2];
	matrix[9] = 2 * quaternion[2] * quaternion[3] - 2 * quaternion[1] * quaternion[0];
	matrix[10] = 1 - 2 * quaternion[1] * quaternion[1] - 2 * quaternion[2] * quaternion[2];
	matrix[11] = 0.0f;

	matrix[12] = 0.0f;
	matrix[13] = 0.0f;
	matrix[14] = 0.0f;
	matrix[15] = 1.0f;
}

