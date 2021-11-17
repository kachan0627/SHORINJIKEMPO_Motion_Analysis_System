


// �w�b�_�t�@�C���̃C���N���[�h
#include "SimpleHumanGLUT.h"


///////////////////////////////////////////////////////////////////////////////
//
//  �����A�v���P�[�V�����̊Ǘ��E�ؑւ̂��߂̕ϐ��E�֐�
//

// ���ݎ��s���̃A�v���P�[�V����
class GLUTBaseApp *    app = NULL;

// �S�A�v���P�[�V�����̃��X�g
vector< class GLUTBaseApp * >    applications;

// �\�t�g�E�F�A����
const char *    software_description = "ShorinjiKempo_Training_System\nSumiya Katsumi";

// ���s�A�v���P�[�V�����̐ؑ֊֐��i�v���g�^�C�v�錾�j
void  ChangeApp(int app_no);

int  WinID[4];

///////////////////////////////////////////////////////////////////////////////
//
//  �A�v���P�[�V�������N���X
//


//
//  �R���X�g���N�^
//
GLUTBaseApp::GLUTBaseApp()
{
	app_name = "Unknown";

	win_width = 0;
	win_height = 0;
	is_initialized = false;
	is_view_updated = true;
}


//
//   ������
//
void  GLUTBaseApp::Initialize()
{
	is_initialized = true;

	camera_yaw = 0.0f;
	camera_pitch = -20.0f;
	camera_distance = 8.0f;
	view_center.set(0.0f, 0.0f, 0.0f);

	drag_mouse_r = false;
	drag_mouse_l = false;
	drag_mouse_m = false;
	last_mouse_x = 0;
	last_mouse_y = 0;

	light_pos.set(0.0f, 10.0f, 0.0f, 1.0f);
	shadow_dir.set(0.0f, 1.0f, 0.0f);
	shadow_color.set(0.2f, 0.2f, 0.2f, 0.5f);
}


//
//   �J�n�E���Z�b�g
//
void  GLUTBaseApp::Start()
{
}


//
//  ��ʕ`��
//
void  GLUTBaseApp::Display()
{

	// ��ʂ��N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// �ϊ��s���ݒ�i���f�����W�n���J�������W�n�j
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -camera_distance);
	glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
	glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
	glTranslatef(-view_center.x, -0.5, -view_center.z);

	// �����ʒu���Đݒ�
	float  light0_position[] = { light_pos.x, light_pos.y, light_pos.z, light_pos.w };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// �i�q�͗l�̏���`��
	DrawFloor(1.0f, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f, 0.8f, 0.8f);
}

void  GLUTBaseApp::Display2()
{

	// ��ʂ��N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// �ϊ��s���ݒ�i���f�����W�n���J�������W�n�j
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -camera_distance);
	glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
	glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
	glTranslatef(-view_center.x, -0.5, -view_center.z);

	// �����ʒu���Đݒ�
	float  light0_position[] = { light_pos.x, light_pos.y, light_pos.z, light_pos.w };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// �i�q�͗l�̏���`��
	DrawFloor(1.0f, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f, 0.8f, 0.8f);
}
void  GLUTBaseApp::Display3()
{

	// ��ʂ��N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// �ϊ��s���ݒ�i���f�����W�n���J�������W�n�j
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -camera_distance);
	glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
	glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
	glTranslatef(-view_center.x, -0.5, -view_center.z);

	// �����ʒu���Đݒ�
	float  light0_position[] = { light_pos.x, light_pos.y, light_pos.z, light_pos.w };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// �i�q�͗l�̏���`��
	DrawFloor(1.0f, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f, 0.8f, 0.8f);
}
void  GLUTBaseApp::Display4()
{

	// ��ʂ��N���A
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// �ϊ��s���ݒ�i���f�����W�n���J�������W�n�j
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -camera_distance);
	glRotatef(-camera_pitch, 1.0, 0.0, 0.0);
	glRotatef(-camera_yaw, 0.0, 1.0, 0.0);
	glTranslatef(-view_center.x, -0.5, -view_center.z);

	// �����ʒu���Đݒ�
	float  light0_position[] = { light_pos.x, light_pos.y, light_pos.z, light_pos.w };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	// �i�q�͗l�̏���`��
	DrawFloor(1.0f, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f, 0.8f, 0.8f);
}
//
//  �E�B���h�E�T�C�Y�ύX
//
void  GLUTBaseApp::Reshape(int w, int h)
{
	// �E�B���h�E�̃T�C�Y���L�^
	win_width = w;
	win_height = h;

	// ���_�̍X�V�t���O��ݒ�
	is_view_updated = true;
}
void  GLUTBaseApp::Reshape2(int w, int h)
{
	// �E�B���h�E�̃T�C�Y���L�^
	win_width = w;
	win_height = h;

	// ���_�̍X�V�t���O��ݒ�
	is_view_updated = true;
}
void  GLUTBaseApp::Reshape3(int w, int h)
{
	// �E�B���h�E�̃T�C�Y���L�^
	win_width = w;
	win_height = h;

	// ���_�̍X�V�t���O��ݒ�
	is_view_updated = true;
}

