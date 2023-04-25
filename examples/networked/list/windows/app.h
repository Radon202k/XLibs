#ifndef APP_H
#define APP_H

typedef struct App {
    bool runServer;
    bool serverConstructed;
    List_linked list;
} App;

static void app_construct(void);
static void app_destruct(void);
static void app_update_and_render(void);

static void       list_update                (List_linked *list);
static void       list_render                (List_linked *list, LinkingContext *link);
static List_data *list_find_closest_to_p     (List_linked *list, v2 p);
static void       list_animations_prepare    (List_linked *list);
static void       list_animations_play       (List_linked *list, List_data *closest);
static void       list_update_draw_positions (List_linked *list);

#endif //APP_H
