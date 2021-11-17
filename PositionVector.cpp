#include "PositionVector.h"
#include "math.h"
#include "stdlib.h"

PositionVector::PositionVector()
{
}


PositionVector::~PositionVector()
{
}

void PositionVector::CreatePV(float x, float y, float z){
	
	V.x = x;
	V.y = y;
	V.z = z;
	CreateVM(V);
}
void PositionVector::CreateVM(Vector a){
	
	CreateVMx(a.x);
	CreateVMy(a.y);
	CreateVMz(a.z);
}

void PositionVector::CreateVMx(float x){
	float e_x[3] = { 1.0, 0.0, 0.0 };//xの単位ベクトル
	VM.xa = e_x[0] * x;
	VM.xb = e_x[1] * x;
	VM.xc = e_x[2] * x;
}
void PositionVector::CreateVMy(float y){
	float e_y[3] = { 0.0, 1.0, 0.0 };//yの単位ベクトル
	VM.ya = e_y[0] * y;
	VM.yb = e_y[1] * y;
	VM.yc = e_y[2] * y;
}
void PositionVector::CreateVMz(float z){
	float e_z[3] = { 0.0, 0.0, 1.0 };//zの単位ベクトル
	VM.za = e_z[0] * z;
	VM.zb = e_z[1] * z;
	VM.zc = e_z[2] * z;
}



void PositionVector::MatrixMultiplication(VectorMatrix a, VectorMatrix b,VectorMatrix c){

	VectorMatrix d, e;
	d.xa = b.xa*c.xa + b.xb*c.ya + b.xc*c.za;	 d.xb = b.xa*c.xb + b.xb*c.yb + b.xc*c.zb;		d.xc = b.xa*c.xc + b.xb*c.yc + b.xc*c.zc;
	d.ya = b.ya*c.xa + b.yb*c.ya + b.yc*c.za;	 d.yb = b.ya*c.xb + b.yb*c.yb + b.yc*c.zb;		d.yc = b.ya*c.xc + b.yb*c.yc + b.yc*c.zc;
	d.za = b.za*c.xa + b.zb*c.ya + b.zc*c.za;	 d.zb = b.za*c.xb + b.zb*c.yb + b.zc*c.zb;		d.zc = b.za*c.xc + b.zb*c.yc + b.zc*c.zc;

	e.xa = a.xa*d.xa + a.xb*d.ya + a.xc*d.za;	 e.xb = a.xa*d.xb + a.xb*d.yb + a.xc*d.zb;		e.xc = a.xa*d.xc + a.xb*d.yc + a.xc*d.zc;
	e.ya = a.ya*d.xa + a.yb*d.ya + a.yc*d.za;	 e.yb = a.ya*d.xb + a.yb*d.yb + a.yc*d.zb;		e.yc = a.ya*d.xc + a.yb*d.yc + a.yc*d.zc;
	e.za = a.za*d.xa + a.zb*d.ya + a.zc*d.za;	 e.zb = a.za*d.xb + a.zb*d.yb + a.zc*d.zb;		e.zc = a.za*d.xc + a.zb*d.yc + a.zc*d.zc;
	VM = e;
}

