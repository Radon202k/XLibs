#ifndef LIST_H
#define LIST_H

static void       list_render_element        (List_data *data, v2 p, v2 dim, v4 bgCol, v4 fgCol, f32 scale, f32 dt);
static List_data *list_find_closest_to_p     (List_linked *list, v2 p);
static void       list_update_draw_positions (List_linked *list);
static void       list_update_dragging_logic (List_linked *list, List_data *data, f32 scale, f32 dt);
static void       list_render                (List_linked *list, LinkingContext *link, f32 scale, f32 dt);
static void       list_update                (List_linked *list, f32 scale, f32 dt);

#endif //LIST_H