void  GLUTBaseApp::Reshape4(int w, int h)
{
	// �E�B���h�E�̃T�C�Y���L�^
	win_width = w;
	win_height = h;

	// ���_�̍X�V�t���O��ݒ�
	is_view_updated = true;
}

//
//  �}�E�X�N���b�N
//
void  GLUTBaseApp::MouseClick(int button, int state, int mx, int my)
{
	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_l = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		drag_mouse_l = false;

	// �E�{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_r = true;
	// �E�{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		drag_mouse_r = false;

	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_m = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
		drag_mouse_m = false;

	// ���݂̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseClick2(int button, int state, int mx, int my)
{
	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_l = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		drag_mouse_l = false;

	// �E�{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_r = true;
	// �E�{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		drag_mouse_r = false;

	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_m = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
		drag_mouse_m = false;

	// ���݂̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseClick3(int button, int state, int mx, int my)
{
	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_l = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		drag_mouse_l = false;

	// �E�{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_r = true;
	// �E�{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		drag_mouse_r = false;

	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_m = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
		drag_mouse_m = false;

	// ���݂̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseClick4(int button, int state, int mx, int my)
{
	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_l = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		drag_mouse_l = false;

	// �E�{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_r = true;
	// �E�{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
		drag_mouse_r = false;

	// ���{�^���������ꂽ��h���b�O�J�n
	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
		drag_mouse_m = true;
	// ���{�^���������ꂽ��h���b�O�I��
	else if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP))
		drag_mouse_m = false;

	// ���݂̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
//
//   �}�E�X�h���b�O
//
void  GLUTBaseApp::MouseDrag(int mx, int my)
{
	// SHIFT�L�[�̉�����Ԃ��擾
	int  mod = glutGetModifiers();

	// �E�{�^���̃h���b�O���͎��_����]����
	//	if ( drag_mouse_r )
	if (drag_mouse_r && !(mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̉��ړ��ɉ����Ăx���𒆐S�ɉ�]
		camera_yaw -= (mx - last_mouse_x) * 1.0;
		if (camera_yaw < 0.0)
			camera_yaw += 360.0;
		else if (camera_yaw > 360.0)
			camera_yaw -= 360.0;

		// �}�E�X�̏c�ړ��ɉ����Ăw���𒆐S�ɉ�]
		camera_pitch -= (my - last_mouse_y) * 1.0;
		if (camera_pitch < -90.0)
			camera_pitch = -90.0;
		else if (camera_pitch > 90.0)
			camera_pitch = 90.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	//	// ���{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	//	if ( drag_mouse_m )
	// SHIFT�L�[ �{ �E�{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	if (drag_mouse_r && (mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̏c�ړ��ɉ����ċ������ړ�
		camera_distance += (my - last_mouse_y) * 0.2;
		if (camera_distance < 2.0)
			camera_distance = 2.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	// ����̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseDrag2(int mx, int my)
{
	// SHIFT�L�[�̉�����Ԃ��擾
	int  mod = glutGetModifiers();

	// �E�{�^���̃h���b�O���͎��_����]����
	//	if ( drag_mouse_r )
	if (drag_mouse_r && !(mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̉��ړ��ɉ����Ăx���𒆐S�ɉ�]
		camera_yaw -= (mx - last_mouse_x) * 1.0;
		if (camera_yaw < 0.0)
			camera_yaw += 360.0;
		else if (camera_yaw > 360.0)
			camera_yaw -= 360.0;

		// �}�E�X�̏c�ړ��ɉ����Ăw���𒆐S�ɉ�]
		camera_pitch -= (my - last_mouse_y) * 1.0;
		if (camera_pitch < -90.0)
			camera_pitch = -90.0;
		else if (camera_pitch > 90.0)
			camera_pitch = 90.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	//	// ���{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	//	if ( drag_mouse_m )
	// SHIFT�L�[ �{ �E�{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	if (drag_mouse_r && (mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̏c�ړ��ɉ����ċ������ړ�
		camera_distance += (my - last_mouse_y) * 0.2;
		if (camera_distance < 2.0)
			camera_distance = 2.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	// ����̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseDrag3(int mx, int my)
{
	// SHIFT�L�[�̉�����Ԃ��擾
	int  mod = glutGetModifiers();

	// �E�{�^���̃h���b�O���͎��_����]����
	//	if ( drag_mouse_r )
	if (drag_mouse_r && !(mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̉��ړ��ɉ����Ăx���𒆐S�ɉ�]
		camera_yaw -= (mx - last_mouse_x) * 1.0;
		if (camera_yaw < 0.0)
			camera_yaw += 360.0;
		else if (camera_yaw > 360.0)
			camera_yaw -= 360.0;

		// �}�E�X�̏c�ړ��ɉ����Ăw���𒆐S�ɉ�]
		camera_pitch -= (my - last_mouse_y) * 1.0;
		if (camera_pitch < -90.0)
			camera_pitch = -90.0;
		else if (camera_pitch > 90.0)
			camera_pitch = 90.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	//	// ���{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	//	if ( drag_mouse_m )
	// SHIFT�L�[ �{ �E�{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	if (drag_mouse_r && (mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̏c�ړ��ɉ����ċ������ړ�
		camera_distance += (my - last_mouse_y) * 0.2;
		if (camera_distance < 2.0)
			camera_distance = 2.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	// ����̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
void  GLUTBaseApp::MouseDrag4(int mx, int my)
{
	// SHIFT�L�[�̉�����Ԃ��擾
	int  mod = glutGetModifiers();

	// �E�{�^���̃h���b�O���͎��_����]����
	//	if ( drag_mouse_r )
	if (drag_mouse_r && !(mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̉��ړ��ɉ����Ăx���𒆐S�ɉ�]
		camera_yaw -= (mx - last_mouse_x) * 1.0;
		if (camera_yaw < 0.0)
			camera_yaw += 360.0;
		else if (camera_yaw > 360.0)
			camera_yaw -= 360.0;

		// �}�E�X�̏c�ړ��ɉ����Ăw���𒆐S�ɉ�]
		camera_pitch -= (my - last_mouse_y) * 1.0;
		if (camera_pitch < -90.0)
			camera_pitch = -90.0;
		else if (camera_pitch > 90.0)
			camera_pitch = 90.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	//	// ���{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	//	if ( drag_mouse_m )
	// SHIFT�L�[ �{ �E�{�^���̃h���b�O���͎��_�ƃJ�����̋�����ύX����
	if (drag_mouse_r && (mod & GLUT_ACTIVE_SHIFT))
	{
		// �O��̃}�E�X���W�ƍ���̃}�E�X���W�̍��ɉ����Ď��_����]

		// �}�E�X�̏c�ړ��ɉ����ċ������ړ�
		camera_distance += (my - last_mouse_y) * 0.2;
		if (camera_distance < 2.0)
			camera_distance = 2.0;

		// ���_�̍X�V�t���O��ݒ�
		is_view_updated = true;
	}

	// ����̃}�E�X���W���L�^
	last_mouse_x = mx;
	last_mouse_y = my;
}
//
//  �}�E�X�ړ�
//
void  GLUTBaseApp::MouseMotion(int mx, int my)
{
}
void  GLUTBaseApp::MouseMotion2(int mx, int my)
{
}
void  GLUTBaseApp::MouseMotion3(int mx, int my)
{
}
void  GLUTBaseApp::MouseMotion4(int mx, int my)
{
}

//
//  �L�[�{�[�h�̃L�[����
//
void  GLUTBaseApp::Keyboard(unsigned char key, int mx, int my)
{
}
void  GLUTBaseApp::Keyboard2(unsigned char key, int mx, int my)
{
}
void  GLUTBaseApp::Keyboard3(unsigned char key, int mx, int my)
{
}
void  GLUTBaseApp::Keyboard4(unsigned char key, int mx, int my)
{
}
//
//  �L�[�{�[�h�̓���L�[����
//
void  GLUTBaseApp::KeyboardSpecial(unsigned char key, int mx, int my)
{
}


//
//  �A�j���[�V��������
//
void  GLUTBaseApp::Animation(float delta)
{
}


//
//  �ȉ��A�⏕����
//


//
//  �i�q�͗l�̏���`��
//
void  GLUTBaseApp::DrawFloor(float tile_size, int num_x, int num_z, float r0, float g0, float b0, float r1, float g1, float b1)
{
	int  x, z;
	float  ox, oz;

	glBegin(GL_QUADS);
	glNormal3d(0.0, 1.0, 0.0);

	ox = -(num_x * tile_size) / 2;
	for (x = 0; x<num_x; x++)
	{
		oz = -(num_z * tile_size) / 2;
		for (z = 0; z<num_z; z++)
		{
			/*if (((x + z) % 2) == 0)
				glColor3f(r0, g0, b0);
			else
				glColor3f(r1, g1, b1);*/
			
				glColor3f(r0, g0, b0);
			
			glTexCoord2d(0.0f, 0.0f);
			glVertex3d(ox, 0.0, oz);
			glTexCoord2d(0.0f, 1.0f);
			glVertex3d(ox, 0.0, oz + tile_size);
			glTexCoord2d(1.0f, 1.0f);
			glVertex3d(ox + tile_size, 0.0, oz + tile_size);
			glTexCoord2d(1.0f, 0.0f);
			glVertex3d(ox + tile_size, 0.0, oz);

			oz += tile_size;
		}
		ox += tile_size;
	}
	glEnd();
}


//
//  ��������`��
//
void  GLUTBaseApp::DrawTextInformation(int line_no, const char * message)
{
	int   i;
	if (message == NULL)
		return;

	// �ˉe�s����������i�������̑O�Ɍ��݂̍s���ޔ��j
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, win_width, win_height, 0.0);

	// ���f���r���[�s����������i�������̑O�Ɍ��݂̍s���ޔ��j
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// �y�o�b�t�@�E���C�e�B���O�̓I�t�ɂ���
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// ���b�Z�[�W�̕`��
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2i(16, 28 + 24 * line_no);
	for (i = 0; message[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);

	// �ݒ��S�ĕ���
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



///////////////////////////////////////////////////////////////////////////////
//
//  GLUT�t���[�����[�N�i�C�x���g�����A�������E���C�������j
//


//
//  ��ʕ`�掞�ɌĂ΂��R�[���o�b�N�֐�
//
void  DisplayCallback(void)
{
	// �A�v���P�[�V�����̕`�揈��
	if (app)
		app->Display();

	// �o�b�N�o�b�t�@�ɕ`�悵����ʂ��t�����g�o�b�t�@�ɕ\��
	glutSwapBuffers();
}


//
//  �E�B���h�E�T�C�Y�ύX���ɌĂ΂��R�[���o�b�N�֐�
//
void  ReshapeCallback(int w, int h)
{
	// �E�B���h�E���̕`����s���͈͂�ݒ�i�����ł̓E�B���h�E�S�̂ɕ`��j
	glViewport(0, 0, w, h);

	// �J�������W�n���X�N���[�����W�n�ւ̕ϊ��s���ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / h, 1, 500);

	// �A�v���P�[�V�����̃E�B���h�E�T�C�Y�ύX
	if (app)
		app->Reshape(w, h);
}


//
//  �}�E�X�N���b�N���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseClickCallback(int button, int state, int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�N���b�N
	if (app)
		app->MouseClick(button, state, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
// �}�E�X�h���b�O���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseDragCallback(int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�h���b�O
	if (app)
		app->MouseDrag(mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �L�[�{�[�h�̃L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  KeyboardCallback(unsigned char key, int mx, int my)
{
	// m �L�[�Ń��[�h�̐؂�ւ�
	if ((key == 'm') && app && (applications.size() > 1))
	{
		// ���̃A�v���P�[�V������I��
		int  app_no = 0;
		for (int i = 0; i < applications.size(); i++)
		{
			if (app == applications[i])
			{
				app_no = i;
				break;
			}
		}
		app_no = (app_no + 1) % applications.size();

		// ���s�A�v���P�[�V�����̐ؑ�
		ChangeApp(app_no);
	}

	// r �L�[�ŃA�v���P�[�V�����̃��Z�b�g
	if (key == 'r')
	{
		if (app)
			app->Start();
	}

	// �A�v���P�[�V�����̃L�[����
	if (app)
		app->Keyboard(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}



//
//  �L�[�{�[�h�̓���L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  SpecialKeyboardCallback(int key, int mx, int my)
{
	// �A�v���P�[�V�����̓���L�[����
	if (app)
		app->KeyboardSpecial(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �A�C�h�����ɌĂ΂��R�[���o�b�N�֐�
//
void  IdleCallback(void)
{
	int WindowNum = 4; //�E�B���h�E�̐�
	// �A�j���[�V��������
	if (app)
	{

		// �A�j���[�V�����̎��ԕω��i�����j���v�Z
#ifdef  WIN32
		// �V�X�e�����Ԃ��擾���A�O�񂩂�̌o�ߎ��Ԃɉ����ă���������
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = (curr_time - last_time) * 0.001f;
		if (delta > 0.03f)
			delta = 0.03f;
		last_time = curr_time;
#else
		// �Œ�̃������g�p
		float  delta = 0.03f;
#endif

		// �A�v���P�[�V�����̃A�j���[�V��������
		if (app)
			app->Animation(delta);
		
		// �ĕ`��̎w�����o���i���̌�ōĕ`��̃R�[���o�b�N�֐����Ă΂��j
		for (int loop = 0; loop < WindowNum; ++loop){
			glutSetWindow(WinID[loop]);
			glutPostRedisplay(); //�ĕ`�� (��display()�֐����Ăяo���֐� )
		}
	}

#ifdef _WIN32
	// Windows���ł́ACTRL�{�E�N���b�N�ł����j���[���Ăяo����悤�ɂ���

	// �E�N���b�N�ł̃��j���[�N���̏��
	static bool  menu_attached = false;

	// CTRL�L�[�̉�����Ԃ��擾�iWin32 API ���g�p�j
	bool  ctrl = (GetKeyState(VK_CONTROL) & 0x80);

	// �E�N���b�N�ł̃��j���[�N���̓o�^�E����
	if (ctrl && !menu_attached)
	{
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = true;
	}
	else if (!ctrl && menu_attached)
	{
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = false;
	}
#endif
}

//
//  ��ʕ`�掞�ɌĂ΂��R�[���o�b�N�֐�
//
void  DisplayCallback2(void)
{
	// �A�v���P�[�V�����̕`�揈��
	if (app)
		app->Display2();

	// �o�b�N�o�b�t�@�ɕ`�悵����ʂ��t�����g�o�b�t�@�ɕ\��
	glutSwapBuffers();
}


//
//  �E�B���h�E�T�C�Y�ύX���ɌĂ΂��R�[���o�b�N�֐�
//
void  ReshapeCallback2(int w, int h)
{
	// �E�B���h�E���̕`����s���͈͂�ݒ�i�����ł̓E�B���h�E�S�̂ɕ`��j
	glViewport(0, 0, w, h);

	// �J�������W�n���X�N���[�����W�n�ւ̕ϊ��s���ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / h, 1, 500);

	// �A�v���P�[�V�����̃E�B���h�E�T�C�Y�ύX
	if (app)
		app->Reshape2(w, h);
}


//
//  �}�E�X�N���b�N���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseClickCallback2(int button, int state, int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�N���b�N
	if (app)
		app->MouseClick2(button, state, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
// �}�E�X�h���b�O���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseDragCallback2(int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�h���b�O
	if (app)
		app->MouseDrag2(mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �L�[�{�[�h�̃L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  KeyboardCallback2(unsigned char key, int mx, int my)
{
	// m �L�[�Ń��[�h�̐؂�ւ�
	if ((key == 'm') && app && (applications.size() > 1))
	{
		// ���̃A�v���P�[�V������I��
		int  app_no = 0;
		for (int i = 0; i < applications.size(); i++)
		{
			if (app == applications[i])
			{
				app_no = i;
				break;
			}
		}
		app_no = (app_no + 1) % applications.size();

		// ���s�A�v���P�[�V�����̐ؑ�
		ChangeApp(app_no);
	}

	// r �L�[�ŃA�v���P�[�V�����̃��Z�b�g
	if (key == 'r')
	{
		if (app)
			app->Start();
	}

	// �A�v���P�[�V�����̃L�[����
	if (app)
		app->Keyboard(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}



//
//  �L�[�{�[�h�̓���L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  SpecialKeyboardCallback2(int key, int mx, int my)
{
	// �A�v���P�[�V�����̓���L�[����
	if (app)
		app->KeyboardSpecial(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �A�C�h�����ɌĂ΂��R�[���o�b�N�֐�
//
void  IdleCallback2(void)
{
	int WindowNum = 2; //�E�B���h�E�̐�
	// �A�j���[�V��������
	if (app)
	{

		// �A�j���[�V�����̎��ԕω��i�����j���v�Z
#ifdef  WIN32
		// �V�X�e�����Ԃ��擾���A�O�񂩂�̌o�ߎ��Ԃɉ����ă���������
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = (curr_time - last_time) * 0.001f;
		if (delta > 0.03f)
			delta = 0.03f;
		last_time = curr_time;
#else
		// �Œ�̃������g�p
		float  delta = 0.03f;
#endif

		// �A�v���P�[�V�����̃A�j���[�V��������
		if (app)
			app->Animation(delta);
		
		// �ĕ`��̎w�����o���i���̌�ōĕ`��̃R�[���o�b�N�֐����Ă΂��j
		for (int loop = 0; loop < WindowNum; ++loop){
			glutSetWindow(WinID[loop]);
			glutPostRedisplay(); //�ĕ`�� (��display()�֐����Ăяo���֐� )
		}
	}

#ifdef _WIN32
	// Windows���ł́ACTRL�{�E�N���b�N�ł����j���[���Ăяo����悤�ɂ���

	// �E�N���b�N�ł̃��j���[�N���̏��
	static bool  menu_attached = false;

	// CTRL�L�[�̉�����Ԃ��擾�iWin32 API ���g�p�j
	bool  ctrl = (GetKeyState(VK_CONTROL) & 0x80);

	// �E�N���b�N�ł̃��j���[�N���̓o�^�E����
	if (ctrl && !menu_attached)
	{
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = true;
	}
	else if (!ctrl && menu_attached)
	{
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = false;
	}
#endif
}
//
//  ��ʕ`�掞�ɌĂ΂��R�[���o�b�N�֐�
//
void  DisplayCallback3(void)
{
	// �A�v���P�[�V�����̕`�揈��
	if (app)
		app->Display3();

	// �o�b�N�o�b�t�@�ɕ`�悵����ʂ��t�����g�o�b�t�@�ɕ\��
	glutSwapBuffers();
}


//
//  �E�B���h�E�T�C�Y�ύX���ɌĂ΂��R�[���o�b�N�֐�
//
void  ReshapeCallback3(int w, int h)
{
	// �E�B���h�E���̕`����s���͈͂�ݒ�i�����ł̓E�B���h�E�S�̂ɕ`��j
	glViewport(0, 0, w, h);

	// �J�������W�n���X�N���[�����W�n�ւ̕ϊ��s���ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / h, 1, 500);

	// �A�v���P�[�V�����̃E�B���h�E�T�C�Y�ύX
	if (app)
		app->Reshape3(w, h);
}


//
//  �}�E�X�N���b�N���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseClickCallback3(int button, int state, int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�N���b�N
	if (app)
		app->MouseClick3(button, state, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
// �}�E�X�h���b�O���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseDragCallback3(int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�h���b�O
	if (app)
		app->MouseDrag3(mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �L�[�{�[�h�̃L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  KeyboardCallback3(unsigned char key, int mx, int my)
{
	// m �L�[�Ń��[�h�̐؂�ւ�
	if ((key == 'm') && app && (applications.size() > 1))
	{
		// ���̃A�v���P�[�V������I��
		int  app_no = 0;
		for (int i = 0; i < applications.size(); i++)
		{
			if (app == applications[i])
			{
				app_no = i;
				break;
			}
		}
		app_no = (app_no + 1) % applications.size();

		// ���s�A�v���P�[�V�����̐ؑ�
		ChangeApp(app_no);
	}

	// r �L�[�ŃA�v���P�[�V�����̃��Z�b�g
	if (key == 'r')
	{
		if (app)
			app->Start();
	}

	// �A�v���P�[�V�����̃L�[����
	if (app)
		app->Keyboard(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}



//
//  �L�[�{�[�h�̓���L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  SpecialKeyboardCallback3(int key, int mx, int my)
{
	// �A�v���P�[�V�����̓���L�[����
	if (app)

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �A�C�h�����ɌĂ΂��R�[���o�b�N�֐�
//
void  IdleCallback3(void)
{
	int WindowNum = 2; //�E�B���h�E�̐�
	// �A�j���[�V��������
	if (app)
	{

		// �A�j���[�V�����̎��ԕω��i�����j���v�Z
#ifdef  WIN32
		// �V�X�e�����Ԃ��擾���A�O�񂩂�̌o�ߎ��Ԃɉ����ă���������
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = (curr_time - last_time) * 0.001f;
		if (delta > 0.03f)
			delta = 0.03f;
		last_time = curr_time;
#else
		// �Œ�̃������g�p
		float  delta = 0.03f;
#endif

		// �A�v���P�[�V�����̃A�j���[�V��������
		if (app)
			app->Animation(delta);

		// �ĕ`��̎w�����o���i���̌�ōĕ`��̃R�[���o�b�N�֐����Ă΂��j
		for (int loop = 0; loop < WindowNum; ++loop){
			glutSetWindow(WinID[loop]);
			glutPostRedisplay(); //�ĕ`�� (��display()�֐����Ăяo���֐� )
		}
	}

#ifdef _WIN32
	// Windows���ł́ACTRL�{�E�N���b�N�ł����j���[���Ăяo����悤�ɂ���

	// �E�N���b�N�ł̃��j���[�N���̏��
	static bool  menu_attached = false;

	// CTRL�L�[�̉�����Ԃ��擾�iWin32 API ���g�p�j
	bool  ctrl = (GetKeyState(VK_CONTROL) & 0x80);

	// �E�N���b�N�ł̃��j���[�N���̓o�^�E����
	if (ctrl && !menu_attached)
	{
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = true;
	}
	else if (!ctrl && menu_attached)
	{
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = false;
	}
#endif
}
//
//  ��ʕ`�掞�ɌĂ΂��R�[���o�b�N�֐�
//
void  DisplayCallback4(void)
{
	// �A�v���P�[�V�����̕`�揈��
	if (app)
		app->Display4();

	// �o�b�N�o�b�t�@�ɕ`�悵����ʂ��t�����g�o�b�t�@�ɕ\��
	glutSwapBuffers();
}


//
//  �E�B���h�E�T�C�Y�ύX���ɌĂ΂��R�[���o�b�N�֐�
//
void  ReshapeCallback4(int w, int h)
{
	// �E�B���h�E���̕`����s���͈͂�ݒ�i�����ł̓E�B���h�E�S�̂ɕ`��j
	glViewport(0, 0, w, h);

	// �J�������W�n���X�N���[�����W�n�ւ̕ϊ��s���ݒ�
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)w / h, 1, 500);

	// �A�v���P�[�V�����̃E�B���h�E�T�C�Y�ύX
	if (app)
		app->Reshape4(w, h);
}


//
//  �}�E�X�N���b�N���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseClickCallback4(int button, int state, int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�N���b�N
	if (app)
		app->MouseClick4(button, state, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
// �}�E�X�h���b�O���ɌĂ΂��R�[���o�b�N�֐�
//
void  MouseDragCallback4(int mx, int my)
{
	// �A�v���P�[�V�����̃}�E�X�h���b�O
	if (app)
		app->MouseDrag4(mx, my);

	// �ĕ`��
	glutPostRedisplay();
}


//
//  �L�[�{�[�h�̃L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  KeyboardCallback4(unsigned char key, int mx, int my)
{
	// m �L�[�Ń��[�h�̐؂�ւ�
	if ((key == 'm') && app && (applications.size() > 1))
	{
		// ���̃A�v���P�[�V������I��
		int  app_no = 0;
		for (int i = 0; i < applications.size(); i++)
		{
			if (app == applications[i])
			{
				app_no = i;
				break;
			}
		}
		app_no = (app_no + 1) % applications.size();

		// ���s�A�v���P�[�V�����̐ؑ�
		ChangeApp(app_no);
	}

	// r �L�[�ŃA�v���P�[�V�����̃��Z�b�g
	if (key == 'r')
	{
		if (app)
			app->Start();
	}

	// �A�v���P�[�V�����̃L�[����
	if (app)
		app->Keyboard(key, mx, my);

	// �ĕ`��
	glutPostRedisplay();
}



//
//  �L�[�{�[�h�̓���L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void  SpecialKeyboardCallback4(int key, int mx, int my)
{
	// �A�v���P�[�V�����̓���L�[����
	if (app)

		// �ĕ`��
		glutPostRedisplay();
}


//
//  �A�C�h�����ɌĂ΂��R�[���o�b�N�֐�
//
void  IdleCallback4(void)
{
	int WindowNum = 2; //�E�B���h�E�̐�
	// �A�j���[�V��������
	if (app)
	{

		// �A�j���[�V�����̎��ԕω��i�����j���v�Z
#ifdef  WIN32
		// �V�X�e�����Ԃ��擾���A�O�񂩂�̌o�ߎ��Ԃɉ����ă���������
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = (curr_time - last_time) * 0.001f;
		if (delta > 0.03f)
			delta = 0.03f;
		last_time = curr_time;
#else
		// �Œ�̃������g�p
		float  delta = 0.03f;
#endif

		// �A�v���P�[�V�����̃A�j���[�V��������
		if (app)
			app->Animation(delta);

		// �ĕ`��̎w�����o���i���̌�ōĕ`��̃R�[���o�b�N�֐����Ă΂��j
		for (int loop = 0; loop < WindowNum; ++loop){
			glutSetWindow(WinID[loop]);
			glutPostRedisplay(); //�ĕ`�� (��display()�֐����Ăяo���֐� )
		}
	}

#ifdef _WIN32
	// Windows���ł́ACTRL�{�E�N���b�N�ł����j���[���Ăяo����悤�ɂ���

	// �E�N���b�N�ł̃��j���[�N���̏��
	static bool  menu_attached = false;

	// CTRL�L�[�̉�����Ԃ��擾�iWin32 API ���g�p�j
	bool  ctrl = (GetKeyState(VK_CONTROL) & 0x80);

	// �E�N���b�N�ł̃��j���[�N���̓o�^�E����
	if (ctrl && !menu_attached)
	{
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = true;
	}
	else if (!ctrl && menu_attached)
	{
		glutDetachMenu(GLUT_RIGHT_BUTTON);
		menu_attached = false;
	}
#endif
}

//
//  ���s�A�v���P�[�V�����̐ؑ�
//
void  ChangeApp(int app_no)
{
	if ((app_no < 0) || (app_no >= applications.size()))
		return;

	// ���݂̃E�B���h�E�̃T�C�Y���擾
	int  win_width, win_height;
	GLUTBaseApp *  curr_app = app;
	if (curr_app)
	{
		win_width = curr_app->GetWindowWidth();
		win_height = curr_app->GetWindowHeight();
	}

	// �A�v���P�[�V�����̏������E�J�n
	app = applications[app_no];
	if (!app->IsInitialized())
		app->Initialize();
	app->Start();
	if (curr_app)
		app->Reshape(win_width, win_height);
}


//
//  �|�b�v�A�b�v���j���[�I�����ɌĂ΂��R�[���o�b�N�֐�
//
void  MenuCallback(int no)
{

	// ���s�A�v���P�[�V�����̐ؑ�
	if ((no >= 0) && (no < applications.size()))
	{
		ChangeApp(no);
	}

	// �\�t�g�E�F�A������\���i�_�C�A���O�{�b�N�X���g�p�j
	else if (no == applications.size())
	{
#ifdef _WIN32

		MessageBox(NULL, software_description, "About", MB_OK | MB_ICONINFORMATION);
		
#endif
	}
}


//
//  ���s�A�v���P�[�V�����ؑւ̂��߂̃|�b�v�A�b�v���j���[�̏�����
//
void  InitAppMenu()
{
	// ���j���[����
	int  menu;
	menu = glutCreateMenu(MenuCallback);

	// �e�A�v���P�[�V�����̃��j���[���ڂ�ǉ�
	for (int i = 0; i < applications.size(); i++)
	{
		glutAddMenuEntry(applications[i]->GetAppName().c_str(), i);
	}

	// �\�t�g�E�F�A�����̂��߂̃��j���[���ڂ�ǉ�
	glutAddMenuEntry("About...", applications.size());

	// ���j���[�ݒ�
	glutSetMenu(menu);

	// ���j���[�o�^�i�}�E�X�̒��{�^���ŕ\�������悤�ɐݒ�j
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
}


//
//  �����_�����O���������֐�
//
void  initEnvironment(void)
{
	// �������쐬����
	float  light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
	float  light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	float  light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	float  light0_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glEnable(GL_LIGHT0);

	// �����v�Z��L���ɂ���
	glEnable(GL_LIGHTING);

	// ���̂̐F����L���ɂ���
	glEnable(GL_COLOR_MATERIAL);

	// �y�e�X�g��L���ɂ���
	glEnable(GL_DEPTH_TEST);

	// �w�ʏ�����L���ɂ���
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// �w�i�F��ݒ�
	glClearColor(0.0, 0.0, 0.0, 0.0);
}



///////////////////////////////////////////////////////////////////////////////
//
//  GLUT�t���[�����[�N�̃��C���֐�
//


//
//  GLUT�t���[�����[�N�̃��C���֐��i���s����A�v���P�[�V�����̃��X�g���w��j
//
int  SimpleHumanGLUTMain(const vector< class GLUTBaseApp * > & app_lists, int argc, char ** argv, const char * win_title, const char * win_title2, const char * win_title3, const char * win_title4, int win_width, int win_height, int*  Winid)
{
	// GLUT�E�B���h�E�̃p�����^�̌���i�����Ŏw�肳��Ȃ������ꍇ�̓f�t�H���g�l��ݒ�j
	if (!win_title || (strlen(win_title) == 0))
		win_title = "�}�X�^�[�f�[�^";
	if (!win_title2 || (strlen(win_title2) == 0))
		win_title2 = "�팱�҃f�[�^";
	if (!win_title3 || (strlen(win_title3) == 0))
		win_title3 = "�C�����@";
	if (!win_title4 || (strlen(win_title3) == 0))
		win_title4 = "�}�X�^�[�f�[�^+�팱�҃f�[�^";
	if (win_width <= 0)
		win_width = 640;
	if (win_height <= 0)
		win_height = 640;

	// GLUT�̏�����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(0, 0);
	WinID[0]=glutCreateWindow(win_title);
	// �R�[���o�b�N�֐��̓o�^
	glutDisplayFunc(DisplayCallback);
	glutReshapeFunc(ReshapeCallback);
	glutMouseFunc(MouseClickCallback);
	glutMotionFunc(MouseDragCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(SpecialKeyboardCallback);
	glutIdleFunc(IdleCallback);

	// �����_�����O��������
	initEnvironment();

	// �S�A�v���P�[�V������o�^
	applications = app_lists;

	// �ŏ��̃A�v���P�[�V���������s�J�n
	ChangeApp(0);

	// ���s�A�v���P�[�V�����ؑւ̂��߂̃|�b�v�A�b�v���j���[�̏�����
	InitAppMenu();

	// GLUT�̃��C�����[�v�ɏ������ڂ�
	//glutMainLoop();
	//glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(640, 0);
	WinID[1] = glutCreateWindow(win_title2);
	// �R�[���o�b�N�֐��̓o�^
	glutDisplayFunc(DisplayCallback2);
	glutReshapeFunc(ReshapeCallback2);
	glutMouseFunc(MouseClickCallback2);
	glutMotionFunc(MouseDragCallback2);
	glutKeyboardFunc(KeyboardCallback2);
	glutSpecialFunc(SpecialKeyboardCallback2);
	glutIdleFunc(IdleCallback);

	// �����_�����O��������
	initEnvironment();

	// �S�A�v���P�[�V������o�^
	applications = app_lists;

	// �ŏ��̃A�v���P�[�V���������s�J�n
	ChangeApp(0);

	// ���s�A�v���P�[�V�����ؑւ̂��߂̃|�b�v�A�b�v���j���[�̏�����
	InitAppMenu();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(0, 640);
	WinID[2] = glutCreateWindow(win_title3);
	// �R�[���o�b�N�֐��̓o�^
	glutDisplayFunc(DisplayCallback3);
	glutReshapeFunc(ReshapeCallback3);
	glutMouseFunc(MouseClickCallback3);
	glutMotionFunc(MouseDragCallback3);
	glutKeyboardFunc(KeyboardCallback3);
	glutSpecialFunc(SpecialKeyboardCallback3);
	glutIdleFunc(IdleCallback);

	// �����_�����O��������
	initEnvironment();

	// �S�A�v���P�[�V������o�^
	applications = app_lists;

	// �ŏ��̃A�v���P�[�V���������s�J�n
	ChangeApp(0);

	// ���s�A�v���P�[�V�����ؑւ̂��߂̃|�b�v�A�b�v���j���[�̏�����
	InitAppMenu();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(640, 640);
	WinID[3] = glutCreateWindow(win_title4);
	// �R�[���o�b�N�֐��̓o�^
	glutDisplayFunc(DisplayCallback4);
	glutReshapeFunc(ReshapeCallback4);
	glutMouseFunc(MouseClickCallback4);
	glutMotionFunc(MouseDragCallback4);
	glutKeyboardFunc(KeyboardCallback4);
	glutSpecialFunc(SpecialKeyboardCallback4);
	glutIdleFunc(IdleCallback);

	// �����_�����O��������
	initEnvironment();

	// �S�A�v���P�[�V������o�^
	applications = app_lists;

	// �ŏ��̃A�v���P�[�V���������s�J�n
	ChangeApp(0);

	// ���s�A�v���P�[�V�����ؑւ̂��߂̃|�b�v�A�b�v���j���[�̏�����
	InitAppMenu();
	// GLUT�̃��C�����[�v�ɏ������ڂ�
	glutMainLoop();
	return 0;
}

//
//  GLUT�t���[�����[�N�̃��C���֐��i���s�����̃A�v���P�[�V�������w��j
//
int  SimpleHumanGLUTMain(class GLUTBaseApp * app, int argc, char ** argv, const char * win_title, int win_width, int win_height, int*  Winid)
{
	vector< class GLUTBaseApp * >    app_lists;
	app_lists.push_back(app);

	return  SimpleHumanGLUTMain(app_lists, argc, argv, win_title, win_title, win_title, win_title, win_width, win_height, WinID);
}



