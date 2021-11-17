/**
***  �L�����N�^�A�j���[�V�����̂��߂̐l�̃��f���̕\���E��{���� ���C�u�����E�T���v���v���O����
***  Copyright (c) 2015-, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http://opensource.org/licenses/mit-license.php
**/

/**
***  GLUT�t���[�����[�N �{ �A�v���P�[�V�������N���X
**/


#ifndef  SIMPLE_HUMAN_GLUT
#define  SIMPLE_HUMAN_GLUT
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600


// Windows�֐���`�̓ǂݍ���
#ifdef  WIN32
#include <windows.h>
#endif



// GLUT ���g�p
#include <GL/glut.h>

#include "resource.h"
//
//  �s��E�x�N�g���̕\���ɂ� vecmath C++���C�u�����ihttp://objectclub.jp/download/vecmath1�j���g�p
//
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color3.h>
#include <Color4.h>

#include <tchar.h>

// STL ���g�p
#include <vector>
#include <string>
using namespace std;



///////////////////////////////////////////////////////////////////////////////
//
//  �A�v���P�[�V�������N���X
//


//
//  �A�v���P�[�V�������N���X
//
class  GLUTBaseApp
{
protected:
	// �A�v���P�[�V�������

	// �A�v���P�[�V������
	string  app_name;

protected:
	// ���_����̂��߂̕ϐ�

	// ���_�̕��ʊp
	float   camera_yaw;

	// ���_�̋p
	float   camera_pitch;

	// ���_�ƒ����_�̋���
	float   camera_distance;

	// �����_�̈ʒu
	Point3f   view_center;

protected:
	// �}�E�X���͏����̂��߂̕ϐ�

	// �E�E���E���{�^�����h���b�O�����ǂ����̃t���O
	bool    drag_mouse_r;
	bool    drag_mouse_l;
	bool    drag_mouse_m;

	// �Ō�ɋL�^���ꂽ�}�E�X�J�[�\���̍��W
	int    last_mouse_x;
	int    last_mouse_y;

protected:
	// ��ʕ`��Ɋւ���ϐ�

	// �����ʒu
	Point4f   light_pos;

	// �e�̕����E�F
	Vector3f  shadow_dir;
	Color4f   shadow_color;

protected:
	// �A�v���P�[�V������Ԃ̕ϐ�

	// �E�B���h�E�̃T�C�Y
	int     win_width;
	int     win_height;

	// �������t���O
	bool    is_initialized;

	// ���_�X�V�t���O
	bool    is_view_updated;
	
	


public:
	// �R���X�g���N�^
	GLUTBaseApp();

	// �f�X�g���N�^
	virtual ~GLUTBaseApp() {}

public:
	// �A�N�Z�T
	const string &  GetAppName() { return  app_name; }
	int  GetWindowWidth() { return  win_width; }
	int  GetWindowHeight() { return  win_height; }
	bool  IsInitialized() { return  is_initialized; }

public:
	// �C�x���g�����C���^�[�t�F�[�X

	//  ������
	virtual void  Initialize();

	//  �J�n�E���Z�b�g
	virtual void  Start();

	//  ��ʕ`��
	virtual void  Display();
	virtual void  Display2();
	virtual void  Display3();
	virtual void  Display4();
	// �E�B���h�E�T�C�Y�ύX
	virtual void  Reshape(int w, int h);
	virtual void  Reshape2(int w, int h);
	virtual void  Reshape3(int w, int h);
	virtual void  Reshape4(int w, int h);
	// �}�E�X�N���b�N
	virtual void  MouseClick(int button, int state, int mx, int my);
	virtual void  MouseClick2(int button, int state, int mx, int my);
	virtual void  MouseClick3(int button, int state, int mx, int my);
	virtual void  MouseClick4(int button, int state, int mx, int my);
	// �}�E�X�h���b�O
	virtual void  MouseDrag(int mx, int my);
	virtual void  MouseDrag2(int mx, int my);
	virtual void  MouseDrag3(int mx, int my);
	virtual void  MouseDrag4(int mx, int my);
	// �}�E�X�ړ�
	virtual void  MouseMotion(int mx, int my);
	virtual void  MouseMotion2(int mx, int my);
	virtual void  MouseMotion3(int mx, int my);
	virtual void  MouseMotion4(int mx, int my);
	// �L�[�{�[�h�̃L�[����
	virtual void  Keyboard(unsigned char key, int mx, int my);
	virtual void  Keyboard2(unsigned char key, int mx, int my);
	virtual void  Keyboard3(unsigned char key, int mx, int my);
	virtual void  Keyboard4(unsigned char key, int mx, int my);
	// �L�[�{�[�h�̓���L�[����
	virtual void  KeyboardSpecial(unsigned char key, int mx, int my);

	// �A�j���[�V��������
	virtual void  Animation(float delta);

protected:
	// �⏕����

	// �i�q�͗l�̏���`��
	void  DrawFloor(float tile_size, int num_x, int num_z, float r0, float g0, float b0, float r1, float g1, float b1);

	// ��������`��
	void  DrawTextInformation(int line_no, const char * message);
};



///////////////////////////////////////////////////////////////////////////////
//
//  GLUT�t���[�����[�N�̃��C���֐�
//

//int  WinId2[2];
//
//  GLUT�t���[�����[�N�̃��C���֐��i���s����A�v���P�[�V�����̃��X�g���w��j
//
int  SimpleHumanGLUTMain(const vector< class GLUTBaseApp * > & app_lists, int argc, char ** argv, const char * win_title = NULL, const char * win_title2 = NULL, const char * win_title3 = NULL, const char * win_title4 = NULL, int win_width = 0, int win_height = 0, int*  Winid = NULL);
int  SimpleHumanGLUTMain2(const vector< class GLUTBaseApp * > & app_lists, int argc, char ** argv, const char * win_title = NULL, int win_width = 0, int win_height = 0);

//
//  GLUT�t���[�����[�N�̃��C���֐��i���s�����̃A�v���P�[�V�������w��j
//
int  SimpleHumanGLUTMain(class GLUTBaseApp * app, int argc, char ** argv, const char * win_title = NULL, int win_width = 0, int win_height = 0, int*  Winid = NULL);

#endif // SIMPLE_HUMAN_GLUT
