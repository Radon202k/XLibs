#ifndef IMGUI_H
#define IMGUI_H

/* USAGE: 

;  v2f buttonP = app.dialogBoxP;
        ;  buttonP.x += 5;
        ;  buttonP.y += 5;
        ;  {
            ;    wchar_t *label = L"Editar";
            ;    v2f labelDim = xrender2d_font_dim(win32.fontSansSerif[0], label);
            ;    v2f buttonDim = {dialogBoxDim.x-10, labelDim.y+10};
            ;
             ;    if (imgui_button(&win32.layers[Layer_ui1], win32.fontSansSerif[1],
                             ;        buttonP, buttonDim, xcolor[Azure], xcolor[White], label,
                             ;        &app.dialogButtonEdit)) {
                ;        app.state = AppStateMachine_edit_note;
              ;    }
            ;
            ;    padd2f(&buttonP, (v2f){0,buttonDim.y+5});
        ;  }
        */

typedef enum {
    XImguiType_null,
    XImguiType_button,
    XImguiType_input,
} XImguiType;

typedef struct {
    wchar_t storage[512];
    u32 index;
    void (* enter)(wchar_t *, u32);
    void *tabId;
    XImguiType tabType;
} XImguiInputState;

typedef struct {
    void *hot;
    void *active;
    XImguiType activeType;
} XImguiContext;

global XImguiContext ximgui;

function bool ximgui_input      (XRenderBatch *batch, XFont *font, 
                                 v2f p, v2f dim, v4f bgColor, v4f fgColor, v4f labelColor, wchar_t *label, bool code,
                                 void *id);

function bool ximgui_button     (XRenderBatch *batch, XFont *font,
                                 v2f p, v2f dim, v4f bgColor, v4f fgColor, wchar_t *label,
                                 void *id);

function void ximgui_vscrollbar (XRenderBatch *contentBatch, XRenderBatch *uiBatch,
                                 v2f panelP, v2f panelDim, f32 contentH, 
                                 f32 *point, f32 *pointVel, f32 scrollVel, f32 scrollDrag);

function void
ximgui_update(XRenderBatch *batch, XFont *font) {
    if (ximgui.active) {
        switch (ximgui.activeType) {
            case XImguiType_input:
            {
                XImguiInputState *state = ximgui.active;
                
                /* Handle char input */
                if (xwin.inputChar) {
                    if (xwin.inputChar != '\n' 
                        && xwin.inputChar != '\r'
                        && xwin.inputChar != '\b'
                        && xwin.inputChar != '\t') {
                        state->storage[state->index++] = xwin.inputChar;
                        state->storage[state->index] = 0;
                    }
                }
                
                /* Handle commands */
                if (xwin.key.enter.pressed) {
                    if (state->enter)
                        state->enter(state->storage, state->index);
                }
                
                if (xwin.key.backspace.pressed) {
                    if (state->index > 0) {
                        --state->index;
                        state->storage[state->index] = 0;
                    }
                }
                
                if (xwin.key.tab.pressed) {
                    ximgui.hot = ximgui.active = state->tabId;
                    ximgui.activeType = state->tabType;
                }
                
            } break;
            
            default:
            {
                /* Ignore */
            } break;
        }
    }
    
#if 0
    /* Debug print */
    wchar_t buf[512];
    swprintf_s(buf, 512, L"Hot: %p Active: %p", imgui.hot, imgui.active);
    
    draw_text(batch, (v2f){0, xd11.back_buffer_size.y-30},
              azu4f, font, buf);
#endif
    
    /* Reset active if mouse pressed with no hot */
    if (!ximgui.hot && xwin.mouse.left.pressed)
        ximgui.active = 0;
    
    if (xwin.lastCursorSet) {
        xwin.cursorSet = xwin.lastCursorSet;
        SetCursor(xwin.cursorSet);
        xwin.lastCursorSet = 0;
    }
    else {
        xwin.cursorSet = cursor_select;
        SetCursor(xwin.cursorSet);
    }
    
    /* Reset hot */
    ximgui.hot = 0;
}


function bool
ximgui_button(XRenderBatch *batch, XFont *font,
              v2f p, v2f dim, v4f bgColor, v4f fgColor, wchar_t *label,
              void *id) {
    bool clicked = false;
    
    
    /* Handle input */
    
    /* If mouse is inside box */
    if (point_vs_rect2(xwin.mouse.pos, rect2_min_dim(p, dim))) {
        /* Set as hot */
        ximgui.hot = id;
        
        xd11_set_cursor(cursor_link);
    }
    
    /* If hot and mouse pressed */
    if (ximgui.hot == id && xwin.mouse.left.pressed) {
        /* Set as active */
        ximgui.active = id;
        ximgui.activeType = XImguiType_button;
        clicked = true;
    }
    
    if (ximgui.hot == id)
        bgColor = xcolor[Crimson];
    else if (ximgui.active == id)
        bgColor = xcolor[Azure];
    
    /* Get the dimensions of the text */
    v2f textDim = xrender2d_font_dim(font, label);
    
    /* Expand the asked dimensions if it is smaller than necessary */
    f32 horizontalPadding = 20;
    if (dim.y < textDim.y)
        dim.y = textDim.y;
    if (dim.x < textDim.x)
        dim.x = textDim.x + horizontalPadding;
    
    /* Draw the box */
    draw_rect_rounded(batch, p, dim, bgColor, 5);
    
    /* Centralize text */
    padd2f(&p, mul2f(.5f, sub2f(dim, textDim)));
    
    /* Draw the label */
    draw_text(batch, p, fgColor, font, label);
    
    return clicked;
}

