#ifndef XLIB_MATH
#define XLIB_MATH

#define PIf 3.14159265358979323846264338327950288   /* pi          */
#define PI2f 1.57079632679489661923132169163975144   /* pi/2        */
#define PI4f 0.785398163397448309615660845819875721  /* pi/4        */

#define max(a, b) (((a)>(b))?(a):(b))

/* =========================================================================
   DATA TYPES
   ========================================================================= */

typedef f32 v2[2];
typedef s32 v2i[2];
typedef f32 v3[3];
typedef s32 v3i[3];
typedef f32 v4[4];
typedef s32 v4i[4];
typedef v4 mat4[4];
typedef v4 versor;

typedef struct Rect2
{
    v2 min;
    v2 max;
} Rect2;

typedef struct Rect3
{
    v3 min;
    v3 max;
} Rect3;

typedef struct Circle
{
    v2 center;
    f32 radius;
} Circle;

typedef struct Ngon
{
    s32 n;
    v2 *vertices;
} Ngon;

#include "xmemory.h"

/* =========================================================================
   FLOAT
   ========================================================================= */

f32 f32_map  (f32 a1, f32 a2, f32 s, f32 b1, f32 b2);
f32 f32_lerp (f32 a, f32 t, f32 b);
f32 f32_d2r  (f32 deg);
f32 f32_r2d  (f32 rad);

/* =========================================================================
   RANDOM
   ========================================================================= */

void random_seed(u32 seed);
s32 s32_rand (s32 a, s32 b);
f32 f32_rand (f32 a, f32 b);

/* =========================================================================
   VECTOR 2 FLOAT
   ========================================================================= */

void v2_copy      (v2 source, v2 dest);
void v2_bezier    (f32 x1, f32 y1, f32 x2, f32 y2, f32 t, v2 dest);
void v2_lerp      (v2 a, f32 t, v2 b, v2 dest);
void v2_mul       (f32 k, v2 a, v2 dest);
void v2_add       (v2 a, v2 b, v2 dest);
void v2_sub       (v2 a, v2 b, v2 dest);
void v2_had       (v2 a, v2 b, v2 dest);
void v2_rand      (v2 a, v2 b, v2 dest);
void v2_rotate    (v2 a, f32 t, v2 dest);
void v2_neg       (v2 a, v2 dest);
void v2_normalize (v2 a);
f32  v2_dot       (v2 a, v2 b);
f32  v2_length2   (v2 a);
f32  v2_length    (v2 a);

/* =========================================================================
   VECTOR 2 INT
   ========================================================================= */

void v2i_lerp      (v2i a, f32 t, v2i b, v2i dest);
void v2i_mul       (f32 k, v2i a, v2i dest);
void v2i_add       (v2i a, v2i b, v2i dest);
void v2i_sub       (v2i a, v2i b, v2i dest);
void v2i_rand      (v2i a, v2i b, v2i dest);
void v2i_normalize (v2i a);
f32  v2i_dot       (v2i a, v2i b);
f32  v2i_length2   (v2i a);
f32  v2i_length    (v2i a);

/* =========================================================================
   VECTOR 3 FLOAT
   ========================================================================= */

void v3_copy      (v3 source, v3 dest);
void v3_lerp      (v3 a, f32 t, v3 b, v3 dest);
void v3_mul       (f32 k, v3 a, v3 dest);
void v3_add       (v3 a, v3 b, v3 dest);
void v3_sub       (v3 a, v3 b, v3 dest);
void v3_rand      (v3 a, v3 b, v3 dest);
void v3_cross     (v3 a, v3 b, v3 dest);
void v3_crossn    (v3 a, v3 b, v3 dest);
void v3_normalize (v3 a);
f32  v3_dot       (v3 a, v3 b);
f32  v3_length2   (v3 a);
f32  v3_length    (v3 a);

/* =========================================================================
   VECTOR 3 INT
   ========================================================================= */

void v3i_lerp      (v3i a, f32 t, v3i b, v3i dest);
void v3i_add       (v3i a, v3i b, v3i dest);
void v3i_sub       (v3i a, v3i b, v3i dest);
void v3i_mul       (f32 k, v3i a, v3i dest);
void v3i_normalize (v3i a);
void v3i_rand      (v3i a, v3i b, v3i dest);
f32  v3i_dot       (v3i a, v3i b);
f32  v3i_length2   (v3i a);
f32  v3i_length    (v3i a);

/* =========================================================================
   VECTOR 4 FLOAT
   ========================================================================= */

