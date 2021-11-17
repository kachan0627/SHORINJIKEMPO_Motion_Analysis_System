/**
***  BVH Player
***  BVH����t�@�C���̓ǂݍ��݁E�`��N���X
***  Copyright (c) 2004-2017, Masaki OSHITA (www.oshita-lab.org)
***  Released under the MIT license http://opensource.org/licenses/mit-license.php
**/


#ifndef  _BVH_H_
#define  _BVH_H_
#define ROOT_2_INV 0.70710678118654752440
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include "quat.h"
#include "Calc.h"
#include"RotationMatrix.h"
#include "Motionbool.h"
#include <Vector3.h>
#include <Point3.h>
#include <Matrix3.h>
#include <Matrix4.h>
#include <Color3.h>
using namespace  std;



//
//  BVH�`���̃��[�V�����f�[�^
//
class  BVH
{
  public:
	/*  �����p�\����  */

	// �`�����l���̎��
	enum  ChannelEnum
	{
		X_ROTATION, Y_ROTATION, Z_ROTATION,
		X_POSITION, Y_POSITION, Z_POSITION
	};
	struct  Joint;

	// �`�����l�����
	struct  Channel
	{
		// �Ή��֐�
		Joint *              joint;
		
		// �`�����l���̎��
		ChannelEnum          type;

		// �`�����l���ԍ�
		int                  index;
	};

	// �֐ߏ��
	struct  Joint
	{
		// �֐ߖ�
		string               name;
		// �֐ߔԍ�
		int                  index;

		// �֐ߊK�w�i�e�֐߁j
		Joint *              parent;
		// �֐ߊK�w�i�q�֐߁j
		vector< Joint * >    children;

		// �ڑ��ʒu
		double               offset[3];

		// ���[�ʒu���������ǂ����̃t���O
		bool                 has_site;
		// ���[�ʒu
		double               site[3];

		// ��]��
		vector< Channel * >  channels;
		//�t���[�������ŗp����֐߂��ǂ����̃t���O
		bool				 p_flag;
		//�e�֐߈ʒu�x�N�g�� �ǉ��@
		PositionVector 	     offset_vector;		 
	};

	

  private://private�ɖ߂�!!
	// ���[�h�������������ǂ����̃t���O
	bool                     is_load_success;

	/*  �t�@�C���̏��  */
	string                   file_name;   // �t�@�C����
	string                   out_file_name;   // �t�@�C����
	string                   motion_name; // ���얼

	/*  �K�w�\���̏��  */
	int                      num_channel; // �`�����l����
	vector< Channel * >      channels;    // �`�����l����� [�`�����l���ԍ�]
	vector< Joint * >        joints;      // �֐ߏ�� [�p�[�c�ԍ�]
	map< string, Joint * >   joint_index; // �֐ߖ�����֐ߏ��ւ̃C���f�b�N�X

