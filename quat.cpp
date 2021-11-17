#include <math.h>
#include "quat.h"

/* 代入 */

/* a = 0 */
void quat_zero(quat *a)
{
	a->w = a->x = a->y = a->z = 0.0;
}

/* a = 1 */
void quat_identity(quat *a)
{
	a->w = 1.0;
	a->x = a->y = a->z = 0.0;
}

/* a = (w, x, y, z) */
void quat_assign(quat *a, float w, float x, float y, float z)
{
	a->w = w;
	a->x = x;
	a->y = y;
	a->z = z;
}

/* クォータニオンどうしの足し算，引き算，掛け算 */

/* a = b + c */
void quat_add(quat *a, const quat *b, const quat *c)
{
	a->w = b->w + c->w;
	a->x = b->x + c->x;
	a->y = b->y + c->y;
	a->z = b->z + c->z;
}

/* a = b - c */
void quat_sub(quat *a, const quat *b, const quat *c)
{
	a->w = b->w - c->w;
	a->x = b->x - c->x;
	a->y = b->y - c->y;
	a->z = b->z - c->z;
}

/* a = b * c */
void quat_mul(quat *a, const quat *b, const quat *c)
{
	a->w = b->w * c->w - b->x * c->x - b->y * c->y - b->z * c->z;
	a->x = b->w * c->x + b->x * c->w - b->y * c->z + b->z * c->y;
	a->y = b->w * c->y + b->x * c->z + b->y * c->w - b->z * c->x;
	a->z = b->w * c->z - b->x * c->y + b->y * c->x + b->z * c->w;
}

/* クォータニオンと実数の掛け算，割り算 */

/* a = s * b */
void quat_mul_real(quat *a, float s, const quat *b)
{
	a->w = s * b->w;
	a->x = s * b->x;
	a->y = s * b->y;
	a->z = s * b->z;
}

/* a = b / s */
void quat_div_real(quat *a, const quat *b, float s)
{
	a->w = b->w / s;
	a->x = b->x / s;
	a->y = b->y / s;
	a->z = b->z / s;
}

/* クォータニオンのノルム */

/* ||a||^2 */
float quat_norm_sqr(const quat *a)
{
	return a->w * a->w + a->x * a->x + a->y * a->y + a->z * a->z;
}

/* ||a|| */
float quat_norm(const quat *a)
{
	return sqrt(quat_norm_sqr(a));
}