void v4_copy      (v4 source, v4 dest);
void v4_lerp      (v4 a, f32 t, v4 b, v4 dest);
void v4_mul       (f32 k, v4 a, v4 dest);
void v4_add       (v4 a, v4 b, v4 dest);
void v4_sub       (v4 a, v4 b, v4 dest);
void v4_rand      (v4 a, v4 b, v4 dest);
void v4_cross     (v4 a, v4 b, v4 dest);
void v4_crossn    (v4 a, v4 b, v4 dest);
void v4_normalize (v4 a);
f32  v4_dot       (v4 a, v4 b);
f32  v4_length2   (v4 a);
f32  v4_length    (v4 a);

/* =========================================================================
   VECTOR 4 INT
   ========================================================================= */

void v4i_lerp      (v4i a, f32 t, v4i b, v4i dest);
void v4i_add       (v4i a, v4i b, v4i dest);
void v4i_sub       (v4i a, v4i b, v4i dest);
void v4i_mul       (f32 k, v4i a, v4i dest);
void v4i_normalize (v4i a);
f32  v4i_dot       (v4i a, v4i b);
f32  v4i_length2   (v4i a);
f32  v4i_length    (v4i a);

/* =========================================================================
    COLORS
   ========================================================================= */

typedef enum {
    NullColor,
    White,
    Black,
    Gray,
    LightGray,
    LighterGray,
    Blue,
    Sapphire,
    Cobalt,
    Cerulean,
    Azure,
    Indigo,
    Turquoise,
    Red,
    Crimson,
    Ruby,
    Scarlet,
    Vermilion,
    Green,
    Emerald,
    Jade,
    Olive,
    Lime,
    Yellow,
    Gold,
    Amber,
    Lemon,
    Mustard,
} Color;

v4 xcolor[] = {
    {0.00f, 0.00f, 0.00f, 0.00f}, // null
    {1.00f, 1.00f, 1.00f, 1.00f}, // white
    {0.00f, 0.00f, 0.00f, 1.00f}, // black
    {0.50f, 0.50f, 0.50f, 1.00f}, // gray
    {0.75f, 0.75f, 0.75f, 1.00f}, // light gray
    {0.90f, 0.90f, 0.90f, 1.00f}, // lighter gray
    {0.00f, 0.00f, 1.00f, 1.00f}, // blue
    {0.06f, 0.32f, 0.73f, 1.00f}, // sapphire
    {0.00f, 0.28f, 0.67f, 1.00f}, // cobalt
    {0.00f, 0.48f, 0.65f, 1.00f}, // cerulean
    {0.00f, 0.50f, 1.00f, 1.00f}, // azure
    {0.29f, 0.00f, 0.51f, 1.00f}, // indigo
    {0.25f, 0.88f, 0.82f, 1.00f}, // turquoise
    {1.00f, 0.00f, 0.00f, 1.00f}, // red
    {0.86f, 0.08f, 0.24f, 1.00f}, // crimson
    {0.88f, 0.07f, 0.37f, 1.00f}, // ruby
    {1.00f, 0.14f, 0.00f, 1.00f}, // scarlet
    {1.00f, 0.28f, 0.00f, 1.00f}, // vermilion
    {0.00f, 1.00f, 0.00f, 1.00f}, // green
    {0.31f, 0.78f, 0.47f, 1.00f}, // emerald
    {0.00f, 0.66f, 0.42f, 1.00f}, // jade
    {0.50f, 0.50f, 0.00f, 1.00f}, // olive
    {0.75f, 1.00f, 0.00f, 1.00f}, // lime
    {1.00f, 1.00f, 0.00f, 1.00f}, // yellow
    {1.00f, 0.84f, 0.00f, 1.00f}, // gold
    {1.00f, 0.75f, 0.00f, 1.00f}, // amber
    {1.00f, 0.97f, 0.00f, 1.00f}, // lemon
    {0.96f, 0.86f, 0.35f, 1.00f}, // mustard
};

/* =========================================================================
   RECT 2 FLOAT
   ========================================================================= */

Rect2 rect2_min_dim    (v2 min, v2 dim);
Rect2 rect2_center_dim (v2 center, v2 dim);
void  rect2_dim        (Rect2 rect, v2 dest);
void  rect2_center     (Rect2 rect, v2 dest);
void  rect2_p          (Rect2 rect, v2 dest);

/* =========================================================================
   RECT 3 FLOAT
   ========================================================================= */

Rect3 rect3_min_dim    (v3 min, v3 dim);
Rect3 rect3_center_dim (v3 center, v3 dim);
void  rect3_dim        (Rect3 rect, v3 dest);
void  rect3_center     (Rect3 rect, v3 dest);

/* =========================================================================
   COLLISION DETECTION
   ========================================================================= */