	/*  ���[�V�����f�[�^�̏��  */
	int                      num_frame;   // �t���[����
	double                   interval;    // �t���[���Ԃ̎��ԊԊu
	double *                 motion;      // [�t���[���ԍ�][�`�����l���ԍ�]
	float *					 sita_box;    //�t���[���Ԋp���x�i�[
	float *					 sita_box2;   //�����ʒu����p�x�i�[
	float*					 sita_box3;	  //�T�v�f�̃N�H�[�^�j�I����
	float *					 sita_box4;	  //�T�v�f�̉�]�p�����o
	float *					 sita_boxq;   //�N�H�[�^�j�I����
	float *					 sita_boxq2;
	float *					 sita_sum;	  //���W�A�����v
	int *					 sflame_box_spine;  //�Î~�t���[���ۑ�
	int *					 sflame_box_leftarm;  //�Î~�t���[���ۑ�
	int *					 sflame_box_rightarm;  //�Î~�t���[���ۑ�
	int *					 sflame_box_leftleg;  //�Î~�t���[���ۑ�
	int *					 sflame_box_rightleg;  //�Î~�t���[���ۑ�
	int *					 s30flame_box_spine;//�R�O�t���[���ȏ�ۑ�
	int *					 s30flame_box_leftarm;//�R�O�t���[���ȏ�ۑ�
	int *					 s30flame_box_rightarm;//�R�O�t���[���ȏ�ۑ�
	int *					 s30flame_box_leftleg;//�R�O�t���[���ȏ�ۑ�
	int *					 s30flame_box_rightleg;//�R�O�t���[���ȏ�ۑ�
	int *					 sflame_box_sum;
	int *					 sum5jointflame_box;//���ׂĂ̊֐߂ŐÎ~���Ă���t���[��
	vector<int>				 sum5jointflame_box10;//���ׂĂ̊֐߂ŐÎ~���Ă���t���[��
	int 					 num_spine;  //�Î~�t���[����
	int 					 num_leftarm;  //�Î~�t���[����
	int 					 num_rightarm;  //�Î~�t���[����
	int 					 num_leftleg;  //�Î~�t���[����
	int 					 num_rightleg;  //�Î~�t���[����
	int 					 num_spine30;  //�Î~�t���[����(30�ȏ�)
	int 					 num_leftarm30;  //�Î~�t���[����
	int 					 num_rightarm30;  //�Î~�t���[����
	int 					 num_leftleg30;  //�Î~�t���[����
	int 					 num_rightleg30;  //�Î~�t���[����
	int						 num_sum;
	int					     num_sum5jointflame;
	int						  num_sum5jointflame10;

	/*�t���[�������p�֐�*/
	vector<quat *>           q_motions;		  //�t���[���Ԃ̉�]�N�H�[�^�j�I��
	vector<quat *>           rq_motions;   	  //�t���[���Ԃ̉�]�N�H�[�^�j�I��(�m����)
	vector<quat *>			 rqdash_motions;  //�t���[���Ԃ̉�]�N�H�[�^�j�I��(�x�N�g���p)
	vector<quat *>           rq_s_motions;   	  //1�t���[���ڂƂ̉�]�N�H�[�^�j�I��(�m����)
	vector<quat *>			 rqdash_s_motions;  //1�t���[���ڂƂ̉�]�N�H�[�^�j�I��(�x�N�g���p)
	vector<quat *>			 select_motions;
	//PositionVector *	     p_vector;		  //�e�֐߈ʒu�x�N�g��
	vector< PositionVector *>	     flame_vectors;   //�t���[�����Ƃ̈ړ��x�N�g���ǉ��@���P�t���[��1�f�[�^�C���f�[�^���͂P,�W�W�T�ƂȂ�D
	vector< RotationMatrix *>	     r_motions;		  //��]�s��i�ُ�l�j�ǉ����T���v���f�[�^�͂P�t���[���Q�P�f�[�^�C�P,�W�W�T�t���[���̏ꍇ�C���f�[�^���R�X,�T�W�T�ƂȂ�D
	vector< RotationMatrix *>	     rdash_motions;		  //�N�H�[�^�j�I����p���ĉ�]�s��i�ُ�l�j�ǉ����T���v���f�[�^�͂P�t���[���Q�P�f�[�^�C�P,�W�W�T�t���[���̏ꍇ�C���f�[�^���R�X,�T�W�T�ƂȂ�D
	vector< RotationMatrix *>	     par_vectors;	  //��]�s��ϊ���̈ʒu�x�N�g�� �ǉ� ���T���v���f�[�^�͂P�t���[���Q0�f�[�^(root�̓x�N�g�����Ȃ����߁j�C�P,�W�W�T�t���[���̏ꍇ�C���f�[�^���R�V,�V�O�O�ƂȂ�D
	vector<RotationMatrix *>		 pardash_vectors; //par_vector�̍s������ւ���20*1885->1885*20�ɕϊ�

