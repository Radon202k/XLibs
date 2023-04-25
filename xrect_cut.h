#ifndef XRECT_CUT_H
#define XRECT_CUT_H

static Rect2
xcut_left(Rect2 *rect, f32 a) {
    f32 minx = rect->min[0];
    rect->min[0] = (f32)fmin(rect->max[0], rect->min[0] + a);
    return (Rect2){ minx, rect->min[1], rect->min[0], rect->max[1] };
}

static Rect2
xcut_right(Rect2 *rect, f32 a) {
    f32 maxx = rect->max[0];
    rect->max[0] = (f32)fmax(rect->min[0], rect->max[0] - a);
    return (Rect2){ rect->max[0], rect->min[1], maxx, rect->max[1] };
}

static Rect2
xcut_top(Rect2 *rect, f32 a) {
    f32 miny = rect->min[1];
    rect->min[1] = (f32)fmin(rect->max[1], rect->min[1] + a);
    return (Rect2){ rect->min[0], miny, rect->max[0], rect->min[1] };
}

static Rect2
xcut_bottom(Rect2 *rect, f32 a) {
    f32 maxy = rect->max[1];
    rect->max[1] = (f32)fmax(rect->min[1], rect->max[1] - a);
    return (Rect2){ rect->min[1], rect->max[1], rect->max[0], maxy };
}

static void
xcut_contract(Rect2 *rect, f32 a) {
    v2_add(rect->min, (v2){a,a}, rect->min);
    v2_add(rect->max, (v2){a,a}, rect->max);
}

static void
xcut_contract_but_top(Rect2 *rect, f32 a) {
    rect->min[0] += a;
    rect->max[0] -= a;
    rect->max[1] -= a;
}

#endif //XRECT_CUT_H