function bool
ximgui_input(XRenderBatch *batch, XFont *font, 
             v2f p, v2f dim, v4f bgColor, v4f fgColor, v4f labelColor, wchar_t *label, bool code,
             void *id) {
    v2f boxP = p;
    
    if (label) {
        /* Get the with of the label */
        v2f labelDim = xrender2d_font_dim(font, label);
        boxP.x += labelDim.x;
    }
    
    /* Handle input */
    
    /* If mouse is inside box */
    if (point_vs_rect2(xwin.mouse.pos, rect2_min_dim(boxP, dim))) {
        /* Set as hot */
        ximgui.hot = id;
    }
    
    /* If hot and mouse pressed */
    if (ximgui.hot == id && xwin.mouse.left.pressed) {
        /* Set as active */
        ximgui.active = id;
        ximgui.activeType = XImguiType_input;
    }
    
    /* Render */
    v4f boxColor = bgColor;
    if (ximgui.active == id)
        boxColor = xcolor[Gold];
    else if (ximgui.hot == id)
        boxColor = xcolor[Azure];
    
    if (label) {
        /* Draw the label */
        draw_text(batch, p, labelColor, font, label);
    }
    
    /* Draw the box */
    draw_rect_rounded(batch, boxP, dim, boxColor, 5);
    
    XImguiInputState *state = id;
    
    /* Draw the data */
    v2f textP = add2f(boxP, (v2f){4,0});
    if (code) {
        wchar_t *at = state->storage;
        
        f32 starRadius = .5f*font->charAvgWidth;
        
        v2f starP = textP;
        starP.x += starRadius;
        starP.y += starRadius + .5f*(dim.y-2*starRadius);
        
        while (*at++) {
            /* Draw star */
            draw_circle(batch, starP, (v4f){0,0,0,1}, starRadius);
            
            /* Advance x */
            starP.x += (2*starRadius);
        }
    }
    else
        draw_text(batch, textP, fgColor, font, state->storage);
    
    v2f cursorDim = (v2f){8,24};
    
    if (state == ximgui.active) {
        
        /* Draw typing cursor */
        v2f textDim = xrender2d_font_dim(font, state->storage);
        
        /* Calculate top margin to centralize vertically */
        f32 marginTop = 0.5f*(dim.y-cursorDim.y);
        
        draw_rect_rounded(batch,
                          add2f(boxP, (v2f){textDim.x, marginTop}),
                          cursorDim,
                          (v4f){0.5f,0.5f,0.5f,1},
                          4);
    }
    
    return false;
}

function void
ximgui_vscrollbar(XRenderBatch *contentBatch, XRenderBatch *uiBatch, 
                  v2f panelP, v2f panelDim, f32 contentH, 
                  f32 *point, f32 *pointVel, f32 scrollVel, f32 scrollDrag) {
    if (point_vs_rect2(xwin.mouse.pos, rect2_min_dim(panelP, panelDim))) {
        if (xwin.mouse.wheel && contentH > panelDim.y) 
            *pointVel -= scrollVel *xwin.mouse.wheel;
    }
    
    *point += xd11.dt * *pointVel;
    
    *pointVel = *pointVel * (1.0f - scrollDrag);
    
    *point = min(*point, 1);
    *point = max(*point, 0);
    
    /* Draw thumb */
    v4f thumbColor = xcolor[Black];
    thumbColor.a = 0.5f;
    if (contentH > panelDim.y) {
        f32 diffPerc = (contentH-panelDim.y) / contentH;
        f32 thumbH = panelDim.y - panelDim.y*diffPerc;
        v2f thumbP = add2f(panelP,
                           (v2f) {
                               panelDim.x - 7, 
                               *point * diffPerc * panelDim.y
                           });
        
        v2f thumbDim = (v2f){5, thumbH};
        draw_rect(uiBatch, thumbP, thumbDim, thumbColor);
    }
    else {
        if (*point > 0)
            *pointVel = *pointVel - 5;
    }
    
    /* Scissor */
    *(contentBatch->scissor) = rect2_min_dim(panelP, panelDim);
}


#endif //IMGUI_H
