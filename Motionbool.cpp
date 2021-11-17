#include <math.h>
#include "Motionbool.h"

Motionbool::Motionbool(){
	mb.a = false;
	mb.b = false;
	mb.c = false;
	mb.d = false;
	mb.e = false;
}
Motionbool::~Motionbool(){
}
void Motionbool::spinetrue(){
	mb.a = true;
}
void Motionbool::leftarmtrue(){
	mb.b = true;
}
void Motionbool::rightarmtrue(){
	mb.c = true;
}
void Motionbool::leftlegtrue(){
	mb.d = true;
}
void Motionbool::rightlegtrue(){
	mb.e = true;
}

void Motionbool::allfalse(){
	mb.a = false;
	mb.b = false;
	mb.c = false;
	mb.d = false;
	mb.e = false;
}

bool Motionbool::check5joint_bool(){
	if (mb.a == true && mb.b == true && mb.c == true && mb.d == true && mb.e == true){
		return true;
	}
	else{
		return false;
	}

}
