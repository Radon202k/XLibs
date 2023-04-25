static void
list_render_element(List_data *data, v2 p, v2 dim, v4 bgCol, v4 fgCol, f32 scale, f32 dt) {
    /* Draw rect */
    draw_rect(plat.passes, p, dim, bgCol);
    /* Draw text */
    draw_text(plat.passes + 1, p, scale, fgCol, data->text);
}

static List_data *
list_find_closest_to_p(List_linked *list, v2 p) {
    f32 closestDist = 1000000;
    List_data *closest = 0;
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        if (data) {
            v2 X, pMinusX;
            v2_add(data->drawP, (v2){0.5f*list->elDim[0], 0}, X);
            v2_sub(p, X, pMinusX);
            
            f32 dist = v2_length2(pMinusX);
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
list_update_dragging_logic(List_linked *list, List_data *data, f32 scale, f32 dt) {
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
        v2 mouseP, halfDim;
        v2_mul(0.5f, elDim, halfDim);
        v2_sub(xmouse.p, halfDim, mouseP);
        list_render_element(data, mouseP, elDim, (v4){1,1,1,0.5f}, xcolor[Crimson], scale, dt);
        
        /* Find the closest element to the mouse position */
        List_data *closest = list_find_closest_to_p(list, xmouse.p);
        if (closest) {
            v2 drawP;
            v2_add(closest->drawP, (v2){0.5f*elDim[0], -20}, drawP);
            // draw_arrow(plat.layers + 1, xmouse.p, drawP, xcolor[Crimson], 15);
            
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
list_render(List_linked *list, LinkingContext *link, f32 scale, f32 dt) {
    v2 origin;
    v2_copy(list->origin, origin);
    /* For each bucket of the list */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        v2 bucketP;
        v2_copy(origin, bucketP);
        
        /* Draw bucket rect */
        draw_rect(plat.passes, bucketP, list->elDim, xcolor[White]);
        
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
            v2 elP, elDim;
            v2_add(origin, (v2){2,2}, elP);
            v2_sub(list->elDim, (v2){4,4}, elDim);
            list_render_element(data, elP, elDim, bgCol, fgCol, scale, dt);
            
            u32 netId = link_obj_to_id(link, data);
            char *numberStr = xstrint(netId, 0);
            v2 textP;
            v2_add(origin, (v2){0,list->elDim[1]+16}, textP);
            draw_text(plat.passes + 1, textP, 0.5f, xcolor[Gold], numberStr);
            
            xfree(numberStr);
            
            /* Save its draw position */
            v2_copy(origin, data->drawP);
        }
        
        v2_add(origin, (v2){list->elDim[0] + list->margin, 0}, origin);
    }
}

static void
list_update(List_linked *list, f32 scale, f32 dt) {
    v2 origin;
    v2_copy(list->origin, origin);
    /* For each bucket of the list */
    for (List_linked_node *at=list->head; at; at=at->next) {
        List_data *data = at->data;
        /* If there is an element in the bucket */
        if (data) {
            list_update_dragging_logic(list, data, scale, dt);
            /* Save its draw position */
            v2_copy(origin, data->drawP);
        }
        
        v2_add(origin, (v2){list->elDim[0] + list->margin, 0}, origin);
    }
}
