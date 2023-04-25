/* =============== Rendering, Dragging statics */

static void
list_render_element(List_data *data, v2 p, v2 dim, 
                    v4 bgCol, v4 fgCol, XFont *font) {
    if (data->animP.isAnimating) {
        animation_update(&data->animP, xd11.dt);
        p = data->drawP;
    }
    
    /* Draw rect */
    draw_rect(plat.layers + 0, p, dim, bgCol);
    /* Draw text */
    draw_text(plat.layers + 1, p, fgCol, font, data->text);
}

static List_data *
list_find_closest_to_p(List_linked *list, v2 p)
{
    f32 closestDist = 1000000;
    List_data *closest = 0;
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        if (data) {
            v2 drawPPlusDimX, pMinusDrawPPlusDimX;
            v2_add(data->drawP, (v2){0.5f*list->elDim[0], 0}, drawPPlusDimX);
            v2_sub(p, drawPPlusDimX, pMinusDrawPPlusDimX);
            f32 dist = v2_length2(pMinusDrawPPlusDimX);
            if (dist < closestDist) {
                closestDist = dist;
                closest = data;
            }
        }
    }
    
    if (closestDist > 250000)
        closest = 0;
    
    return closest;
}

static void 
list_update_draw_positions(List_linked *list) {
    v2 origin;
    v2_copy(list->origin, origin);
    
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        if (data)
            v2_copy(origin, data->drawP);
        v2_add(origin, (v2){list->elDim[0] + list->margin, 0}, origin);
    }
}

static void
list_animations_prepare(List_linked *list) {
    /* Save a positions */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        v2_copy(data->drawP, data->animP.a);
    }
}

static void
list_animations_play(List_linked *list, List_data *closest) {
    list_update_draw_positions(list);
    
    /* Save b positions and start animations */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        v2_copy(data->drawP, data->animP.b);
        data->animP.isAnimating = true;
        data->animP.tPlay = 0;
        data->animP.tTotal = 1;
    }
}

static void
list_update_dragging_logic(List_linked *list, List_data *data) {
    /* Handle mouse input for dragging elements of the list */
    if (point_vs_rect2(xmouse.p, rect2_min_dim(data->drawP, list->elDim))) {
        /* If there is not element in this list currently hot, make this one it */
        if (!list->hot)
            list->hot = data->node;
        if (!list->active &&
            list->hot && list->hot->data == data && 
            xmouse.left.pressed)
            list->active = data->node;
    }
    
    /* If this is the active element */
    if (list->active && list->active->data == data) {
        v2 elDim;
        v2_copy((v2){80,60}, elDim);
        /* Draw a "ghost" version of it where the mouse is */
        v2 halfDim, mousePMinusHalfDim;
        v2_mul(0.5f, elDim, halfDim);
        v2_sub(xmouse.p, halfDim, mousePMinusHalfDim);
        list_render_element(data, mousePMinusHalfDim, elDim, (v4){1,1,1,0.5f},
                            xcolor[Crimson], list->font);
        
        /* Find the closest element to the mouse position */
        List_data *closest = list_find_closest_to_p(list, xmouse.p);
        if (closest) {
            v2 drawPPlusHalfDimX;
            v2_add(closest->drawP, (v2){0.5f*elDim[0], -20}, drawPPlusHalfDimX);
            draw_arrow(plat.layers + 1, xmouse.p, drawPPlusHalfDimX, xcolor[Crimson], 15);
            
            /* If the user released the mouse */
            if (!xmouse.left.down) {
                /* Send packet to server asking move element of the list */
                u32 elNetId = link_obj_to_id(&client_link, data);
                u32 refNetId = link_obj_to_id(&client_link, closest);
                client_send_move_packet(elNetId, refNetId);
                list->active = 0;
            }
        }
    }
}