bool line_vs_line(v2 p1, v2 p2, v2 q1, v2 q2, v2 intersection);
bool circle_vs_circle(Circle a, Circle b);
f32  circle_vs_circle_dist(Circle a, Circle b);
void circle_vs_circle_witness(Circle a, Circle b, v2 p1, v2 p2);
bool circle_vs_ngon(Circle circle, Ngon polygon);
bool point_vs_rect2(v2 p, Rect2 rect);







/* =========================================================================
    INTENDED WHITE SPACE
   ========================================================================= */









/* =========================================================================
   IMPLEMENTATION
   ========================================================================= */

/* =========================================================================
   FLOAT
   ========================================================================= */

inline f32 
f32_square(f32 value) {
    return value*value;
}

void
random_seed(u32 seed) {
    if (seed == 0)
        srand((u32)time(0));
    else
        srand(seed);
}

inline s32
s32_rand(s32 a, s32 b) {
    return (rand() % (b - a + 1)) + a;
}

inline f32
f32_rand(f32 a, f32 b) {
    return (float)rand() / RAND_MAX * (b - a) + a;
}

inline f32
f32_map(f32 a1, f32 a2, f32 s, f32 b1, f32 b2) {
    assert(a2!=a1);
    return (b1+(s-a1)*((b2-b1)/(a2-a1)));
}

inline f32
f32_lerp(f32 a, f32 t, f32 b) {
    return (1-t)*a+t*b;
}

inline f32
f32_d2r(f32 deg) {
    return deg*PIf/180.0f;
}

inline f32
f32_r2d(f32 rad) {
    return rad*180.0f/PIf;
}

/* =========================================================================
   VECTOR 2 FLOAT
   ========================================================================= */

void
v2_copy(v2 source, v2 dest) {
    dest[0] = source[0];
    dest[1] = source[1];
}

inline void
v2_bezier(f32 x1, f32 y1, f32 x2, f32 y2, f32 t, v2 dest) {
    v2 p0 = {0,0};
    v2 p1 = {x1,y1};
    v2 p2 = {x2,y2};
    v2 p3 = {1,1};
    
    f32 u = 1.0f - t;
    f32 tt = t * t;
    f32 uu = u * u;
    f32 uuu = uu * u;
    f32 ttt = tt * t;
    v2 p = {
        uuu * p0[0] + 3.0f * uu * t * p1[0] + 3.0f * u * tt * p2[0] + ttt * p3[0],
        uuu * p0[1] + 3.0f * uu * t * p1[1] + 3.0f * u * tt * p2[1] + ttt * p3[1]
    };
    v2_copy(p, dest);
}

inline void
v2_add(v2 a, v2 b, v2 dest) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
}

inline void
v2_sub(v2 a, v2 b, v2 dest) {
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
}

inline void
v2_neg(v2 v, v2 dest) {
    dest[0] = -v[0];
    dest[1] = -v[1];
}

inline void
v2_mul(f32 k, v2 a, v2 dest) {
    dest[0] = a[0]*k;
    dest[1] = a[1]*k;
}

inline f32
v2_dot(v2 a, v2 b) {
    f32 result = a[0]*b[0] + a[1]*b[1];
    return result;
}

inline f32
v2_length2(v2 a) {
    return v2_dot(a, a);
}

inline f32
v2_length(v2 a) {
    return sqrtf(v2_length2(a));
}

inline void
v2_had(v2 a, v2 b, v2 dest) {
    dest[0] = a[0]*b[0];
    dest[1] = a[1]*b[1];
}

inline void
v2_normalize(v2 a) {
    v2 temp;
    v2_copy(a, temp);
    assert(v2_length2(temp) != 0);
    
    v2 temp2;
    v2_mul(1.0f/v2_length(temp), temp, temp2);
    v2_copy(temp2, a);
}

inline void
v2_lerp(v2 a, f32 t, v2 b, v2 dest) {
    dest[0] = f32_lerp(a[0], t, b[0]);
    dest[1] = f32_lerp(a[1], t, b[1]);
}

inline void
v2_rand(v2 a, v2 b, v2 dest) {
    dest[0] = f32_rand(a[0], b[0]);
    dest[1] = f32_rand(a[1], b[1]);
}

inline void
v2_rotate(v2 v, f32 a, v2 dest)
{
    a = f32_d2r(a);
    dest[0] = cosf(a) * v[0] - sinf(a) * v[1];
    dest[1] = sinf(a) * v[0] + cosf(a) * v[1];
}

/* =========================================================================
   VECTOR 2 INT
   ========================================================================= */

inline void
v2i_add(v2i a, v2i b, v2i dest) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
}

