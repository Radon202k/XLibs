#ifndef APP_H
#define APP_H

typedef struct App {
    bool runServer;
    bool serverConstructed;
    List_linked list;
} App;

static void app_construct(void);
static void app_destruct(void);
static void app_update_and_render(f32 dt);

#endif //APP_H