	vector<Motionbool*>				 sum5joint_bools;

  public:
	// �R���X�g���N�^�E�f�X�g���N�^
	BVH();
	BVH( const char * bvh_file_name );
	~BVH();

	// �S���̃N���A
	void  Clear();

	// BVH�t�@�C���̃��[�h
	void  Load_site();
	void  Load( const char * bvh_file_name );
	void loadcalcbvh(int c_num_channel, Channel *  c_channel, Joint *   c_new_joint, int c_num_frame, double c_interval, double * c_motion);
  public:
	/*  �f�[�^�A�N�Z�X�֐�  */

	// ���[�h�������������ǂ������擾
	bool  IsLoadSuccess() const { return is_load_success; }

	// �t�@�C���̏��̎擾
	const string &  GetFileName() const { return file_name; }
	const string &  GetMotionName() const { return motion_name; }

	// �K�w�\���̏��̎擾
	const int       GetNumJoint() const { return  joints.size(); }
	const Joint *   GetJoint( int no ) const { return  joints[no]; }
	const int       GetNumChannel() const { return  channels.size(); }
	const Channel * GetChannel( int no ) const { return  channels[no]; }

	const Joint *   GetJoint( const string & j ) const  {
		map< string, Joint * >::const_iterator  i = joint_index.find( j );
		return  ( i != joint_index.end() ) ? (*i).second : NULL; }
	const Joint *   GetJoint( const char * j ) const  {
		map< string, Joint * >::const_iterator  i = joint_index.find( j );
		return  ( i != joint_index.end() ) ? (*i).second : NULL; }
	//�t���[�������Ɋւ�����̎擾
	
	// ���[�V�����f�[�^�̏��̎擾
	int     GetNumFrame() const { return  num_frame; }
	double  GetInterval() const { return  interval; }
	double  GetMotion( int f, int c ) const { return  motion[ f*num_channel + c ]; }

	// ���[�V�����f�[�^�̏��̕ύX
	void  SetMotion( int f, int c, double v ) { motion[ f*num_channel + c ] = v; }
	
  public:
	/*  �p���̕`��֐�  */
	
	// �w��t���[���̎p����`��
	void  RenderFigure( int frame_no, float scale = 0.4f );

	// �w�肳�ꂽBVH���i�E�p����`��i�N���X�֐��j
	static void  RenderFigure( const Joint * root, const double * data, float scale = 0.4f );

	// BVH���i�̂P�{�̃����N��`��i�N���X�֐��j
	static void  RenderBone( float x0, float y0, float z0, float x1, float y1, float z1 );

 public:
	 /*�t���[������*/
	 void FramePartition(vector <int>& a);
	// void PartitionJointDetection(const Joint *p_root);//void PartitionJointDetection(const Joint *p_root, Joint *   b_joint);//�t���[���������s�����߂ɗp����֐ߏ����Z�o����
	 static void create_rotation_matrix(RotationMatrix *m,const quat *q);
	 float AngleOf2Vector(quat*, quat*);
	 float get_vector_length(quat* );
	 double dot_product(quat* vl, quat* vr);
	 void out_of_file();//�t���[���Ԃł̉�]�p�x
	 void out_of_file2();//1�t���[���ڂɑ΂����]�p�x
	 void out_of_file3();//���ڊ֐߂̉�]�p�x
	 void out_of_file4();//�N�H�[�^�j�I����
	 void out_of_file5();//�N�H�[�^�j�I�����������_�Q�_�ȉ��Ŏl�̌ܓ�
	 
	 void out_of_file_spine();
	 void out_of_file_leftarm();
	 void out_of_file_rightarm();
	 void out_of_file_leftleg();
	 void out_of_file_rightleg();
	 void out_of_file_5joint30();
	 void out_of_file_sita_sum();
	 void out_of_file_sita_sumframe();
		
	 vector<Point3f> end_site;
};



#endif // _BVH_H_
