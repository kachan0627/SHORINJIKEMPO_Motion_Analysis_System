#include"RotationMatrix.h"
#include "PositionVector.h"
RotationMatrix::RotationMatrix(){
	//for (int i = 0; i < 3; i++){
	//	original_vector[i] = 0.0;
	//}
}

RotationMatrix::~RotationMatrix(){

}
void RotationMatrix::XRotationMatrix(float a){
	xr_Vector.VM.xa = cos(a); xr_Vector.VM.xb = -sin(a); xr_Vector.VM.xc = 0;
	xr_Vector.VM.ya = sin(a); xr_Vector.VM.yb = cos(a) ; xr_Vector.VM.yc = 0;
	xr_Vector.VM.za = 0		; xr_Vector.VM.zb = 0	   ; xr_Vector.VM.zc = 1;
}
void RotationMatrix::YRotationMatrix(float a){
	yr_Vector.VM.xa = cos(a) ; yr_Vector.VM.xb = 0; yr_Vector.VM.xc = sin(a);
	yr_Vector.VM.ya = 0		 ; yr_Vector.VM.yb = 1; yr_Vector.VM.yc = 0		;
	yr_Vector.VM.za = -sin(a); yr_Vector.VM.zb = 0; yr_Vector.VM.zc = cos(a);
}
void RotationMatrix::ZRotationMatrix(float a){
	zr_Vector.VM.xa = 1; zr_Vector.VM.xb = 0	 ; zr_Vector.VM.xc = 0		;
	zr_Vector.VM.ya = 0; zr_Vector.VM.yb = cos(a); zr_Vector.VM.yc = -sin(a);
	zr_Vector.VM.za = 0; zr_Vector.VM.zb = sin(a); zr_Vector.VM.zc = cos(a) ;
}
void RotationMatrix::rRotationMatrix(float a,float b,float c){
	XRotationMatrix(a);
	YRotationMatrix(b);
	ZRotationMatrix(c);
	r_Vector.MatrixMultiplication(yr_Vector.VM, xr_Vector.VM, zr_Vector.VM);

}
void RotationMatrix::CreateRV(PositionVector a, PositionVector b){

	MatrixMultiplication(a, b);
}

void RotationMatrix::MatrixMultiplication(PositionVector a, PositionVector b){
	//‚Ps‚R—ñ–‚Rs‚R—ñ
	PositionVector d;
	d.V.x = a.V.x*b.VM.xa + a.V.y*b.VM.ya + a.V.z*b.VM.za;	d.V.y = a.V.x*b.VM.xb + a.V.y*b.VM.yb + a.V.z*b.VM.zb;	 d.V.z = a.V.x*b.VM.xc + a.V.y*b.VM.yc + a.V.z*b.VM.zc;
	r_Vector.CreatePV(d.V.x, d.V.y, d.V.z);
}



