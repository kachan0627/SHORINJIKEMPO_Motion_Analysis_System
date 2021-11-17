//#include "StdAfx.h"
#include <GL/glut.h>
#include ".\calc.h"
#include "direct.h"

CCalc::CCalc(void)
{
	for (int i = 0; i < 3; i++){
		original_vector[i] = 0.0;
	}
}

CCalc::~CCalc(void)
{
}

// 子関節への回転角度を求める（Z軸回転→X軸回転）
void CCalc::GetJointRotateAngle(float  child_vector[3], float  rotate_angle[3])
{
	//親の座標系のY軸を子ベクトルにあわせる//

	float axis_vector[3];	//回転軸
	float y_vector[3];	//親のy軸を表すベクトル
	float target_vector[3];	//あわせる子ベクトル
	float outer_vector[3];	//外積
	float Zrotate_sin, Zrotate_cos;	//ｚ軸の回転角度のCosとSin

	rotate_angle[1] = 0.0;	//ｙ軸の回転角度はなし

	///////////Z軸回転の回転角度を求める（子ベクトルがZ-Y平面にくるように回転）/////////////////////
	//回転軸（Z軸）
	axis_vector[0] = 0.0;
	axis_vector[1] = 0.0;
	axis_vector[2] = 1.0;

	//親の座標系のｙ軸
	y_vector[0] = 0.0;
	y_vector[1] = 1.0;
	y_vector[2] = 0.0;

	//x-y平面に射影した子ベクトル
	target_vector[0] = child_vector[0];
	target_vector[1] = child_vector[1];
	target_vector[2] = 0.0;

	//回転軸と，子ベクトルの外積
	OuterProduct(axis_vector, child_vector, outer_vector);
	if (outer_vector[0] != 0.0 || outer_vector[1] != 0.0 || outer_vector[2] != 0.0){	//ｚ軸に回転角度が０でない
		rotate_angle[2] = GetAngle(y_vector, target_vector, axis_vector, &Zrotate_sin, &Zrotate_cos);
	}
	else{	//ｚ軸の回転角度０度
		rotate_angle[2] = 0.0;
		Zrotate_sin = 0.0;
		Zrotate_cos = 1.0;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////X軸回転の回転角度を求める（子ベクトルが回転後の親のYベクトルにくるように回転）//////////
	//回転軸（回転後のX軸）
	axis_vector[0] = Zrotate_cos;
	axis_vector[1] = Zrotate_sin;
	axis_vector[2] = 0.0;

	//回転後の親座標系のY軸
	y_vector[0] = Zrotate_sin * -1;
	y_vector[1] = Zrotate_cos;
	y_vector[2] = 0.0;

	//子ベクトル
	target_vector[0] = child_vector[0];
	target_vector[1] = child_vector[1];
	target_vector[2] = child_vector[2];

	//回転軸と，子ベクトルの外積
	OuterProduct(axis_vector, child_vector, outer_vector);
	if (outer_vector[0] != 0.0 || outer_vector[1] != 0.0 || outer_vector[2] != 0.0){	//ｚ軸に回転角度が０でない
		rotate_angle[0] = GetAngle(y_vector, target_vector, axis_vector, &Zrotate_sin, &Zrotate_cos);
	}
	else{	//ｚ軸の回転角度０度
		rotate_angle[0] = 0.0;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
}

// 外積を求める
void CCalc::OuterProduct(float  va[3], float  vb[3], float  OP[3])
{
	OP[0] = va[1] * vb[2] - va[2] * vb[1];
	OP[1] = va[2] * vb[0] - va[0] * vb[2];
	OP[2] = va[0] * vb[1] - va[1] * vb[0];
}

// ２線分のなす角度を求める
float CCalc::GetAngle(float  va[3], float  vb[3], float  axis[3], float * sin, float * cos)
{
	float angle = 0.0;	//角度
	float length_a = 0.0, length_b = 0.0;	//va,vbの長さ
	float op[3];	//外積
	float length_op = 0.0;	//外積a*bの大きさ
	float ip;	//内積
	float cos_angle = 0.0;	//cos
	float sin_angle = 0.0;	//sin

	length_a = GetDistance(original_vector, va);
	length_b = GetDistance(original_vector, vb);

	ip = InnerProduct(va, vb);
	OuterProduct(va, vb, op);
	length_op = GetDistance(original_vector, op);

	//cosを求める
	cos_angle = ip / (length_a * length_b);
	//誤差の考慮(cos)
	if (cos_angle > 1) {
		cos_angle = 1.0;
	}
	else if (cos_angle < -1) {
		cos_angle = -1.0;
	}
	*cos = cos_angle;

	//sinを求める
	sin_angle = length_op / (length_a * length_b);
	//誤差の考慮(sin)
	if (sin_angle > 1) {
		sin_angle = 1.0;
	}

	//angleを求める
	angle = acos(cos_angle);
	//弧度法→度数法に変換
	angle = (float)(angle * 180 / PI);

	//回転の向きを求める
	ip = InnerProduct(axis, op);	//回転軸と外積で内積を求める
	if (ip < 0){	//内積が負の場合
		angle = angle * -1;
		sin_angle = sin_angle * -1;
	}

	*sin = sin_angle;

	return angle;
}

// 2点間の距離を求める
float CCalc::GetDistance(float  va[3], float  vb[3])
{
	return sqrt(pow(va[0] - vb[0], 2) + pow(va[1] - vb[1], 2) + pow(va[2] - vb[2], 2));
}

// 内積を求める
float CCalc::InnerProduct(float  va[3], float  vb[3])
{
	return (va[0] * vb[0]) + (va[1] * vb[1]) + (va[2] * vb[2]);
}

// 逆行列を求める
void CCalc::GetInverseMatrix(float* matrix, float* i_matrix)
{
	//同次行列の逆行列の計算
	float translate_vector[3], result_vector[3];

	//回転成分の転置
	i_matrix[0] = matrix[0];		i_matrix[4] = matrix[1];		i_matrix[8] = matrix[2];
	i_matrix[1] = matrix[4];		i_matrix[5] = matrix[5];		i_matrix[9] = matrix[6];
	i_matrix[2] = matrix[8];		i_matrix[6] = matrix[9];		i_matrix[10] = matrix[10];
	i_matrix[3] = matrix[3];		i_matrix[7] = matrix[7];		i_matrix[11] = matrix[11];

	translate_vector[0] = matrix[12];
	translate_vector[1] = matrix[13];
	translate_vector[2] = matrix[14];

	result_vector[0] = i_matrix[0] * translate_vector[0] + i_matrix[4] * translate_vector[1] + i_matrix[8] * translate_vector[2];
	result_vector[1] = i_matrix[1] * translate_vector[0] + i_matrix[5] * translate_vector[1] + i_matrix[9] * translate_vector[2];
	result_vector[2] = i_matrix[2] * translate_vector[0] + i_matrix[6] * translate_vector[1] + i_matrix[10] * translate_vector[2];

	i_matrix[12] = -result_vector[0];
	i_matrix[13] = -result_vector[1];
	i_matrix[14] = -result_vector[2];
	i_matrix[15] = matrix[15];

	//int n = 16; //逆行列の大きさ　n行×n列行列
	//int i, j, k;
	//double temp;

	////単位行列で初期化
	//for (j = 0; j < n; j++){
	//	for (i = 0; i < n; i++){
	//		if (i == j){
	//			i_matrix[i + j * n] = 1;
	//		}else{
	//			i_matrix[i + j * n] = 0;
	//		}
	//	}
	//}

	//for (k = 0; k < n; k++){
	//	//k行k列目の要素を１にする
	//	temp = matrix[k + k * n];
	//	//if (temp == 0) return -1;    //エラー
	//	for (i = 0; i < n; i++){
	//		matrix[i + k * n] /= temp;
	//		i_matrix[i + k * n] /= temp;
	//	}

	//	for (j = 0; j < n; j++){
	//		if (j != k){
	//			temp = matrix[k + j * n] / matrix[k + k * n];
	//			for (i = 0; i < n; i++){
	//				matrix[i + j * n] -= matrix[i + k * n] * temp;
	//				i_matrix[i + j * n] -= i_matrix[i + k * n] * temp;
	//			}            
	//		}
	//	}
	//}
}

//ベクトルと行列の掛け算///////////////////////////////////
void CCalc::MultVectorMatrix(float* input_v, float* output_v, float* matrix)
{
	output_v[0] = matrix[0] * input_v[0] + matrix[4] * input_v[1] + matrix[8] * input_v[2] + matrix[12];
	output_v[1] = matrix[1] * input_v[0] + matrix[5] * input_v[1] + matrix[9] * input_v[2] + matrix[13];
	output_v[2] = matrix[2] * input_v[0] + matrix[6] * input_v[1] + matrix[10] * input_v[2] + matrix[14];

}

// 行列×行列 (4×4の正方行列)
void CCalc::MultMatrix(float* matrix_a, float* matrix_b, float* matrix_out)
{
#if(0)

	matrix_out[0] = matrix_a[0] * matrix_b[0] + matrix_a[4] * matrix_b[1] + matrix_a[8] * matrix_b[2] + matrix_a[12] * matrix_b[3];
	matrix_out[1] = matrix_a[1] * matrix_b[0] + matrix_a[5] * matrix_b[1] + matrix_a[9] * matrix_b[2] + matrix_a[13] * matrix_b[3];
	matrix_out[2] = matrix_a[2] * matrix_b[0] + matrix_a[6] * matrix_b[1] + matrix_a[10] * matrix_b[2] + matrix_a[14] * matrix_b[3];
	matrix_out[3] = matrix_a[3] * matrix_b[0] + matrix_a[7] * matrix_b[1] + matrix_a[11] * matrix_b[2] + matrix_a[15] * matrix_b[3];

	matrix_out[4] = matrix_a[0] * matrix_b[4] + matrix_a[4] * matrix_b[5] + matrix_a[8] * matrix_b[6] + matrix_a[12] * matrix_b[7];
	matrix_out[5] = matrix_a[1] * matrix_b[4] + matrix_a[5] * matrix_b[5] + matrix_a[9] * matrix_b[6] + matrix_a[13] * matrix_b[7];
	matrix_out[6] = matrix_a[2] * matrix_b[4] + matrix_a[6] * matrix_b[5] + matrix_a[10] * matrix_b[6] + matrix_a[14] * matrix_b[7];
	matrix_out[7] = matrix_a[3] * matrix_b[4] + matrix_a[7] * matrix_b[5] + matrix_a[11] * matrix_b[6] + matrix_a[15] * matrix_b[7];

	matrix_out[8] = matrix_a[0] * matrix_b[8] + matrix_a[4] * matrix_b[9] + matrix_a[8] * matrix_b[10] + matrix_a[12] * matrix_b[11];
	matrix_out[9] = matrix_a[1] * matrix_b[8] + matrix_a[5] * matrix_b[9] + matrix_a[9] * matrix_b[10] + matrix_a[13] * matrix_b[11];
	matrix_out[10] = matrix_a[2] * matrix_b[8] + matrix_a[6] * matrix_b[9] + matrix_a[10] * matrix_b[10] + matrix_a[14] * matrix_b[11];
	matrix_out[11] = matrix_a[3] * matrix_b[8] + matrix_a[7] * matrix_b[9] + matrix_a[11] * matrix_b[10] + matrix_a[15] * matrix_b[11];

	matrix_out[12] = matrix_a[0] * matrix_b[12] + matrix_a[4] * matrix_b[13] + matrix_a[8] * matrix_b[14] + matrix_a[12] * matrix_b[15];
	matrix_out[13] = matrix_a[1] * matrix_b[12] + matrix_a[5] * matrix_b[13] + matrix_a[9] * matrix_b[14] + matrix_a[13] * matrix_b[15];
	matrix_out[14] = matrix_a[2] * matrix_b[12] + matrix_a[6] * matrix_b[13] + matrix_a[10] * matrix_b[14] + matrix_a[14] * matrix_b[15];
	matrix_out[15] = matrix_a[3] * matrix_b[12] + matrix_a[7] * matrix_b[13] + matrix_a[11] * matrix_b[14] + matrix_a[15] * matrix_b[15];

#else

	int i, j, k;
	float tmp;

	for (i = 0; i<16; i += 4){
		for (j = 0; j<4; j++){
			tmp = 0.0;
			for (k = 0; k<4; k++){
				tmp += matrix_a[j + k * 4] * matrix_b[k + i];
			}
			matrix_out[i + j] = tmp;
		}
	}

#endif
}


// 回転量を求める(a*b)
float CCalc::GetRotateAngle(float* va, float* vb)
{
	float angle = 0.0;	//角度
	float length_a = 0.0, length_b = 0.0;	//va,vbの長さ
	float ip;	//内積
	float cos_angle = 0.0;	//cos

	length_a = GetDistance(original_vector, va);
	length_b = GetDistance(original_vector, vb);
	ip = InnerProduct(va, vb);

	//cosを求める
	cos_angle = ip / (length_a * length_b);
	//誤差の考慮(cos)
	if (cos_angle > 1) {
		cos_angle = 1.0;
	}
	else if (cos_angle < -1) {
		cos_angle = -1.0;
	}
	//angleを求める
	angle = acos(cos_angle);
	//度数法に変換(デバッグ用)
	//float test_angle = (float)(angle * 180 / PI);

	return angle;
}

// ベクトルを正規化する
void CCalc::NormalizeVector(float *vector)
{
	float length = 0.0;	//ベクトルの長さ

	length = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);

	if (length != 0.0){
		for (int i = 0; i < 3; i++){
			vector[i] = vector[i] / length;
		}
	}
	///////デバッグ用/////////
	//length = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
}

//cosθを返す
float CCalc::GetCosAngle(float* va, float* vb)
{
	float cos_angle = 0.0;	//cos
	float length_a = 0.0, length_b = 0.0;	//va,vbの長さ
	float ip;	//内積

	length_a = GetDistance(original_vector, va);
	length_b = GetDistance(original_vector, vb);
	ip = InnerProduct(va, vb);

	//cosを求める
	cos_angle = ip / (length_a * length_b);
	//誤差の考慮(cos)
	if (cos_angle > 1) {
		cos_angle = 1.0;
	}
	else if (cos_angle < -1) {
		cos_angle = -1.0;
	}

	return cos_angle;
}

//立方根の計算(2010/01/08)////////////////////////////////////////////////////////////////////////
float CCalc::CalcCbRoot(float x)
{
	double stv;
	double cbr;

	stv = GetStartValue((double)x); /* 立方根の初期値を求める */
	cbr = GetCbRoot((double)x, stv); /* 立方根を求める */

	return (float)cbr;
}

//立方根の初期値を求める
double CCalc::GetStartValue(double x)
{
	if (x >= 0)/* x が正なら平方根を返す */
		return(sqrt(x));
	if (x < 0)/* x が負なら絶対値の平方根 */
		return(-sqrt(-x));/* に-を付けて返す */
	return 0;
}

//立方根を求める
double CCalc::GetCbRoot(double x, double stv)
{
	double dx = stv / 10;

	if (x == 0.0) /* x が０なら平方根も０ */
		return (0.0);

	if (x < stv * stv * stv)
		dx *= -1.0;

	while (1) { /* ２重の無限ループ */
		while (1) {
			if ((dx > 0 && (x < stv * stv * stv)) || (dx < 0 && (x > stv * stv * stv)))
				/* 立方根と近似値の大小関係が変化したら */
				break; /* 内側の無限ループから抜ける */
			else
				stv += dx;
		}
		if (fabs(dx) < 0.00000001)
			/* 小数点以下８桁まで精度が出たら */
			break; /* 外側の無限ループから抜ける */
		else /* まだならここ */
			dx *= -0.1;
	}
	return (stv);
}
/////////////////////////////////////////////////////////////////////////////////////////////

//行列式の算出(3次：サラスの方法)(2010/01/21)
float CCalc::CalcSarrus(float *matrix)
{
	float det = 0.0f;

	det = matrix[0] * matrix[4] * matrix[8];
	det += matrix[3] * matrix[7] * matrix[2];
	det += matrix[6] * matrix[1] * matrix[5];
	det -= matrix[0] * matrix[7] * matrix[5];
	det -= matrix[3] * matrix[1] * matrix[8];
	det -= matrix[6] * matrix[4] * matrix[2];

	return det;
}

// 2線分のなす角を求める(その２)(2010/03/16)
float CCalc::GetAngle2(float va[3], float vb[3])
{
	float angle = 0.0;	//角度
	float length_a = 0.0, length_b = 0.0;	//va,vbの長さ
	float op[3];	//外積
	float length_op = 0.0;	//外積a*bの大きさ
	float ip;	//内積
	float cos_angle = 0.0;	//cos

	float sin_angle = 0.0;	//sin

	length_a = GetDistance(original_vector, va);
	length_b = GetDistance(original_vector, vb);

	ip = InnerProduct(va, vb);
	OuterProduct(va, vb, op);
	length_op = GetDistance(original_vector, op);

	//cosを求める
	cos_angle = ip / (length_a * length_b);
	//誤差の考慮(cos)
	if (cos_angle > 1) {
		cos_angle = 1.0;
	}
	else if (cos_angle < -1) {
		cos_angle = -1.0;
	}

	//angleを求める
	angle = acos(cos_angle);
	//弧度法→度数法に変換
	angle = (float)(angle * 180 / PI);

	return angle;
}

// RGBをHSVに変換
void CCalc::RGB2HSV(double* H, double* S, double* V, double R, double G, double B)
{
	double Z;
	double r, g, b;
	*V = max_color(R, G, B);
	Z = min_color(R, G, B);
	if (*V != 0.0)
		*S = (*V - Z) / *V;
	else
		*S = 0.0;
	if ((*V - Z) != 0){
		r = (*V - R) / (*V - Z);
		g = (*V - G) / (*V - Z);
		b = (*V - B) / (*V - Z);
	}
	else{
		r = 0;
		g = 0;
		b = 0;
	}
	if (*V == R)
		*H = 60 * (b - g); // 60 = PI/3
	else if (*V == G)
		*H = 60 * (2 + r - b);
	else
		*H = 60 * (4 + g - r);
	if (*H < 0.0)
		*H = *H + 360;
}

double CCalc::max_color(double r, double g, double b)
{
	double ret;
	if (r > g){
		if (r > b)
			ret = r;
		else
			ret = b;
	}
	else{
		if (g > b)
			ret = g;
		else
			ret = b;
	}
	return ret;
}

double CCalc::min_color(double r, double g, double b)
{
	double ret;
	if (r < g){
		if (r < b)
			ret = r;
		else
			ret = b;
	}
	else{
		if (g < b)
			ret = g;
		else
			ret = b;
	}
	return ret;
}

// HSVをRGBに変換
void CCalc::HSV2RGB(float* rr, float* gg, float* bb, float H, float S, float V)
{
	int in;
	float fl;
	float m, n;
	in = (int)floor(H / 60);
	fl = (H / 60) - in;
	if (!(in & 1)) fl = 1 - fl; // if i is even

	m = V * (1 - S);
	n = V * (1 - S * fl);
	switch (in){
	case 0: *rr = V; *gg = n; *bb = m; break;
	case 1: *rr = n; *gg = V; *bb = m; break;
	case 2: *rr = m; *gg = V; *bb = n; break;
	case 3: *rr = m; *gg = n; *bb = V; break;
	case 4: *rr = n; *gg = m; *bb = V; break;
	case 5: *rr = V; *gg = m; *bb = n; break;
	}
}

// ベクトルの長さを求めて返す(多次元に対応)
float CCalc::VectorLength(float * vector, int num)
{
	int i;
	float length = 0.0;

	for (i = 0; i<num; i++)
		length += vector[i] * vector[i];

	length = sqrt(length);

	return length;
}

// 法線の算出
void CCalc::CalcNormalVector(float* xyz1, float* xyz2, float* xyz3, float* norm)
{
	float vec1[3], vec2[3];
	int i;
	float normal;

	for (i = 0; i<3; i++){
		vec1[i] = xyz1[i] - xyz3[i];
		vec2[i] = xyz2[i] - xyz3[i];
	}

	norm[0] = (vec1[1] * vec2[2]) - (vec1[2] * vec2[1]);
	norm[1] = (vec1[2] * vec2[0]) - (vec1[0] * vec2[2]);
	norm[2] = (vec1[0] * vec2[1]) - (vec1[1] * vec2[0]);

	normal = sqrt((norm[0] * norm[0]) + (norm[1] * norm[1]) + (norm[2] * norm[2]));

	for (i = 0; i<3; i++)
		norm[i] /= normal;
}
