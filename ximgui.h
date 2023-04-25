#ifndef IMGUI_H
#define IMGUI_H

/* USAGE: 

;  v2 buttonP = app.dialogBoxP;
        ;  buttonP[0] += 5;
        ;  buttonP[1] += 5;
        ;  {
            ;    char *label = L"Editar";
            ;    v2 labelDim = xrender2d_font_dim(win32.fontSansSerif[0], label);
            ;    v2 buttonDim = {dialogBoxDim[0]-10, labelDim[1]+10};
            ;
             ;    if (imgui_button(&win32.layers[Layer_ui1], win32.fontSansSerif[1],
                             ;        buttonP, buttonDim, xcolor[Azure], xcolor[White], label,
                             ;        &app.dialogButtonEdit)) {
                ;        app.state = AppStateMachine_edit_note;
              ;    }
            ;
            ;    padd2f(&buttonP, (v2){0,buttonDim[1]+5});
        ;  }
        */

typedef enum {
    XImguiType_null,
    XImguiType_button,
    XImguiType_input,
} XImguiType;

typedef struct {
    char storage[512];
    u32 index;
    void (* enter)(char *, u32);
    void *tabId;
    XImguiType tabType;
} XImguiInputState;

typedef struct {
    void *hot;
    void *active;
    XImguiType activeType;
} XImguiContext;

global XImguiContext ximgui;

static bool ximgui_input      (XRenderBatch *batch, XFont *font, 
                               v2 p, v2 dim, v4 bgColor, v4 fgColor, v4 labelColor, char *label, bool code,
                               void *id);

static bool ximgui_button     (XRenderBatch *batch, XFont *font,
                               v2 p, v2 dim, v4 bgColor, v4 fgColor, char *label,
                               void *id);

static void ximgui_vscrollbar (XRenderBatch *contentBatch, XRenderBatch *uiBatch,
                               v2 panelP, v2 panelDim, f32 contentH, 
                               f32 *point, f32 *pointVel, f32 scrollVel, f32 scrollDrag);

static void
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
    char buf[512];
    swprintf_s(buf, 512, L"Hot: %p Active: %p", imgui.hot, imgui.active);
    
    draw_text(batch, (v2){0, xd11.back_buffer_size[1]-30},
              azu4f, font, buf);
#endif
    
    /* Reset active if mouse pressed with no hot */
    if (!ximgui.hot && xmouse.left.pressed)
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


