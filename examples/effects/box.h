#ifndef BOX_H
#define BOX_H

typedef struct
{
    v2f pos;
    v2f size;
    v4f color;
} Box;

typedef struct
{
    List_T storage;
    List_node *drag_node;
    Box *drag;
    v2f last_p;
    v2f original_p;
} Boxes;

void boxes_spawn             (Boxes *boxes, v4f colors[], s32 count);
void boxes_update_layout     (Boxes *boxes);
void boxes_update_and_render (Boxes *boxes);

Box *box_new          (v4f color);
bool box_hover        (Box *box);
void box_start_drag   (Boxes *boxes, Box *box, List_node *node);
void box_dragged      (Boxes *boxes, Box *box);
void box_dropped      (Boxes *boxes, Box *box, List_node *node);
v2f  box_get_position (Box *box);






void boxes_spawn(Boxes *boxes, v4f colors[], s32 count)
{
    for (s32 i = 0; i < 4; ++i)
        List_push(&boxes->storage, box_new(colors[i]));
}

void boxes_update_layout(Boxes *boxes)
{
    f32 margin = 20;
    v2f box_size = {90,50};
    
    v4f colors[4] = {cri4f, gol4f, eme4f, azu4f}; 
    
    f32 at_y = 0;

    s32 i = 0;
    List_node *at = boxes->storage.head;
    while (at)
    {
        Box *box = at->data;

        box->pos = (v2f){0, at_y};
        box->size = box_size;
        if (i++ == 1)
            box->size = add2f(box_size, (v2f){0,100});

        at_y += (box->size.y + margin);

        at = at->next;
    }
}

void boxes_update_and_render(Boxes *boxes)
{
    if (boxes->drag)
    {
        box_dragged(boxes, boxes->drag);
        
        if (!xwin.mouse.left.down)
        {
            box_dropped(boxes, boxes->drag, boxes->drag_node);
        }
    }
    
    List_node *at = boxes->storage.head;
    while (at)
    {
        Box *box = at->data;
        
        v4f box_color = box->color;
        box_color.a = 0.8f;
        
        if (box_hover(box))
        {
            box_color.a = 1.0f;
            
            if (xwin.mouse.left.pressed && !boxes->drag)
                box_start_drag(boxes, box, at);
            
        }
        
        v2f box_pos = box_get_position(box);
        draw_rect(&layer1, box_pos, box->size, box_color);
        
        if (at->next)
        {
            Box *next_box = at->next->data;
            draw_arrow(&layer1, 
                add2f(box_pos, box->size),
                add2f(box_get_position(next_box), (v2f){box->size.x, 0}), 
                box_color, 10);
        }

        if (at->prev)
        {
            Box *prev_box = at->prev->data;
            draw_arrow(&layer1, 
                box_pos,
                add2f(box_get_position(prev_box), (v2f){0, box->size.y}), 
                box_color, 10);
        }
        
        at = at->next;
    }
}

Box *box_new(v4f color)
{
    Box *result = xalloc(sizeof *result);
    result->color = color;
    return result;
}

bool box_hover(Box *box)
{
    v2f p = box_get_position(box);
    rect2f rect = {p, add2f(p, box->size)};
    return (point_vs_rect2(xwin.mouse.pos, rect));
}

void box_start_drag(Boxes *boxes, Box *box, List_node *node)
{
    boxes->drag_node = node;
    boxes->drag = box;
    boxes->last_p = xwin.mouse.pos;
    boxes->original_p = box->pos;
}

void box_dropped(Boxes *boxes, Box *box, List_node *node)
{
    boxes->drag = 0;
    
    List_node *closest_node = 0;
    Box *closest_box = 0;
    f32 min_dist = 10000.0f;

    /* Loop all nodes to find closest */
    List_node *at = boxes->storage.head;
    while (at)
    {
        if (at != node)
        {
            Box *other = at->data;

            f32 dist = len2f(sub2f(box_get_position(box), other->pos));
            if (dist < min_dist)
            {
                min_dist = dist;
                closest_node = at;
                closest_box = other;
            }
        }

        at = at->next;
    }

    /* If closest is close enough */
    if (min_dist < 100)
    {
        assert(closest_node);

        /* Remove from list */
        void *data = List_remove(&boxes->storage, node);


        if (xwin.mouse.pos.y - 0.5f*closest_box->size.y < closest_box->pos.y)
        {
            /* Put before closest box found */
            List_put_before(&boxes->storage, data, closest_node);
        }
        else
        {
            /* Put after closest box found */
            List_put_after(&boxes->storage, data, closest_node);
        }
    }
    else
        box->pos = boxes->original_p;

    boxes_update_layout(boxes);
}

void box_dragged(Boxes *boxes, Box *box)
{
    v2f delta = sub2f(xwin.mouse.pos, boxes->last_p);
    boxes->last_p = xwin.mouse.pos;
    
    padd2f(&box->pos, delta);
}

v2f box_get_position(Box *box)
{
    return add2f(box->pos, (v2f){50,0});
}

#endif
