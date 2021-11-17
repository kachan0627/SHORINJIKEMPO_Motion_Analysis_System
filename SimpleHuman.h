/**
***  �L�����N�^�A�j���[�V�����̂��߂̐l�̃��f���̕\���E��{���� ���C�u�����E�T���v���v���O����
***  Copyright (c) 2015-, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http://opensource.org/licenses/mit-license.php
**/


#ifndef  _SIMPLE_HUMAN_H_
#define  _SIMPLE_HUMAN_H_


//
//  �s��E�x�N�g���̕\���ɂ� vecmath C++���C�u�����ihttp://objectclub.jp/download/vecmath1�j���g�p
//
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color4.h>

// STL�iStandard Template Library�j���g�p
#include <vector>
#include <string>
using namespace  std;

// �v���g�^�C�v�錾
struct  Segment;
struct  Joint;
struct  Skeleton;
struct  Posture;



//
//  �l�̃��f���̑̐߂�\���\����
//
struct  Segment
{
	// �̐ߔԍ��E���O
	int                  index;
	string               name;

	// �ڑ��֐�
	vector< Joint * >    joints;

	// �e�֐߂̐ڑ��ʒu�i�̐߂̃��[�J�����W�n�j
	vector< Point3f >    joint_positions;

	// �̐߂̖��[�ʒu
	bool                 has_site;
	Point3f              site_position;
};


//
//  �l�̃��f���̊֐߂�\���\����
//
struct  Joint
{
	// �֐ߔԍ��E���O
	int                  index;
	string               name;

	// �ڑ��̐�
	Segment *            segments[2];
};


//
//  �l�̃��f���̍��i��\���\����
//
struct  Skeleton
{
	// �֐ߐ�
	int                  num_segments;

	// �֐߂̔z�� [�֐ߔԍ�]
	Segment **           segments;

	// �̐ߐ�
	int                  num_joints;

	// �̐߂̔z�� [�̐ߔԍ�]
	Joint **             joints;


	// �R���X�g���N�^�E�f�X�g���N�^
	Skeleton();
	Skeleton(int s, int j);
	~Skeleton();
};


//
//  �l�̃��f���̎p����\���\����
//
struct  Posture
{
	// ���i���f��
	Skeleton *           body;

	// ���[�g�̈ʒu
	Point3f              root_pos;

	// ���[�g�̌����i��]�s��\���j
	Matrix3f             root_ori;

	// �e�֐߂̑��Ή�]�i��]�s��\���j[�֐ߔԍ�]
	Matrix3f *           joint_rotations;


	// �R���X�g���N�^�E�f�X�g���N�^
	Posture();
	Posture(Skeleton * b);
	Posture(const Posture & p);
	Posture &operator=(const Posture & p);
	~Posture();

	// ������
	void  Init(Skeleton * b);
};


//
//  �����\���\����
//
struct  Motion
{
	// ���i���f��
	Skeleton *           body;

	// �t���[����
	int                  num_frames;

	// �t���[���Ԃ̎��ԊԊu
	float                interval;

	// �S�t���[���̎p�� [�t���[���ԍ�]
	Posture *            frames;

	// ���얼
	string               name;


	// �R���X�g���N�^�E�f�X�g���N�^
	Motion();
	Motion(Skeleton * b, int num);
	Motion(const Motion & m);
	Motion &operator=(const Motion & m);
	~Motion();

	// ������
	void  Init(Skeleton * b, int num);

	// ����̒������擾
	float  GetDuration() const { return  num_frames * interval; }

	// �p�����擾
	Posture *  GetFrame(int no) const;
	Posture *  GetFrameTime(float time) const;
	void  GetPosture(float time, Posture & p) const;
	void  GetPosture(int no, Posture & p) const;
};


//
//  �L�[�t���[�������\���\����
//
struct  KeyframeMotion
{
	// ���i���f��
	Skeleton *           body;

	// �L�[�t���[����
	int                  num_keyframes;

	// �e�L�[�����̔z�� �m�L�[�t���[���ԍ��n 
	float  *             key_times;

	// �e�L�[�p���̔z�� �m�L�[�t���[���ԍ��n 
	Posture *            key_poses;


	// �R���X�g���N�^�E�f�X�g���N�^
	KeyframeMotion();
	KeyframeMotion(Skeleton * b, int num);
	KeyframeMotion(const KeyframeMotion & m);
	KeyframeMotion &operator=(const KeyframeMotion & m);
	~KeyframeMotion();

	// ������
	void  Init(Skeleton * b, int num);
	void  Init(Skeleton * b, int num, const float * times, const Posture * poses);

	// ����̒������擾
	float  GetDuration() const;

	// �p�����擾
	void  GetPosture(float time, Posture & p) const;
};


//
//  �l�̃��f���̍��i�E�p���E����̊�{����
//

// �p���̏�����
void  InitPosture(Posture & posture, Skeleton * body = NULL);

// BVH���삩�獜�i���f���𐶐�
Skeleton *  CoustructBVHSkeleton(class BVH * bvh);

// BVH���삩�瓮��f�[�^�i�{���i���f���j�𐶐�
Motion *  CoustructBVHMotion(class BVH * bvh, Skeleton * bvh_body = NULL);

// BVH���삩��p�����擾
void  GetBVHPosture(const class BVH * bvh, int frame_no, Posture & posture);

// ���^���w�v�Z
void  ForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array, vector< Point3f > & joi_pos_array);

// ���^���w�v�Z
void  ForwardKinematics(const Posture & posture, vector< Matrix4f > & seg_frame_array);

// �p���̕`��i�X�e�B�b�N�t�B�M���A�ŕ`��j
void  DrawPosture(const Posture & posture);

// �p���̉e�̕`��i�X�e�B�b�N�t�B�M���A�ŕ`��j
void  DrawPostureShadow(const Posture & posture, const Vector3f & light_dir, const Color4f & color);

// �p����ԁi�Q�̎p�����ԁj
void  PostureInterpolation(const Posture & p0, const Posture & p1, float ratio, Posture & p);


/*
// �� �ȉ��̊�{�����́A���|�[�g�ۑ�ō쐬

// �ϊ��s��̐��������i���ʊp�j�������v�Z
float  ComputeOrientation( const Matrix3f & ori );

// �Q�̎p���̈ʒu�E���������킹�邽�߂̕ϊ��s����v�Z
//�inext_frame �̈ʒu�E�������Aprev_frame �̈ʒu�����ɍ��킹�邽�߂̕ϊ��s�� trans_mat ���v�Z�j
void  ComputeConnectionTransformation( const Matrix4f & prev_frame, const Matrix4f & next_frame, Matrix4f & trans_mat );

// �p���̈ʒu�E�����ɕϊ��s���K�p
void  TransformPosture( const Matrix4f & trans, Posture & posture );

// ���[�֐߂���x�_�֐߂ւ̃p�X�i�֐߂̔z��Ɗe�֐߂ɂ����閖�[�֐߂̕����j��T��
void  FindJointPath( const Skeleton * body, int base_joint_no, int ee_joint_no, vector< int > & joint_path, vector< int > & joint_path_signs );

// Inverse Kinematics �v�Z�iCCD�@�j
void  ApplyInverseKinematicsCCD( Posture & posture, int base_joint_no, int ee_joint_no, Point3f ee_joint_position );

*/


#endif // _SIMPLE_HUMAN_H_