static bool
ximgui_button(XRenderBatch *batch, XFont *font,
              v2 p, v2 dim, v4 bgColor, v4 fgColor, char *label,
              void *id) {
    bool clicked = false;
    
    
    /* Handle input */
    
    /* If mouse is inside box */
    if (point_vs_rect2(xmouse.p, rect2_min_dim(p, dim))) {
        /* Set as hot */
        ximgui.hot = id;
        
        xd11_set_cursor(cursor_link);
    }
    
    /* If hot and mouse pressed */
    if (ximgui.hot == id && xmouse.left.pressed) {
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
    v2 textDim;
    xrender2d_font_dim(font, label, textDim);
    
    /* Expand the asked dimensions if it is smaller than necessary */
    f32 horizontalPadding = 20;
    if (dim[1] < textDim[1])
        dim[1] = textDim[1];
    if (dim[0] < textDim[0])
        dim[0] = textDim[0] + horizontalPadding;
    
    /* Draw the box */
    draw_rect_rounded(batch, p, dim, bgColor, 5);
    
    /* Centralize text */
    v2 dimMinusTextDim, half;
    v2_sub(dim, textDim, dimMinusTextDim);
    v2_mul(.5f, dimMinusTextDim, half);
    v2_add(p, half, p);
    
    /* Draw the label */
    draw_text(batch, p, fgColor, font, label);
    
    return clicked;
}

static bool
ximgui_input(XRenderBatch *batch, XFont *font, 
             v2 p, v2 dim, v4 bgColor, v4 fgColor, v4 labelColor, char *label, bool code,
             void *id) {
    v2 boxP;
    v2_copy(p, boxP);
    
    if (label) {
        /* Get the with of the label */
        v2 labelDim;
        xrender2d_font_dim(font, label, labelDim);
        boxP[0] += labelDim[0];
    }
    
    /* Handle input */
    
    /* If mouse is inside box */
    if (point_vs_rect2(xmouse.p, rect2_min_dim(boxP, dim))) {
        /* Set as hot */
        ximgui.hot = id;
    }
    
    /* If hot and mouse pressed */
    if (ximgui.hot == id && xmouse.left.pressed) {
        /* Set as active */
        ximgui.active = id;
        ximgui.activeType = XImguiType_input;
    }
    
    /* Render */
    v4 boxColor;
    v4_copy(bgColor, boxColor);
    if (ximgui.active == id)
        v4_copy(xcolor[Gold], boxColor);
    else if (ximgui.hot == id)
        v4_copy(xcolor[Azure], boxColor);
    
    if (label) {
        /* Draw the label */
        draw_text(batch, p, labelColor, font, label);
    }
    
    /* Draw the box */
    draw_rect_rounded(batch, boxP, dim, boxColor, 5);
    
    XImguiInputState *state = id;
    
    /* Draw the data */
    v2 textP;
    v2_add(boxP, (v2){4,0}, textP);
    if (code) {
        char *at = state->storage;
        
        f32 starRadius = .5f*font->charAvgWidth;
        
        v2 starP;
        v2_copy(textP, starP);
        starP[0] += starRadius;
        starP[1] += starRadius + .5f*(dim[1]-2*starRadius);
        
        while (*at++) {
            /* Draw star */
            draw_circle(batch, starP, (v4){0,0,0,1}, starRadius);
            
            /* Advance x */
            starP[0] += (2*starRadius);
        }
    }
    else
        draw_text(batch, textP, fgColor, font, state->storage);
    
    v2 cursorDim = {8,24};
    
    if (state == ximgui.active) {
        
        /* Draw typing cursor */
        v2 textDim;
        xrender2d_font_dim(font, state->storage, textDim);
        
        /* Calculate top margin to centralize vertically */
        f32 marginTop = 0.5f*(dim[1]-cursorDim[1]);
        
        v2 boxPPlusTextDimX;
        v2_add(boxP, (v2){textDim[0], marginTop}, boxPPlusTextDimX);
        draw_rect_rounded(batch, boxPPlusTextDimX, cursorDim, (v4){0.5f,0.5f,0.5f,1}, 4);
    }
    
    return false;
}

static void
ximgui_vscrollbar(XRenderBatch *contentBatch, XRenderBatch *uiBatch, 
                  v2 panelP, v2 panelDim, f32 contentH, 
                  f32 *point, f32 *pointVel, f32 scrollVel, f32 scrollDrag) {
    if (point_vs_rect2(xmouse.p, rect2_min_dim(panelP, panelDim))) {
        if (xmouse.wheel && contentH > panelDim[1]) 
            *pointVel -= scrollVel *xmouse.wheel;
    }
    
    *point += xd11.dt * *pointVel;
    
    *pointVel = *pointVel * (1.0f - scrollDrag);
    
    *point = min(*point, 1);
    *point = max(*point, 0);
    
    /* Draw thumb */
    v4 thumbColor;
    v4_copy(xcolor[Black], thumbColor);
    thumbColor[3] = 0.5f;
    if (contentH > panelDim[1]) {
        f32 diffPerc = (contentH-panelDim[1]) / contentH;
        f32 thumbH = panelDim[1] - panelDim[1]*diffPerc;
        v2 thumbP;
        v2_add(panelP,
               (v2) {
                   panelDim[0] - 7, 
                   *point * diffPerc * panelDim[1]
               }, thumbP);
        
        v2 thumbDim = {5, thumbH};
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