inline void
v2i_sub(v2i a, v2i b, v2i dest) {
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
}

inline void
v2i_rand(v2i a, v2i b, v2i dest) {
    dest[0] = s32_rand(a[0], b[0]);
    dest[1] = s32_rand(a[1], b[1]);
}

/* =========================================================================
   VECTOR 3 FLOAT
   ========================================================================= */

inline void
v3_copy(v3 source, v3 dest) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[2];
}

inline void
v3_add(v3 a, v3 b, v3 dest) {
    dest[0] = a[0] + b[0];
    dest[1] = a[1] + b[1];
    dest[2] = a[2] + b[2];
}

inline void
v3_sub(v3 a, v3 b, v3 dest)
{
    dest[0] = a[0] - b[0];
    dest[1] = a[1] - b[1];
    dest[2] = a[2] - b[2];
}

inline void
v3_neg(v3 v, v3 dest)
{
    dest[0] = -v[0];
    dest[1] = -v[1];
    dest[2] = -v[2];
}

inline void
v3_mul(f32 k, v3 a, v3 dest)
{
    dest[0] = a[0]*k;
    dest[1] = a[1]*k;
    dest[2] = a[2]*k;
}

inline f32
v3_dot(v3 a, v3 b) {
    f32 result = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    return result;
}

inline f32
v3_length2(v3 a) {
    return v3_dot(a, a);
}

inline f32 
v3_length(v3 a) {
    return sqrtf(v3_length2(a));
}

inline void
v3_had(v3 a, v3 b, v3 dest) {
    dest[0] = a[0]*b[0];
    dest[1] = a[1]*b[1];
    dest[2] = a[2]*b[2];
}

inline void
v3_cross(v3 a, v3 b, v3 dest) {
    dest[0] = a[1] * b[2] - a[2] * b[1];
    dest[1] = a[2] * b[0] - a[0] * b[2];
    dest[2] = a[0] * b[1] - a[1] * b[0];
}

inline void
v3_normalize(v3 a) {
    v3 temp;
    v3_copy(a, temp);
    assert(v3_length2(temp) != 0);
    
    v3 temp2;
    v3_mul(1.0f/v3_length(temp), temp, temp2);
    
    v3_copy(temp2, a);
}

inline void
v3_crossn(v3 a, v3 b, v3 dest) {
    v3_cross(a, b, dest);
    v3_normalize(dest);
}

inline void
v3_lerp(v3 a, f32 t, v3 b, v3 dest) {
    dest[0] = f32_lerp(a[0], t, b[0]);
    dest[1] = f32_lerp(a[1], t, b[1]);
    dest[2] = f32_lerp(a[2], t, b[2]);
}

inline void
v3_rand(v3 a, v3 b, v3 dest) {
    dest[0] = f32_rand(a[0], b[0]);
    dest[1] = f32_rand(a[1], b[1]);
    dest[2] = f32_rand(a[2], b[2]);
}

/* =========================================================================
   VECTOR 4 FLOAT
   ========================================================================= */

inline void
v4_copy(v4 source, v4 dest) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[2];
    dest[3] = source[3];
}

inline void 
v4_lerp(v4 a, f32 t, v4 b, v4 dest) {
    dest[0] = f32_lerp(a[0], t, b[0]);
    dest[1] = f32_lerp(a[1], t, b[1]);
    dest[2] = f32_lerp(a[2], t, b[2]);
    dest[3] = f32_lerp(a[3], t, b[3]);
}

inline void
v4_rand(v4 a, v4 b, v4 dest) {
    dest[0] = f32_rand(a[0], b[0]);
    dest[1] = f32_rand(a[1], b[1]);
    dest[2] = f32_rand(a[2], b[2]);
    dest[3] = f32_rand(a[3], b[3]);
}

inline f32
v4_dot(v4 a, v4 b) {
    f32 result = a[0]*b[0] + 
        a[1]*b[1] + 
        a[2]*b[2] + 
        a[3]*b[3];
    return result;
}

inline f32
v4_length2(v4 a) {
    return v4_dot(a, a);
}

inline f32
v4_length(v4 a) {
    return sqrtf(v4_length2(a));
}

inline void 
v4_mul(f32 k, v4 a, v4 dest) {
    dest[0] = a[0]*k;
    dest[1] = a[1]*k;
    dest[2] = a[2]*k;
    dest[3] = a[3]*k;
}

inline void
v4_normalize(v4 a) {
    v4 temp;
    v4_copy(a, temp);
    
    v4 temp2;
    v4_mul(1.0f/v4_length(temp), temp, temp2);
    
    v4_copy(temp2, a);
}

