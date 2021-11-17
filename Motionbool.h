#ifndef __MOTIONBOOL_H
#define __MOTIONBOOL_H
class Motionbool{
public:
	struct MOTIONBOOL {
		bool a, b, c, d, e;
	};
	
	MOTIONBOOL mb;
	Motionbool();
	~Motionbool();
	void spinetrue();
	void leftarmtrue();
	void rightarmtrue();
	void leftlegtrue();
	void rightlegtrue();
	void allfalse();
	bool check5joint_bool();
};
#endif