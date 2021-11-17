#pragma once
//#include<gl/gl.h>
#include <GL/glut.h>
class CQuaternion
{
public:
	CQuaternion(void);
	~CQuaternion(void);
	// ��]��\���N�H�[�^�j�I��
	float Quaternion_U[4];
private:
	// �������֐�
	void InitQuaternion(void);
	const double PI = 3.14159265;	//�~����
public:
	// �N�H�[�^�j�I���̃R���X�g���N�^
	CQuaternion(float angle, float* axis);
	// ��]�s��
	float rotate_matrix[16];
	// ��]�s�������
	void creat_matrix(void);
	void SetValue(float in_real, float in_x, float in_y, float in_z);
	// �S�~�S�̍s����R�~�R�ɒu��������
	void Relocation_Matrix(float* re_matrix);
private:
	// �����N�H�[�^�j�I��
	float Quaternion_Conj[4];
public:
	// �N�H�[�^�ɃI���̋t��
	float Quaternion_Inverse[4];
	// �N�H�[�^�j�I���̋t�������߂�
	void GetInverseQuaternion(void);
private:
	// �����N�H�[�^�j�I�������߂�
	void SetQuaternion_Conj(void);
	// �N�H�[�^�j�I���̃m�����i�傫���j��2������߂�
	float GetQuaternionNorm(void);
public:
	// �N�H�[�^�j�I���̊|���Z(Quaternion_a * Quaternion_b)
	void Multiplication_Quaternion(float Quaternion_a[4], float Quaternion_b[4], float Quaternion_m[4]);
	// q_base/q_emotion��萶�������N�H�[�^�j�I���N���X�𐶐�����R���X�g���N�^
	CQuaternion(CQuaternion q_base, CQuaternion q_emotion);
	//�N�H�[�^�j�I���̈ꎞ���p�Ɏg�p
	void SetQuaternion(float angle, float* axis);
	// �V�K�쐬
	void InitAllClass(void);
	// ��]�s����N�H�[�^�j�I���ɕϊ����ăZ�b�g����
	void QuatConvertFromMatrix(float in_matrix[16]);
	// 2�̃N�H�[�^�j�I����in_weight�ŕ�Ԃ���
	void QuatSlerp(CQuaternion in_qua1, CQuaternion in_qua2, float in_weight);
	void MakeQuat(float r[3]);
	// �s�񂩂�N�H�[�^�j�I���ւ̕ϊ�
	void MatrixToQuaternion(float * matrix, float * quaternion);
	// ���ʐ��`���
	void SLERPwithQuaternion(float * quaternion_A, float * quaternion_B, float * out_quaternion, float weight);
	// �N�H�[�^�j�I���̐��K��
	void NormalizeQuaternion(float * quaternion);
	// �N�I�[�^�j�I������s��ւ̕ϊ�
	void QuaternionToMatrix(float * quaternion, float * matrix);
};
