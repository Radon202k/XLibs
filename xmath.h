#ifndef XLIB_MATH
#define XLIB_MATH

/* =========================================================================
   DATA TYPES
   ========================================================================= */

typedef union v2f
{
    f32 e[2];
    struct { f32 x; f32 y; };
} v2f;

typedef union v2i
{
    s32 e[2];
    struct { s32 x; s32 y; };
} v2i;

typedef union v3f
{
    f32 e[3];
    struct { f32 x; f32 y; f32 z; };
    struct { v2f xy; f32 ignored0; };
} v3f;

typedef union v3i
{
    s32 e[3];
    struct { s32 x; s32 y; s32 z; };
    struct { v2i xy; s32 ignored0; };
} v3i;

typedef union v4f
{
    f32 e[4];
    struct { f32 x; f32 y; f32 z; f32 w; };
    struct { f32 r; f32 g; f32 b; f32 a; };
} v4f;

typedef union v4i
{
    s32 e[4];
    struct { s32 x; s32 y; s32 z; s32 w; };
    struct { s32 r; s32 g; s32 b; s32 a; };
} v4i;

typedef struct rect2f
{
    v2f min;
    v2f max;
} rect2f;

typedef struct rect3f
{
    v3f min;
    v3f max;
} rect3f;

typedef struct mat4f
{
    f32 data[4][4];
} mat4f;

typedef struct circlef
{
    v2f center;
    f32 radius;
} circlef;

typedef struct ngonf
{
    s32 n;
    v2f *vertices;
} ngonf;

#include "xmemory.h"

/* =========================================================================
   INT
   ========================================================================= */

s32 rndi(s32 a, s32 b);

/* =========================================================================
   FLOAT
   ========================================================================= */

#define maxf(a, b) (((a)>(b)) ? (a) : (b))
#define minf(a, b) (((a)>(b)) ? (b) : (a))

f32 mapf(f32 a1, f32 a2, f32 s, f32 b1, f32 b2);
f32 lrpf(f32 a, f32 t, f32 b);
f32 radf(f32 deg);
f32 degf(f32 rad);

/* =========================================================================
   RANDOM
   ========================================================================= */

f32 rndf(f32 a, f32 b);
void random_seed(u32 seed);

/* =========================================================================
   VECTOR 2 FLOAT
   ========================================================================= */