static void
list_render(List_linked *list, LinkingContext *link) {
    v2 origin;
    v2_copy(list->origin, origin);
    /* For each bucket of the list */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        v2 bucketP;
        v2_copy(origin, bucketP);
        
        /* Draw bucket rect */
        draw_rect(plat.layers, bucketP, list->elDim, xcolor[White]);
        
        /* If there is an element in the bucket */
        if (data) {
            /* Draw updated state */
            v4 bgCol, fgCol;
            v4_copy(data->color, bgCol);
            v4_copy(xcolor[Black], fgCol);
            
            if (list->hot && list->hot->data == data){
                v4_copy(xcolor[White], bgCol);
                v4_copy(xcolor[Crimson], fgCol);
            }
            
            /* Draw element */
            v2 originPlus2;
            v2_add(origin, (v2){2,2}, originPlus2);
            
            v2 dimMinus4;
            v2_sub(list->elDim, (v2){4,4}, dimMinus4);
            
            list_render_element(data, originPlus2, dimMinus4, bgCol, fgCol, list->font);
            
            u32 netId = link_obj_to_id(link, data);
            char *numberStr = xstrint(netId, 0);
            
            v2 originPlusDimY;
            v2_add(origin, (v2){0,list->elDim[1]+16}, originPlusDimY);
            draw_text(plat.layers, originPlusDimY, xcolor[Gold], plat.fonts[2], numberStr);
            xfree(numberStr);
            
            /* Save its draw position */
            v2_copy(origin, data->drawP);
        }
        
        v2_add(origin, (v2){list->elDim[0] + list->margin, 0}, origin);
    }
}

static void
list_update(List_linked *list) {
    v2 origin;
    v2_copy(list->origin, origin);
    /* For each bucket of the list */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        /* If there is an element in the bucket */
        if (data) {
            list_update_dragging_logic(list, data);
            /* Save its draw position */
            v2_copy(origin, data->drawP);
        }
        
        v2_add(origin, (v2){list->elDim[0] + list->margin, 0}, origin);
    }
}

static void app_construct(void) {
    list_linked_construct(&app.list);
    v2_copy((v2){80,60}, app.list.elDim);
    app.list.margin = 10;
    app.list.font = plat.fonts[1];
}

static void app_destruct(void) {
    list_linked_destruct(&app.list);
    link_destruct();
}

static void app_update_and_render(void) {
    if (!client.isConnected) {
        char *connecting = "Connecting...";
        v2 textDim;
        xrender2d_font_dim(plat.fonts[2], connecting, textDim);
        draw_text(plat.layers + 2, (v2){0.5f*(xd11.bbDim[0]-textDim[0]),100}, xcolor[Emerald], plat.fonts[2], connecting);
    }
    
    /* Test navbar */
    Rect2 layout = rect2_min_max(v2_zero, xd11.bbDim);
    Rect2 top = xcut_top(&layout, 0.5f*xd11.bbDim[1]);
    Rect2 bot = layout;
    
    xcut_contract(&top, 4);
    xcut_contract_but_top(&bot, 4);
    
    v2 topP, topDim;
    rect2_p(top, topP);
    rect2_dim(top, topDim);
    draw_rect_rounded(plat.layers, topP, topDim, (v4){.1f,.1f,.1f,1}, 5);
    
    v2 botP, botDim;
    rect2_p(bot, botP);
    rect2_dim(bot, botDim);
    draw_rect_rounded(plat.layers, botP, botDim, (v4){.1f,.1f,.1f,1}, 5);
    
    if (xwin.key.control.down && xwin.key.c.pressed) {
        app.runServer = true;
    }
    
    /* Calculate the total width of the list */
    f32 width = app.list.length*app.list.elDim[0] + (app.list.length-1)*app.list.margin;
    /* Update the list's draw dimension */
    v2_copy((v2){width, app.list.elDim[1]}, app.list.drawDim);
    /* Update the origin based on the calculated width */
    v2_copy((v2){
                0.5f*(xd11.bbDim[0]-app.list.drawDim[0]),
                bot.min[1],
            }, app.list.origin);
    list_update(&app.list);
    list_render(&app.list, &client_link);
    
    /* Reset hot index after everything has been updated for this frame */
    app.list.hot = 0;
    
    client_update();
}