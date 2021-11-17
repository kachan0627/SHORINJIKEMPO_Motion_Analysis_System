#ifndef __QUAT_H
#define __QUAT_H

/* クォータニオン構造体 */
struct QUAT {
	float w, x, y, z;
};
typedef struct QUAT quat;

/* 代入 */
/* a = 0 */
void quat_zero(quat *a);
/* a = 1 */
void quat_identity(quat *a);
/* a = (w, x, y, z) */
void quat_assign(quat *a, float w, float x, float y, float z);

/* クォータニオンどうしの足し算，引き算，掛け算 */
/* a = b + c */
void quat_add(quat *a, const quat *b, const quat *c);
/* a = b - c */
void quat_sub(quat *a, const quat *b, const quat *c);
/* a = b * c */
void quat_mul(quat *a, const quat *b, const quat *c);

/* クォータニオンと実数の掛け算，割り算 */
/* a = s * b */
void quat_mul_real(quat *a, float s, const quat *b);
/* a = b / s */
void quat_div_real(quat *a, const quat *b, float s);

/* クォータニオンのノルム */
/* ||a||^2 */
float quat_norm_sqr(const quat *a);
/* ||a|| */
float quat_norm(const quat *a);

#endif