v2f  bez2f (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v2f  lrp2f (v2f a, f32 t, v2f b);
v2f  fil2f (f32 x);
v2f  ini2fs(s32 x, s32 y);
v2f *new2f (f32 x, f32 y);
v2f  add2f (v2f a, v2f b);
v2f  sub2f (v2f a, v2f b);
v2f  neg2f (v2f v);
v2f  mul2f (f32 k, v2f a);
v2f  had2f (v2f a, v2f b);
v2f  nrm2f (v2f a);
v2f  rnd2f (v2f a, v2f b);
f32  dot2f (v2f a, v2f b);
f32  lsq2f (v2f a);
f32  len2f (v2f a);
v2f  rot2f (v2f v, f32 a);
void padd2f(v2f *d, v2f v);
void psub2f(v2f *d, v2f v);

/* =========================================================================
   VECTOR 2 INT
   ========================================================================= */

v2i  bez2i (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v2i  lrp2i (v2i a, f32 t, v2i b);
v2i  fil2i (s32 x);
v2i  ini2if(f32 x, f32 y);
v2i *new2i (s32 x, s32 y);
v2i  add2i (v2i a, v2i b);
v2i  sub2i (v2i a, v2i b);
v2i  mul2i (f32 k, v2i a);
v2i  nrm2i (v2i a);
v2i  rnd2i (v2i a, v2i b);
f32  dot2i (v2i a, v2i b);
f32  lsq2i (v2i a);
f32  len2i (v2i a);
void padd2i(v2i *d, v2i v);
void psub2i(v2i *d, v2i v);

/* =========================================================================
   VECTOR 3 FLOAT
   ========================================================================= */

v3f  bez3f (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v3f  lrp3f (v3f a, f32 t, v3f b);
v3f *new3f (f32 x, f32 y, f32 z);
v3f  add3f (v3f a, v3f b);
v3f  sub3f (v3f a, v3f b);
v3f  mul3f (f32 k, v3f a);
v3f  nrm3f (v3f a);
v3f  rnd3f (v3f a, v3f b);
f32  dot3f (v3f a, v3f b);
f32  lsq3f (v3f a);
f32  len3f (v3f a);
void padd3f(v3f *d, v3f v);
void psub3f(v3f *d, v3f v);

/* =========================================================================
   VECTOR 3 INT
   ========================================================================= */

v3i  bez3i (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v3i  lrp3i (v3i a, f32 t, v3i b);
v3i *new3i (s32 x, s32 y);
v3i  add3i (v3i a, v3i b);
v3i  sub3i (v3i a, v3i b);
v3i  mul3i (f32 k, v3i a);
v3i  nrm3i (v3i a);
v3i  rnd3i (v3i a, v3i b);
f32  dot3i (v3i a, v3i b);
f32  lsq3i (v3i a);
f32  len3i (v3i a);
void padd3i(v3i *d, v3i v);
void psub3i(v3i *d, v3i v);

/* =========================================================================
   VECTOR 4 FLOAT
   ========================================================================= */

v4f  bez4f (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v4f  lrp4f (v4f a, f32 t, v4f b);
v4f  ini4f (f32 x, f32 y, f32 z, f32 w);
v4f *new4f (f32 x, f32 y, f32 z, f32 w);
v4f  add4f (v4f a, v4f b);
v4f  sub4f (v4f a, v4f b);
v4f  mul4f (f32 k, v4f a);
v4f  nrm4f (v4f a);
v4f  rnd4f (v4f a, v4f b);
f32  dot4f (v4f a, v4f b);
f32  lsq4f (v4f a);
f32  len4f (v4f a);

void padd4f(v4f *d, v4f v);
void psub4f(v4f *d, v4f v);

/* =========================================================================
   VECTOR 4 INT
   ========================================================================= */

v4i  vbez4i (f32 x1, f32 y1, f32 x2, f32 y2, f32 t);
v4i  vlrp4i (v4i a, f32 t, v4i b);
v4i *vnew4i (s32 x, s32 y);
v4i  vadd4i (v4i a, v4i b);
v4i  vsub4i (v4i a, v4i b);
v4i  vmul4i (f32 k, v4i a);
v4i  vnrm4i (v4i a);
f32  vdot4i (v4i a, v4i b);
f32  vlsq4i (v4i a);
f32  vlen4i (v4i a);
void pvadd4i(v4i *d, v4i v);
void pvsub4i(v4i *d, v4i v);

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

const v4f xcolor[] = {
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

typedef struct rect2f rect2f;

rect2f rini2f           (f32 minX, f32 minY, f32 maxX, f32 maxY);
rect2f rect2_min_dim    (v2f min, v2f dim);
rect2f rect2_center_dim (v2f center, v2f dim);
v2f    rect2_dim        (rect2f rect);
v2f    rect2_center     (rect2f rect);


/* =========================================================================
   RECT 3 FLOAT
   ========================================================================= */

typedef struct rect3f rect3f;

rect3f rect3_min_dim    (v3f min, v3f dim);
rect3f rect3_center_dim (v3f center, v3f dim);
v3f    rect3_dim        (rect3f rect);
v3f    rect3_center     (rect3f rect);


/* =========================================================================
   MATRIX 4X4 FLOAT
   ========================================================================= */

typedef struct mat4f mat4f;

/* =========================================================================
   COLLISION DETECTION
   ========================================================================= */

bool line_vs_line(v2f p1, v2f p2, v2f q1, v2f q2, v2f *intersection);
bool circlef_vs_circlef(circlef A, circlef B);
f32  circlef_vs_circlef_dist(circlef A, circlef B);
void circlef_vs_circlef_witness(circlef A, circlef B, v2f *p1, v2f *p2);
bool circlef_vs_ngonf(circlef circle, ngonf polygon);
bool point_vs_rect2(v2f p, rect2f rect);








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
sq(f32 value) {
    return value*value;
}

void random_seed(u32 seed)
{
    if (seed == 0)
        srand((u32)time(0));
    else
        srand(seed);
}

inline s32 rndi(s32 a, s32 b)
{
    return (rand() % (b - a + 1)) + a;
}

inline f32 rndf(f32 a, f32 b)
{
    return (float)rand() / RAND_MAX * (b - a) + a;
}

inline f32 mapf(f32 a1, f32 a2, f32 s, f32 b1, f32 b2)
{
    assert(a2!=a1);
    return (b1+(s-a1)*((b2-b1)/(a2-a1)));
}

inline f32 lrpf(f32 a, f32 t, f32 b)
{
    return (1-t)*a+t*b;
}

inline f32 radf(f32 deg)
{
    return deg*PIf/180.0f;
}

inline f32 degf(f32 rad)
{
    return rad*180.0f/PIf;
}

/* =========================================================================
   VECTOR 2 FLOAT
   ========================================================================= */

inline v2f fil2f(f32 v)
{
    v2f r = {v, v};
    return r;
}

inline v2f ini2fs(s32 x, s32 y)
{
    v2f r = {(f32)x, (f32)y};
    return r;
}

inline v2f *new2f(f32 x, f32 y)
{
    v2f *r = xalloc(sizeof *r);
    *r = (v2f){x,y};
    return r;
}

inline v2f bez2f(f32 x1, f32 y1, f32 x2, f32 y2, f32 t)
{
    v2f p0 = {0,0};
    v2f p1 = {x1,y1};
    v2f p2 = {x2,y2};
    v2f p3 = {1,1};
    
    f32 u = 1.0f - t;
    f32 tt = t * t;
    f32 uu = u * u;
    f32 uuu = uu * u;
    f32 ttt = tt * t;
    v2f p =
    {
        uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x,
        uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y
    };
    return p;
}

inline v2f add2f(v2f a, v2f b)
{
    v2f r = 
    {
        a.x + b.x,
        a.y + b.y
    };
    return r;
}

inline v2f sub2f(v2f a, v2f b)
{
    v2f r = 
    {
        a.x - b.x,
        a.y - b.y
    };
    return r;
}

inline v2f neg2f(v2f v)
{
    v2f r =
    {
        -v.x,
        -v.y,
    };
    return r;
}

inline v2f mul2f(f32 k, v2f a)
{
    v2f r = 
    {
        a.x*k,
        a.y*k
    };
    return r;
}

inline f32 dot2f(v2f a, v2f b)
{
    f32 result = a.x*b.x + a.y*b.y;
    return result;
}

inline f32 lsq2f(v2f a)
{
    return dot2f(a, a);
}

inline f32 len2f(v2f a)
{
    return sqrtf(lsq2f(a));
}

void padd2f(v2f* d, v2f v)
{
    *d = add2f(*d, v);
}

void psub2f(v2f* d, v2f v)
{
    *d = sub2f(*d, v);
}

inline v2f
had2f(v2f a, v2f b) {
    v2f result = {
        a.x*b.x,
        a.y*b.y,
    };
    return result;
}

inline v2f nrm2f(v2f a)
{
    assert(len2f(a) != 0);
    return mul2f(1.0f/len2f(a), a);
}

inline v2f lrp2f(v2f a, f32 t, v2f b)
{
    v2f r =
    {
        lrpf(a.x, t, b.x),
        lrpf(a.y, t, b.y),
    };
    return r;
}

inline v2f rnd2f(v2f a, v2f b)
{
    v2f r =
    {
        rndf(a.x, b.x),
        rndf(a.y, b.y),
    };
    return r;
}

inline v2f rot2f(v2f v, f32 a)
{
    a = radf(a);
    v2f r =
    {
        cosf(a) * v.x - sinf(a) * v.y,
        sinf(a) * v.x + cosf(a) * v.y,
    };
    return r;
}

/* =========================================================================
   VECTOR 2 INT
   ========================================================================= */

inline v2i fil2i(s32 v)
{
    v2i r = {v, v};
    return r;
}

inline v2i ini2if(f32 x, f32 y)
{
    v2i r = {(s32)x, (s32)y};
    return r;
}

inline v2i add2i(v2i a, v2i b)
{
    v2i r =
    {
        a.x + b.x,
        a.y + b.y
    };
    return r;
}

inline v2i sub2i(v2i a, v2i b)
{
    v2i r =
    {
        a.x - b.x,
        a.y - b.y
    };
    return r;
}

void padd2i(v2i* d, v2i v)
{
    *d = add2i(*d, v);
}

inline v2i rnd2i(v2i a, v2i b)
{
    v2i r =
    {
        rndi(a.x, b.x),
        rndi(a.y, b.y),
    };
    return r;
}

/* =========================================================================
   VECTOR 4 FLOAT
   ========================================================================= */

inline v4f ini4f(f32 r, f32 g, f32 b, f32 a)
{
    v4f res = {r, g, b, a};
    return res;
}

inline v4f lrp4f(v4f a, f32 t, v4f b)
{
    v4f r = 
    {
        lrpf(a.r, t, b.r),
        lrpf(a.g, t, b.g),
        lrpf(a.b, t, b.b),
        lrpf(a.a, t, b.a),
    };
    return r;
}

inline v4f rnd4f(v4f a, v4f b)
{
    v4f r = 
    {
        rndf(a.r, b.r),
        rndf(a.g, b.g),
        rndf(a.b, b.b),
        rndf(a.a, b.a),
    };
    return r;
}

/* =========================================================================
   RECT 2 FLOAT
   ========================================================================= */

inline rect2f
inir2f(f32 minX, f32 minY, f32 maxX, f32 maxY)
{
    rect2f r =
    {
        {minX, minY},
        {maxX, maxY},
    };
    return r;
}

inline rect2f
rect2_center_dim(v2f center, v2f dim)
{
    rect2f r =
    {
        {center.x - 0.5f*dim.x, center.y - 0.5f*dim.y},
        {center.x + 0.5f*dim.x, center.y + 0.5f*dim.y},
    };
    return r;
}

inline rect2f
rect2_min_max(v2f min, v2f max)
{
    rect2f r =
    {
        {min.x, min.y},
        {max.x, max.y},
    };
    return r;
}

inline rect2f
rect2_min_dim(v2f min, v2f dim)
{
    rect2f r =
    {
        {min.x, min.y},
        {min.x + dim.x, min.y + dim.y},
    };
    return r;
}

inline v2f
rect2_dim(rect2f rect) {
    v2f result = sub2f(rect.max, rect.min);
    return result;
}

inline v2f
rect2_center(rect2f rect) {
    v2f dim = sub2f(rect.max, rect.min);
    v2f center = add2f(rect.min, mul2f(0.5f, dim));
    return center;
}


/* =========================================================================
   RECT 2 FLOAT
   ========================================================================= */

inline rect3f
rect3_center_dim(v3f center, v3f dim)
{
    rect3f r =
    {
        {center.x-0.5f*dim.x, center.y-0.5f*dim.y, center.z-0.5f*dim.z},
        {center.x+0.5f*dim.x, center.y+0.5f*dim.y, center.z+0.5f*dim.z},
    };
    return r;
}

inline rect3f
rect3_min_max(v3f min, v3f max)
{
    rect3f r =
    {
        {min.x, min.y, min.z},
        {max.x, max.y, max.z},
    };
    return r;
}

inline rect3f
rect3_min_dim(v3f min, v3f dim)
{
    rect3f r =
    {
        {min.x, min.y, min.z},
        {min.x+dim.x, min.y+dim.y, min.z+dim.z},
    };
    return r;
}

inline v3f
rect3_dim(rect3f rect) {
    v3f result = sub3f(rect.max, rect.min);
    return result;
}

inline v3f
rect3_center(rect3f rect) {
    v3f dim = sub3f(rect.max, rect.min);
    v3f center = add3f(rect.min, mul3f(0.5f, dim));
    return center;
}

/* =========================================================================
     COLLISION DETECTION
   ========================================================================= */

bool line_vs_line(v2f p1, v2f p2, v2f q1, v2f q2, v2f *intersection)
{
    v2f r = { p2.x - p1.x, p2.y - p1.y };
    v2f s = { q2.x - q1.x, q2.y - q1.y };
    float rxs = r.x * s.y - r.y * s.x;
    v2f qp = { q1.x - p1.x, q1.y - p1.y };
    float t = (qp.x * s.y - qp.y * s.x) / rxs;
    float u = (qp.x * r.y - qp.y * r.x) / rxs;
    if (rxs == 0 || t < 0 || t > 1 || u < 0 || u > 1)
        return false; // No intersection
    else {
        intersection->x = p1.x + t * r.x;
        intersection->y = p1.y + t * r.y;
        return true; // Intersection found
    }
}

bool circlef_vs_circlef(circlef A, circlef B)
{
    f32 distsq = lsq2f(sub2f(B.center, A.center));
    
    f32 radii = A.radius + B.radius;
    f32 radiisq = radii * radii;
    
    return (distsq < radiisq);
}

f32 circlef_vs_circlef_dist(circlef A, circlef B)
{
    return maxf(A.radius + B.radius - len2f(sub2f(A.center, B.center)), 0);
}

void circlef_vs_circlef_witness(circlef A, circlef B, v2f *p1, v2f *p2)
{
    v2f v = nrm2f(sub2f(A.center, B.center));
    *p1 = sub2f(A.center, mul2f(A.radius, v));
    *p2 = add2f(B.center, mul2f(B.radius, v));
}

bool circlef_vs_ngonf(circlef circle, ngonf polygon)
{
    // Cast a ray from the center of the circle to the right
    v2f ray_origin = circle.center;
    v2f ray_direction = { 1.0f, 0.0f };
    v2f ray_endpoint = { ray_origin.x + 2.0f * circle.radius, ray_origin.y };
    
    int num_intersections = 0;
    // Check if the ray intersects with any of the polygon edges
    for (int i = 0; i < polygon.n; i++) {
        v2f edge_start = polygon.vertices[i];
        v2f edge_end = polygon.vertices[(i + 1) % polygon.n];
        v2f intersection_point;
        if (line_vs_line(ray_origin, ray_endpoint, edge_start, edge_end, &intersection_point))
            num_intersections++;
    }
    
    return (num_intersections % 2 == 1);
}

bool point_vs_rect2(v2f p, rect2f rect)
{
    if (p.x > rect.max.x ||
        p.y > rect.max.y ||
        p.x < rect.min.x ||
        p.y < rect.min.y)
        return false;
    return true;
}

#endif