/* =========================================================================
   RECT 2 FLOAT
   ========================================================================= */

inline Rect2
rect2_center_dim(v2 center, v2 dim) {
    Rect2 r = {
        {center[0] - 0.5f*dim[0], center[1] - 0.5f*dim[1]},
        {center[0] + 0.5f*dim[0], center[1] + 0.5f*dim[1]},
    };
    return r;
}

inline Rect2
rect2_min_max(v2 min, v2 max) {
    Rect2 r = {
        {min[0], min[1]},
        {max[0], max[1]},
    };
    return r;
}

inline Rect2
rect2_min_dim(v2 min, v2 dim) {
    Rect2 r = {
        {min[0], min[1]},
        {min[0] + dim[0], min[1] + dim[1]},
    };
    return r;
}

inline void
rect2_dim(Rect2 rect, v2 dest) {
    v2_sub(rect.max, rect.min, dest);
}

inline void
rect2_center(Rect2 rect, v2 dest) {
    v2 dim;
    v2_sub(rect.max, rect.min, dim);
    
    v2 halfDim;
    v2_mul(0.5f, dim, halfDim);
    
    v2_add(rect.min, halfDim, dest);
}

/* =========================================================================
   RECT 2 FLOAT
   ========================================================================= */

inline Rect3
rect3_center_dim(v3 center, v3 dim)
{
    Rect3 r = {
        {center[0]-0.5f*dim[0], center[1]-0.5f*dim[1], center[2]-0.5f*dim[2]},
        {center[0]+0.5f*dim[0], center[1]+0.5f*dim[1], center[2]+0.5f*dim[2]},
    };
    return r;
}

inline Rect3
rect3_min_max(v3 min, v3 max) {
    Rect3 r = {
        {min[0], min[1], min[2]},
        {max[0], max[1], max[2]},
    };
    return r;
}

inline Rect3
rect3_min_dim(v3 min, v3 dim) {
    Rect3 r = {
        {min[0], min[1], min[2]},
        {min[0]+dim[0], min[1]+dim[1], min[2]+dim[2]},
    };
    return r;
}

inline void
rect3_dim(Rect3 rect, v3 dest) {
    v3_sub(rect.max, rect.min, dest);
}

inline void
rect3_center(Rect3 rect, v3 dest) {
    v3 dim;
    v3_sub(rect.max, rect.min, dim);
    
    v3 halfDim;
    v3_mul(0.5f, dim, halfDim);
    
    v3_add(rect.min, halfDim, dest);
}

/* =========================================================================
      MATRIX 4x4
   ========================================================================= */

