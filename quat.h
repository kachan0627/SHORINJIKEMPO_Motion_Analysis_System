#ifndef __QUAT_H
#define __QUAT_H

/* �N�H�[�^�j�I���\���� */
struct QUAT {
	float w, x, y, z;
};
typedef struct QUAT quat;

/* ��� */
/* a = 0 */
void quat_zero(quat *a);
/* a = 1 */
void quat_identity(quat *a);
/* a = (w, x, y, z) */
void quat_assign(quat *a, float w, float x, float y, float z);

/* �N�H�[�^�j�I���ǂ����̑����Z�C�����Z�C�|���Z */
/* a = b + c */
void quat_add(quat *a, const quat *b, const quat *c);
/* a = b - c */
void quat_sub(quat *a, const quat *b, const quat *c);
/* a = b * c */
void quat_mul(quat *a, const quat *b, const quat *c);

/* �N�H�[�^�j�I���Ǝ����̊|���Z�C����Z */
/* a = s * b */
void quat_mul_real(quat *a, float s, const quat *b);
/* a = b / s */
void quat_div_real(quat *a, const quat *b, float s);

/* �N�H�[�^�j�I���̃m���� */
/* ||a||^2 */
float quat_norm_sqr(const quat *a);
/* ||a|| */
float quat_norm(const quat *a);

#endif
