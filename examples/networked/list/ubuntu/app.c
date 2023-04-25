/* =============== Rendering, Dragging statics */


static void 
app_construct(void) {
    list_linked_construct(&app.list);
    v2_copy((v2){80,60}, app.list.elDim);
    app.list.margin = 10;
}

static void 
app_destruct(void) {
    list_linked_destruct(&app.list);
    link_destruct();
}

static void 
app_update_and_render(f32 dt) {
    /* Test navbar */
    v2 bbDim = {(f32)x11.width, (f32)x11.height};
    Rect2 layout = rect2_min_dim(v2_zero, bbDim);
    Rect2 top = xcut_top(&layout, 0.5f*bbDim[1]);
    Rect2 bot = layout;
    
    xcut_contract(&top, 4);
    xcut_contract_but_top(&bot, 4);
    
    v2 rectTopP, rectBotP, rectDim;
    rect2_p(top, rectTopP);
    rect2_p(bot, rectBotP);
    rect2_dim(top, rectDim);
    draw_rect(plat.passes, rectTopP, rectDim, (v4){.1f,.1f,.1f,1});
    draw_rect(plat.passes, rectBotP, rectDim, (v4){.1f,.1f,.1f,1});
    
    /* Calculate the total width of the list */
    f32 width = app.list.length*app.list.elDim[0] + (app.list.length-1)*app.list.margin;
    /* Update the list's draw dimension */
    v2_copy((v2){width, app.list.elDim[1]}, app.list.drawDim);
    /* Update the origin based on the calculated width */
    v2_copy((v2){0.5f*(bbDim[0]-app.list.drawDim[0]), bot.min[1]}, app.list.origin);
    list_update(&app.list, 1, dt);
    list_render(&app.list, &client_link, 1, dt);
    
    
    char buf[512];
    snprintf(buf, 512, "dt: %.6f", dt);
    draw_text(&plat.passes[1], (v2){0,128}, 0.25f, xcolor[Gold], buf);
    
    draw_rect(&plat.passes[2], xmouse.p, (v2){50,50}, xcolor[Azure]);
    
    client_update(dt);
    
    app.list.hot = 0;
}