void
mat4_mulv(mat4 m, v4 v, v4 dest) {
    v4 res;
    res[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    res[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    res[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    res[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
    v4_copy(res, dest);
}

void
mat4_mul(mat4 m1, mat4 m2, mat4 dest) {
    float a00 = m1[0][0], a01 = m1[0][1], a02 = m1[0][2], a03 = m1[0][3],
    a10 = m1[1][0], a11 = m1[1][1], a12 = m1[1][2], a13 = m1[1][3],
    a20 = m1[2][0], a21 = m1[2][1], a22 = m1[2][2], a23 = m1[2][3],
    a30 = m1[3][0], a31 = m1[3][1], a32 = m1[3][2], a33 = m1[3][3],
    
    b00 = m2[0][0], b01 = m2[0][1], b02 = m2[0][2], b03 = m2[0][3],
    b10 = m2[1][0], b11 = m2[1][1], b12 = m2[1][2], b13 = m2[1][3],
    b20 = m2[2][0], b21 = m2[2][1], b22 = m2[2][2], b23 = m2[2][3],
    b30 = m2[3][0], b31 = m2[3][1], b32 = m2[3][2], b33 = m2[3][3];
    
    dest[0][0] = a00 * b00 + a10 * b01 + a20 * b02 + a30 * b03;
    dest[0][1] = a01 * b00 + a11 * b01 + a21 * b02 + a31 * b03;
    dest[0][2] = a02 * b00 + a12 * b01 + a22 * b02 + a32 * b03;
    dest[0][3] = a03 * b00 + a13 * b01 + a23 * b02 + a33 * b03;
    dest[1][0] = a00 * b10 + a10 * b11 + a20 * b12 + a30 * b13;
    dest[1][1] = a01 * b10 + a11 * b11 + a21 * b12 + a31 * b13;
    dest[1][2] = a02 * b10 + a12 * b11 + a22 * b12 + a32 * b13;
    dest[1][3] = a03 * b10 + a13 * b11 + a23 * b12 + a33 * b13;
    dest[2][0] = a00 * b20 + a10 * b21 + a20 * b22 + a30 * b23;
    dest[2][1] = a01 * b20 + a11 * b21 + a21 * b22 + a31 * b23;
    dest[2][2] = a02 * b20 + a12 * b21 + a22 * b22 + a32 * b23;
    dest[2][3] = a03 * b20 + a13 * b21 + a23 * b22 + a33 * b23;
    dest[3][0] = a00 * b30 + a10 * b31 + a20 * b32 + a30 * b33;
    dest[3][1] = a01 * b30 + a11 * b31 + a21 * b32 + a31 * b33;
    dest[3][2] = a02 * b30 + a12 * b31 + a22 * b32 + a32 * b33;
    dest[3][3] = a03 * b30 + a13 * b31 + a23 * b32 + a33 * b33;
}

f32 
mat4_det(mat4 mat) {
    /* [square] det(A) = det(At) */
    float t[6];
    float a = mat[0][0], b = mat[0][1], c = mat[0][2], d = mat[0][3],
    e = mat[1][0], f = mat[1][1], g = mat[1][2], h = mat[1][3],
    i = mat[2][0], j = mat[2][1], k = mat[2][2], l = mat[2][3],
    m = mat[3][0], n = mat[3][1], o = mat[3][2], p = mat[3][3];
    
    t[0] = k * p - o * l;
    t[1] = j * p - n * l;
    t[2] = j * o - n * k;
    t[3] = i * p - m * l;
    t[4] = i * o - m * k;
    t[5] = i * n - m * j;
    
    return a * (f * t[0] - g * t[1] + h * t[2])
        - b * (e * t[0] - g * t[3] + h * t[4])
        + c * (e * t[1] - f * t[3] + h * t[5])
        - d * (e * t[2] - f * t[4] + g * t[5]);
}

void
mat4_scale_p(mat4 m, float s) {
    m[0][0] *= s; m[0][1] *= s; m[0][2] *= s; m[0][3] *= s;
    m[1][0] *= s; m[1][1] *= s; m[1][2] *= s; m[1][3] *= s;
    m[2][0] *= s; m[2][1] *= s; m[2][2] *= s; m[2][3] *= s;
    m[3][0] *= s; m[3][1] *= s; m[3][2] *= s; m[3][3] *= s;
}

void 
mat4_inv(mat4 mat, mat4 dest) {
    float t[6];
    float det;
    float a = mat[0][0], b = mat[0][1], c = mat[0][2], d = mat[0][3],
    e = mat[1][0], f = mat[1][1], g = mat[1][2], h = mat[1][3],
    i = mat[2][0], j = mat[2][1], k = mat[2][2], l = mat[2][3],
    m = mat[3][0], n = mat[3][1], o = mat[3][2], p = mat[3][3];
    
    t[0] = k * p - o * l; t[1] = j * p - n * l; t[2] = j * o - n * k;
    t[3] = i * p - m * l; t[4] = i * o - m * k; t[5] = i * n - m * j;
    
    dest[0][0] =  f * t[0] - g * t[1] + h * t[2];
    dest[1][0] =-(e * t[0] - g * t[3] + h * t[4]);
    dest[2][0] =  e * t[1] - f * t[3] + h * t[5];
    dest[3][0] =-(e * t[2] - f * t[4] + g * t[5]);
    
    dest[0][1] =-(b * t[0] - c * t[1] + d * t[2]);
    dest[1][1] =  a * t[0] - c * t[3] + d * t[4];
    dest[2][1] =-(a * t[1] - b * t[3] + d * t[5]);
    dest[3][1] =  a * t[2] - b * t[4] + c * t[5];
    
    t[0] = g * p - o * h; t[1] = f * p - n * h; t[2] = f * o - n * g;
    t[3] = e * p - m * h; t[4] = e * o - m * g; t[5] = e * n - m * f;
    
    dest[0][2] =  b * t[0] - c * t[1] + d * t[2];
    dest[1][2] =-(a * t[0] - c * t[3] + d * t[4]);
    dest[2][2] =  a * t[1] - b * t[3] + d * t[5];
    dest[3][2] =-(a * t[2] - b * t[4] + c * t[5]);
    
    t[0] = g * l - k * h; t[1] = f * l - j * h; t[2] = f * k - j * g;
    t[3] = e * l - i * h; t[4] = e * k - i * g; t[5] = e * j - i * f;
    
    dest[0][3] =-(b * t[0] - c * t[1] + d * t[2]);
    dest[1][3] =  a * t[0] - c * t[3] + d * t[4];
    dest[2][3] =-(a * t[1] - b * t[3] + d * t[5]);
    dest[3][3] =  a * t[2] - b * t[4] + c * t[5];
    
    det = 1.0f / (a * dest[0][0] + b * dest[1][0]
                  + c * dest[2][0] + d * dest[3][0]);
    
    mat4_scale_p(dest, det);
}

void
mat4_zero(mat4 dest) {
    dest[0][0] = 0;
    dest[0][1] = 0;
    dest[0][2] = 0;
    dest[0][3] = 0;
    dest[1][0] = 0;
    dest[1][1] = 0;
    dest[1][2] = 0;
    dest[1][3] = 0;
    dest[2][0] = 0;
    dest[2][1] = 0;
    dest[2][2] = 0;
    dest[2][3] = 0;
    dest[3][0] = 0;
    dest[3][1] = 0;
    dest[3][2] = 0;
    dest[3][3] = 0;
}

void
mat4_ortho_rh(f32 left, f32 right, f32 bottom, f32 top,
              f32 nearZ, f32 farZ, mat4 dest) {
    float rl, tb, fn;
    
    mat4_zero(dest);
    
    rl = 1.0f / (right  - left);
    tb = 1.0f / (top    - bottom);
    fn =-1.0f / (farZ - nearZ);
    
    dest[0][0] = 2.0f * rl;
    dest[1][1] = 2.0f * tb;
    dest[2][2] = 2.0f * fn;
    dest[3][0] =-(right  + left)    * rl;
    dest[3][1] =-(top    + bottom)  * tb;
    dest[3][2] = (farZ + nearZ) * fn;
    dest[3][3] = 1.0f;
}

void
mat4_perspective_rh(f32 fovy, f32 aspect, f32 nearZ, f32 farZ, mat4 dest) {
    f32 f, fn;
    
    mat4_zero(dest);
    
    f  = 1.0f / tanf(fovy * 0.5f);
    fn = 1.0f / (nearZ - farZ);
    
    dest[0][0] = f / aspect;
    dest[1][1] = f;
    dest[2][2] = (nearZ + farZ) * fn;
    dest[2][3] =-1.0f;
    dest[3][2] = 2.0f * nearZ * farZ * fn;
}

void
mat4_lookat_rh(v3 eye, v3 center, v3 up, mat4 dest) {
    v3 f, u, s;
    
    v3_sub(center, eye, f);
    v3_normalize(f);
    
    v3_crossn(f, up, s);
    v3_cross(s, f, u);
    
    dest[0][0] = s[0];
    dest[0][1] = u[0];
    dest[0][2] =-f[0];
    dest[1][0] = s[1];
    dest[1][1] = u[1];
    dest[1][2] =-f[1];
    dest[2][0] = s[2];
    dest[2][1] = u[2];
    dest[2][2] =-f[2];
    dest[3][0] =-v3_dot(s, eye);
    dest[3][1] =-v3_dot(u, eye);
    dest[3][2] = v3_dot(f, eye);
    dest[0][3] = dest[1][3] = dest[2][3] = 0.0f;
    dest[3][3] = 1.0f;
}

void
mat4_translation(v3 offset, mat4 dest) {
    dest[0][0] = 1;
    dest[0][1] = 0;
    dest[0][2] = 0;
    dest[0][3] = 0;
    
    dest[1][0] = 0;
    dest[1][1] = 1;
    dest[1][2] = 0;
    dest[1][3] = 0;
    
    dest[2][0] = 0;
    dest[2][1] = 0;
    dest[2][2] = 1;
    dest[2][3] = 0;
    
    dest[3][0] = offset[0];
    dest[3][1] = offset[1];
    dest[3][2] = offset[2];
    dest[3][3] = 1;
}

void
mat4_scale(v3 scale, mat4 dest) {
    dest[0][0] = scale[0];
    dest[0][1] = 0;
    dest[0][2] = 0;
    dest[0][3] = 0;
    
    dest[1][0] = 0;
    dest[1][1] = scale[1];
    dest[1][2] = 0;
    dest[1][3] = 0;
    
    dest[2][0] = 0;
    dest[2][1] = 0;
    dest[2][2] = scale[2];
    dest[2][3] = 0;
    
    dest[3][0] = 0;
    dest[3][1] = 0;
    dest[3][2] = 0;
    dest[3][3] = 1;
}

void
mat4_identity(mat4 dest) {
    mat4_scale((v3){1,1,1}, dest);
}

void
quat_angle_axis(versor q, float angle, v3 axis) {
    v3 k;
    float a, c, s;
    
    a = angle * 0.5f;
    c = cosf(a);
    s = sinf(a);
    
    v3_copy(axis, k);
    v3_normalize(k);
    
    q[0] = s * k[0];
    q[1] = s * k[1];
    q[2] = s * k[2];
    q[3] = c;
}

void
quat_mat4(versor q, mat4 dest) {
    float w, x, y, z,
    xx, yy, zz,
    xy, yz, xz,
    wx, wy, wz, norm, s;
    
    norm = v4_length(q);
    s    = norm > 0.0f ? 2.0f / norm : 0.0f;
    
    x = q[0];
    y = q[1];
    z = q[2];
    w = q[3];
    
    xx = s * x * x;   xy = s * x * y;   wx = s * w * x;
    yy = s * y * y;   yz = s * y * z;   wy = s * w * y;
    zz = s * z * z;   xz = s * x * z;   wz = s * w * z;
    
    dest[0][0] = 1.0f - yy - zz;
    dest[1][1] = 1.0f - xx - zz;
    dest[2][2] = 1.0f - xx - yy;
    
    dest[0][1] = xy + wz;
    dest[1][2] = yz + wx;
    dest[2][0] = xz + wy;
    
    dest[1][0] = xy - wz;
    dest[2][1] = yz - wx;
    dest[0][2] = xz - wy;
    
    dest[0][3] = 0.0f;
    dest[1][3] = 0.0f;
    dest[2][3] = 0.0f;
    dest[3][0] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][2] = 0.0f;
    dest[3][3] = 1.0f;
}

/* =========================================================================
     COLLISION DETECTION
   ========================================================================= */

bool
line_vs_line(v2 p1, v2 p2, v2 q1, v2 q2, v2 intersection) {
    v2 r = { p2[0] - p1[0], p2[1] - p1[1] };
    v2 s = { q2[0] - q1[0], q2[1] - q1[1] };
    float rxs = r[0] * s[1] - r[1] * s[0];
    v2 qp = { q1[0] - p1[0], q1[1] - p1[1] };
    float t = (qp[0] * s[1] - qp[1] * s[0]) / rxs;
    float u = (qp[0] * r[1] - qp[1] * r[0]) / rxs;
    if (rxs == 0 || t < 0 || t > 1 || u < 0 || u > 1)
        return false; // No intersection
    else {
        intersection[0] = p1[0] + t * r[0];
        intersection[1] = p1[1] + t * r[1];
        return true; // Intersection found
    }
}

bool
circle_vs_circle(Circle A, Circle B)
{
    v2 bMinusA;
    v2_sub(B.center, A.center, bMinusA);
    
    f32 distsq = v2_length2(bMinusA);
    
    f32 radii = A.radius + B.radius;
    f32 radiisq = radii * radii;
    
    return (distsq < radiisq);
}

f32 
circle_vs_circle_dist(Circle A, Circle B)
{
    v2 aMinusB;
    v2_sub(A.center, B.center, aMinusB);
    f32 dist = v2_length(aMinusB);
    return max(A.radius + B.radius - dist, 0);
}

void
circle_vs_circle_witness(Circle A, Circle B, v2 p1, v2 p2)
{
    v2 aMinusB;
    v2_sub(A.center, B.center, aMinusB);
    
    v2_normalize(aMinusB);
    
    v2 vA;
    v2_mul(A.radius, aMinusB, vA);
    
    v2 vB;
    v2_mul(B.radius, aMinusB, vB);
    
    v2_sub(A.center, vA, p1);
    v2_add(B.center, vB, p2);
}

bool 
circle_vs_ngon(Circle circle, Ngon polygon) {
    // Cast a ray from the center of the circle to the right
    v2 ray_origin;
    v2_copy(circle.center, ray_origin);
    v2 ray_direction = { 1.0f, 0.0f };
    v2 ray_endpoint = { ray_origin[0] + 2.0f * circle.radius, ray_origin[1] };
    
    int num_intersections = 0;
    // Check if the ray intersects with any of the polygon edges
    for (int i = 0; i < polygon.n; i++) {
        v2 edge_start;
        v2_copy(polygon.vertices[i], edge_start);
        v2 edge_end;
        v2_copy(polygon.vertices[(i + 1) % polygon.n], edge_end);
        v2 intersection_point;
        if (line_vs_line(ray_origin, ray_endpoint, edge_start, edge_end, intersection_point))
            num_intersections++;
    }
    
    return (num_intersections % 2 == 1);
}

bool
point_vs_rect2(v2 p, Rect2 rect) {
    if (p[0] > rect.max[0] ||
        p[1] > rect.max[1] ||
        p[0] < rect.min[0] ||
        p[1] < rect.min[1])
        return false;
    return true;
}

#endif