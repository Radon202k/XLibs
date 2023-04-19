#ifndef XRENDER2D_H
#define XRENDER2D_H

/* =========================================================================
   USAGE
   ========================================================================= */

/*

// Load Font from disk
{
    wchar_t full_path[512];
    xwin_path_abs(full_path, 512, L"fonts\\Inconsolata.ttf");
    font = xrender2d_font(&xrender2d.texture_atlas,
                         full_path, L"Inconsolata", 64);
}

// Load PNG from disk and make Sprite in Texture Atlas
{
    wchar_t full_path[512];
    xwin_path_abs(full_path, 512, L"images\\naruto.png");
    sprite_naruto = xrender2d_sprite_from_png(&xrender2d.texture_atlas,
                                             full_path, false);
}

// Draw Line
draw_line(&layer1, (v2f){0,0}, xwin.mouse.pos, (v4f){1,1,1,1});

// Draw Rect
{
    v2f s = {200,20};
    v2f p = add2f(xwin.mouse.pos, mul2f(-.5f,s));
    draw_rect(&layer1, p, s, (v4f){1,0,0,1});
}

// Draw Sprite
{
    v2f s = sprite_naruto.size;
    v2f p = add2f(xwin.mouse.pos, mul2f(-.5f,s));
    draw_sprite(&layer1, p, s, (v4f){1,1,1,1}, sprite_naruto);
}

// Draw Text
{
    v2f p = {0,0};
    draw_text(&layer1, p, (v4f){1,0,0,1}, &font, L"Test");
}

// Example draw board
void draw_board(v2f board_size, f32 line_thickness)
{
    v2f board_pos = sub2f(mul2f(.5f, xd11.bbDim), mul2f(.5f,board_size));
    v2i line_count = {9,9};
    v2f line_size = {line_thickness,board_size.y+line_thickness};
    v2f line_space = (v2f){board_size.x/(line_count.x-1), board_size.y/(line_count.y-1)};
    for (s32 x=0; x<line_count.x; ++x)
        draw_rect(&layer1, 
                  add2f(board_pos, (v2f){x*line_space.x, 0}),
                  line_size, (v4f){0,0,0,1});
    
    line_size = (v2f){board_size.x+line_thickness,line_thickness};
    for (s32 y=0; y<line_count.y; ++y)
        draw_rect(&layer1, 
                  add2f(board_pos, (v2f){0, y*line_space.y}),
                  line_size, (v4f){0,0,0,1});
}

*/

/* =========================================================================
   INTERFACE
   ========================================================================= */

#define XRENDER2D_LINEVERTEX_MAX (1024/4*1024)/sizeof(TexturedVertex)
#define XRENDER2D_TEXTUREDVERTEX_MAX (1*1024*1024)/sizeof(TexturedVertex)

#define ONE_PAST_MAX_FONT_CODEPOINT (0x10FFFF + 1)

#define MAX_FONT_WIDTH 1024
#define MAX_FONT_HEIGHT 1024

typedef struct
{
    v2i dim;
    rect3f uvs;
    v2f align;
} XSprite;

typedef struct
{
    u32 unicodeCodePoint;
    XSprite sprite;
} XFontGlyph;

typedef struct
{
    wchar_t path[MAX_PATH];
    HBITMAP bitmap;
    VOID *bytes;
    //
    HFONT handle;
    TEXTMETRICW metrics;
    
    f32 charAvgWidth;
    f32 lineHeight;
    
    XFontGlyph *glyphs;
    f32 *horizontalAdvance;
    
    u32 minCodePoint;
    u32 maxCodePoint;
    
    u32 maxGlyphCount;
    u32 glyphCount;
    
    u32 *glyphIndexFromCodePoint;
    u32 onePastHighestCodepoint;
    
    u8 *atlasBytes;
    XTextureAtlas atlas;
    rect3f atlasUvs;
} XFont;

typedef struct
{
    v2f a;
    v2f b;
    v4f color;
} XRenderCommandLine;

typedef struct
{
    v2f position;
    v2f size;
    v4f color;
} XRenderCommandRect;

typedef struct
{
    v2f position;
    v2f size;
    v4f color;
    XSprite sprite;
} XRenderCommandSprite;

typedef struct
{
    v2f position;
    v4f color;
    XFont *font;
    u32 length;
    wchar_t *text;
} XRenderCommandText;

typedef enum XRenderCommandType
{
    XRenderCommandType_null,
    XRenderCommandType_line,
    XRenderCommandType_rect,
    XRenderCommandType_sprite,
    XRenderCommandType_text,
} XRenderCommandType;

typedef struct XRenderCommand
{
    XRenderCommandType type;
    union
    {
        XRenderCommandLine line;
        XRenderCommandRect rect;
        XRenderCommandSprite sprite;
        XRenderCommandText text;
    };
} XRenderCommand;

typedef struct
{
    XRenderCommand commands[4096];
    u32 command_index;
    
    rect2f *scissor;
} XRenderBatch;

typedef struct
{
    ID3D11Texture2D *texture;
    XD11DepthStencil depth_stencil;
} XRenderTarget;

typedef struct
{
    /* Core variables */
    XRenderTarget target_default;
    
    v4f clear_color;
    
    u32 target_view_count;
    ID3D11RenderTargetView **target_views;
    
    
#if 0
    XD11Texture texture_atlas_texture;
    XTextureAtlas texture_atlas;
#else
    XD11TextureArray textures;
#endif
    
    /* Passes */
    XD11RenderPass pass_lines;
    XD11RenderPass pass_sprites;
} XRender2D;

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

global XRender2D xrender2d;
global HDC globalFontDeviceContext;
global VOID *globalFontBits;

/* Resources */
global XSprite sprite_white;
global XSprite sprite_circle;
global XSprite sprite_arrow;

global XSprite sprite_circle_tl;
global XSprite sprite_circle_tr;
global XSprite sprite_circle_bl;
global XSprite sprite_circle_br;

void xrender2d_initialize (v4f clear_color);
void xrender2d_shutdown   (void);
void xrender2d_pre_update (void);
void xrender2d_post_update(XRenderBatch *array, u32 count);

void draw_line   (XRenderBatch *batch, v2f a,   v2f b,    v4f color);
void draw_arrow  (XRenderBatch *batch, v2f a,   v2f b,    v4f color, f32 head_size);
void draw_rect   (XRenderBatch *batch, v2f pos, v2f size, v4f color);

void draw_sprite (XRenderBatch *batch, v2f pos, v2f size, v4f color, XSprite sprite);

void draw_circle (XRenderBatch *batch, v2f pos, v4f color, f32 radius);

void draw_rect_rounded (XRenderBatch *batch, v2f pos, v2f size, v4f color, f32 radius);

void draw_text   (XRenderBatch *batch, v2f pos, v4f color, XFont *font, wchar_t *text);
void draw_grid   (XRenderBatch *batch, v2f gap, v2f line_thickness, v4f color);

void xrender2d_reset_batch(XRenderBatch *batch);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XFont *  xrender2d_font        (wchar_t *path, wchar_t *name, int heightpoints);
XSprite  xrender2d_font_glyph  (XFont *font, u32 c);
void     xrender2d_font_free   (XFont *font);
v2f      xrender2d_font_dim    (XFont *font, wchar_t *text);

void     xrender2d_break_text  (wchar_t *lines, u32 *lengths, u32 *lineCount, 
                                u32 maxLineCount, u32 maxLineCharCount, 
                                XFont *font, wchar_t *str, f32 maxWidth);




/* =========================================================================
   TEXTURE / TEXTURE ATLAS / SPRITES
   ========================================================================= */

/*  A D11 Batch is composed of many vertices representing triangles texture
mapped to a single gpu texture that is called the texture atlas. A XSprite is a
smaller rectangle than the whole texture atlas that represents a texture in it
self, i.e., the texture atlas is composed of many different smaller textures.
  The point of that is faster performance. xspritepng creates a sprite from a
png file located at path, i.e., it loads the png and copies the bytes of the
texture into the texture atlas and fills XXSprite with the corresponding uv coo
rdinates and other info about the source image. */

u8 *      xrender2d_load_png(wchar_t *path, v2i *dim);
XSprite   xrender2d_sprite_from_png   (wchar_t *path);
XSprite   xrender2d_sprite_from_bytes (u8 *b, v2i dim);

/* =========================================================================
   END OF INTERFACE
   ========================================================================= */













/* =========================================================================
   IMPLEMENTATION INTERFACE
   ========================================================================= */

typedef struct
{
    v3f position;
    v4f color;
} LineVertex;

typedef struct
{
    v3f position;
    v3f uvs;
    v4f color;
} TexturedVertex;

void xrender2d_create_line_pass(void);
void xrender2d_create_sprite_pass(void);
void xrender2d_create_resources(void);
void xrender2d_produce_vertices(XRenderBatch *batch, 
                                Array_T *lineVertices, 
                                Array_T *texVertices);
void xrender2d_update_line_pass(void);
void xrender2d_update_sprite_pass(void);
void xrender2d_free_passes(void);

XRenderCommand *xrender2d_push_command(XRenderBatch *batch);











/* =========================================================================
   IMPLEMENTATION
   ========================================================================= */

function void initialize_font_dc(void);

/* Lines Vertex shader */
char *linesVS = 
"struct vs_in"
"{"
"  float3 position_local : POS;"
"  float4 color          : COL;"
"};"

"struct vs_out"
"{"
"  float4 position_clip : SV_POSITION;"
"  float4 color         : COLOR;"
"};"

"cbuffer cbPerObject"
"{"
"  float4x4 WVP;"
"};"

"vs_out vs_main(vs_in input)"
"{"
"  vs_out output = (vs_out)0;"
"  float4 pos = float4(input.position_local, 1.0);"
"  pos.z += 500;"

"  output.position_clip = mul(pos, WVP);"
"  output.color = input.color;"
"  return output;"
"}";

/* Lines Pixel shader */
char *linesPS = 
"struct ps_in"
"{"
"  float4 position_clip : SV_POSITION;"
"  float4 color         : COLOR;"
"};"

"float4 ps_main(ps_in input) : SV_TARGET"
"{"
"  return input.color;"
"}";

/* Textured Vertex shader */
char *spritesVS = 
"struct vs_in"
"{"
"  float3 position_local : POS;"
"  float3 uv             : TEX;"
"  float4 color          : COL;"
"};"

"struct vs_out"
"{"
"  float4 position_clip : SV_POSITION;"
"  float3 uv            : TEXCOORD;"
"  float4 color         : COLOR;"
"};"

"cbuffer cbPerObject"
"{"
"  float4x4 WVP;"
"};"

"vs_out vs_main(vs_in input)"
"{"
"  vs_out output = (vs_out)0;"
"  float4 pos = float4(input.position_local, 1.0);"
"  pos.z += 500;"

"  output.position_clip = mul(pos, WVP);"
"  output.uv = input.uv;"
"  output.color = input.color;"
"  return output;"
"}";

/* Textured Pixel shader */
char *spritesPS = 
"struct ps_in"
"{"
"  float4 position_clip : SV_POSITION;"
"  float3 uv            : TEXCOORD;"
"  float4 color         : COLOR;"
"};"

"Texture2DArray tex;"
"SamplerState samp;"

"float4 ps_main(ps_in input) : SV_TARGET"
"{"
"  return tex.Sample(samp, input.uv) * input.color;"
"}";

void draw_line(XRenderBatch *batch, v2f a, v2f b, v4f color)
{
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_line;
    command->line.a = a;
    command->line.b = b;
    command->line.color = color;
}

void draw_arrow(XRenderBatch *batch, v2f a, v2f b, v4f color, f32 head_size)
{
    if (a.x != b.x || a.y != b.y)
    {
        draw_line(batch, a, b, color);
        
        v2f dir = nrm2f(sub2f(b, a));
        v2f normal = {-dir.y, dir.x};
        
        v2f back = sub2f(b, mul2f(head_size, dir));
        v2f head_a = add2f(back, mul2f(+head_size, normal));
        v2f head_b = add2f(back, mul2f(-head_size, normal));
        
        draw_line(batch, b, head_a, color);
        draw_line(batch, b, head_b, color);
    }
}

void draw_rect(XRenderBatch *batch, v2f pos, v2f size, v4f color)
{
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_rect;
    command->sprite.position = pos;
    command->sprite.size = size;
    command->sprite.color = color;
}

void draw_sprite(XRenderBatch *batch, v2f pos, v2f size, v4f color, XSprite sprite)
{
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_sprite;
    command->sprite.position = pos;
    command->sprite.size = size;
    command->sprite.color = color;
    command->sprite.sprite = sprite;
}

void
draw_circle(XRenderBatch *batch, v2f pos, v4f color, f32 radius) {
    draw_sprite(batch, 
                sub2f(pos, (v2f){radius,radius}),
                (v2f){2*radius,2*radius}, 
                color, 
                sprite_circle);
}

void
draw_rect_rounded(XRenderBatch *batch, v2f pos, v2f size, v4f color, f32 radius) {
    v2f dim = (v2f){radius,radius};
    draw_sprite(batch, 
                pos, 
                dim, 
                color,
                sprite_circle_tl);
    
    draw_sprite(batch, 
                add2f(pos, (v2f){size.x-radius,0}), 
                dim, 
                color,
                sprite_circle_tr);
    
    draw_sprite(batch, 
                add2f(pos, (v2f){0,size.y-radius}),
                dim, 
                color,
                sprite_circle_bl);
    
    draw_sprite(batch, 
                add2f(pos, (v2f){size.x-radius,size.y-radius}), 
                dim, 
                color,
                sprite_circle_br);
    
    f32 posXPlusRadius = floorf(pos.x + radius);
    f32 posYPlusRadius = floorf(pos.y + radius);
    
    /* Center */
    draw_rect(batch,
              (v2f){posXPlusRadius, pos.y},
              (v2f){floorf(size.x-2*radius)+3,size.y},
              color);
    
    /* Left strip */
    draw_rect(batch,
              (v2f){pos.x, posYPlusRadius},
              (v2f){ceilf(radius),floorf(size.y-2*radius)},
              color);
    
    /* Right strip */
    draw_rect(batch,
              add2f(pos, (v2f){size.x-radius,radius}),
              (v2f){radius,floorf(size.y-2*radius)},
              color);
    
}


void draw_text(XRenderBatch *batch, v2f pos, v4f color, XFont *font, wchar_t *text)
{
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_text;
    command->text.position = pos;
    command->text.color = color;
    command->text.font = font;
    command->text.length = xstrlen(text);
    command->text.text = xstrnew(text);
}

void draw_grid(XRenderBatch *batch, v2f gap, v2f line_thickness, v4f color)
{
    v2i grid_horizontal_line_count = 
    {
        (s32)ceilf(xd11.bbDim.x / gap.x),
        (s32)ceilf(xd11.bbDim.y / gap.y),
    };
    
    /* Draw vertical lines */
    for (s32 x=0; x<grid_horizontal_line_count.x; ++x)
        draw_rect(batch, 
                  (v2f){x*gap.x, 0}, 
                  (v2f){line_thickness.x, xd11.bbDim.y}, 
                  color);
    
    /* Draw horizontal lines */
    for (s32 y=0; y<grid_horizontal_line_count.y; ++y)
        draw_rect(batch, 
                  (v2f){0, y*gap.y}, 
                  (v2f){xd11.bbDim.x, line_thickness.y}, 
                  color);
    
}

void xrender2d_reset_batch(XRenderBatch *batch)
{
    batch->command_index = 0;
}

XRenderCommand *xrender2d_push_command(XRenderBatch *batch)
{
    u32 index = batch->command_index++;
    assert(narray(batch->commands) > index);
    return batch->commands + index;
}

void xrender2d_pre_update(void)
{
    MSG message;
    while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    
    xrender2d_update_line_pass();
    xrender2d_update_sprite_pass();
}

void xrender2d_post_update(XRenderBatch *array, u32 count)
{
    /* For each batch */
    for (u32 i=0; i < count; ++i)
    {
        XRenderBatch *batch = array + i;
        
        Array_T lineVertices = Array_new(32, sizeof(LineVertex));
        Array_T texVertices = Array_new(32, sizeof(TexturedVertex));
        
        /* Produce vertices */
        xrender2d_produce_vertices(batch, &lineVertices, &texVertices);
        
        /* Get C arrays from Array_Ts */ 
        s32 lineVertexCount = 0, texVertexCount = 0;
        LineVertex     *lineVertexArray = 0;
        TexturedVertex *texVertexArray = 0;
        
        Array_toarray(lineVertices, &lineVertexArray, &lineVertexCount);
        Array_toarray(texVertices, &texVertexArray, &texVertexCount);
        
        /* Update vertex buffers */
        xd11_buffer_update(xrender2d.pass_lines.vBuffers.array[0], 
                           lineVertexArray, lineVertexCount*sizeof(LineVertex));
        
        xd11_buffer_update(xrender2d.pass_sprites.vBuffers.array[0], 
                           texVertexArray, texVertexCount*sizeof(TexturedVertex));
        
        /* Execute render passes */
        xd11_render_pass(&xrender2d.pass_lines, lineVertexCount, 
                         batch->scissor, 0, 0);
        
        xd11_render_pass(&xrender2d.pass_sprites, texVertexCount, 
                         batch->scissor, 1, &xrender2d.textures.tex.view);
        
        /* Free C arrays and Array_Ts */
        xfree(lineVertexArray);
        Array_free(&lineVertices);
        
        xfree(texVertexArray);
        Array_free(&texVertices);
    }
    
    
    xd11_update();
    xwin_update(true, xd11.wndDim);
}

void xrender2d_depth_stencil(XD11DepthStencil *depth_stencil,
                             D3D11_TEXTURE2D_DESC desc_texture,
                             D3D11_DEPTH_STENCIL_VIEW_DESC desc_view,
                             D3D11_DEPTH_STENCIL_DESC desc_state)
{
    /* Create depth stencil buffer texture */
    depth_stencil->texture = xd11_texture2d(desc_texture, 0);
    
    /* Create depth stencil view */
    depth_stencil->view = xd11_depth_stencil_view(depth_stencil->texture, desc_view);
    
    /* Create depth stencil state */            
    depth_stencil->state = xd11_depth_stencil_state(desc_state);
}

void xrender2d_initialize(v4f clear_color)
{
    initialize_font_dc();
    
    xrender2d.clear_color = clear_color;
    
    /* Get back buffer texture from swap chain */    
    xrender2d.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    xrender2d.target_view_count = 1;
    xrender2d.target_views = xalloc(1*sizeof(ID3D11RenderTargetView *));
    xrender2d.target_views[0] = xd11_target_view(xrender2d.target_default.texture);
    
    /* Depth stencil */
    xrender2d_depth_stencil(&xrender2d.target_default.depth_stencil,
                            (D3D11_TEXTURE2D_DESC)
                            {
                                (s32)xd11.bbDim.x, // Width
                                (s32)xd11.bbDim.y, // Height
                                0, // Mip levels
                                1, // Array size
                                DXGI_FORMAT_D24_UNORM_S8_UINT, // Format
                                (DXGI_SAMPLE_DESC){1,0}, // Sample desc
                                D3D11_USAGE_DEFAULT, // Usage
                                D3D11_BIND_DEPTH_STENCIL, // Bind flags
                                0, // Cpu access flags
                                0 // Misc flags
                            },
                            (D3D11_DEPTH_STENCIL_VIEW_DESC)
                            {
                                DXGI_FORMAT_D24_UNORM_S8_UINT, // format
                                D3D11_DSV_DIMENSION_TEXTURE2D, // dim
                                .Texture2D = (D3D11_TEX2D_DSV){0}
                            },
                            (D3D11_DEPTH_STENCIL_DESC) 
                            {
                                true, // Depth enable
                                D3D11_DEPTH_WRITE_MASK_ALL, // Depth write mask
                                D3D11_COMPARISON_GREATER_EQUAL, // Depth func
                                false, // Stencil enable
                                0, // Stencil read mask
                                0, // Stencil write mask
                                {0}, // Front face
                                {0}, // Back face
                            });
    
    
    /* XSprite and Lines */
    xrender2d.pass_lines.depthStencil = xrender2d.target_default.depth_stencil;
    xrender2d.pass_sprites.depthStencil = xrender2d.target_default.depth_stencil;
    
    /* Texture array */
    {
        u32 textureArraySide = 512;
        xrender2d.textures.mipLevels  = 9;
        
        /* Create D3D11 texture */
        D3D11_TEXTURE2D_DESC desc =
        {
            textureArraySide, // Width
            textureArraySide, // Height
            xrender2d.textures.mipLevels,  // Mip levels
            32, // Array size
            DXGI_FORMAT_R8G8B8A8_UNORM, // Format
            {1,0}, // Sample
            D3D11_USAGE_DEFAULT, // Usage 
            D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET, // Bind flags
            0, // Cpu access flags
            D3D11_RESOURCE_MISC_GENERATE_MIPS // Misc flags
        };
        xrender2d.textures.tex.handle = xd11_texture2d(desc, 0);
        xrender2d.textures.tex.dim    = fil2i(textureArraySide);
        xrender2d.textures.dim        = fil2i(textureArraySide);
        
        /* Create the shader resource view */
        xrender2d.textures.tex.view = 
            xd11_shader_res_view(xrender2d.textures.tex.handle,
                                 (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                 {
                                     DXGI_FORMAT_R8G8B8A8_UNORM,
                                     D3D_SRV_DIMENSION_TEXTURE2DARRAY,
                                     .Texture2DArray = (D3D11_TEX2D_ARRAY_SRV){0, desc.MipLevels, 0, desc.ArraySize}
                                 });
    }
    
    /* Create passes */
    xrender2d_create_line_pass();
    xrender2d_create_sprite_pass();
    
    /* Create resources */
    xrender2d_create_resources();
}

void xrender2d_shutdown(void) {
    xrender2d_free_passes();
}

void xrender2d_create_line_pass(void)
{
    /* Input format */
    D3D11_INPUT_ELEMENT_DESC inputFormat[] =
    {
        {
            "POS",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0, 
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COL",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0, 
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
    };
    
    /* Vertex shader */
    ID3DBlob *compiledVS;
    xrender2d.pass_lines.vShader = xd11_compile_vshader(linesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    xrender2d.pass_lines.pShader = xd11_compile_pshader(linesPS, &compiledPS);
    
    /* Input layout */
    xrender2d.pass_lines.inputLayout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    xrender2d.pass_lines.targetView = xrender2d.target_views[0];
    
    /* Blend state */
    xrender2d.pass_lines.blendState = xd11_blend_state((D3D11_BLEND_DESC)
                                                       {
                                                           false,
                                                           false,
                                                           .RenderTarget[0] = (D3D11_RENDER_TARGET_BLEND_DESC){
                                                               true,
                                                               D3D11_BLEND_SRC_ALPHA,
                                                               D3D11_BLEND_INV_SRC_ALPHA,
                                                               D3D11_BLEND_OP_ADD,
                                                               D3D11_BLEND_ONE,
                                                               D3D11_BLEND_ZERO,
                                                               D3D11_BLEND_OP_ADD,
                                                               D3D11_COLOR_WRITE_ENABLE_ALL,
                                                           },
                                                       });
    /* Rasterizer state */
    xrender2d.pass_lines.rasterizerState = 
        xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
                              {
                                  D3D11_FILL_SOLID, 
                                  D3D11_CULL_BACK, 
                                  false,
                                  0, 
                                  0, 
                                  0, 
                                  true, 
                                  true, 
                                  false, 
                                  false,
                              });
    
    /* Sampler state */
    xrender2d.pass_lines.samplerState = 
        xd11_sampler_state((D3D11_SAMPLER_DESC)
                           {
                               D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                               D3D11_TEXTURE_ADDRESS_WRAP,
                               D3D11_TEXTURE_ADDRESS_WRAP,
                               D3D11_TEXTURE_ADDRESS_WRAP,
                               D3D11_COMPARISON_NEVER,
                               0,
                               D3D11_FLOAT32_MAX,
                           });
    
    /* Vertex buffers */
    xrender2d.pass_lines.vBuffers.count = 1;
    xrender2d.pass_lines.vBuffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    xrender2d.pass_lines.vBuffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
                                                         {
                                                             XRENDER2D_LINEVERTEX_MAX*sizeof(LineVertex),
                                                             D3D11_USAGE_DYNAMIC, 
                                                             D3D11_BIND_VERTEX_BUFFER,
                                                             D3D11_CPU_ACCESS_WRITE, 
                                                             0, 
                                                             sizeof(LineVertex)
                                                         },
                                                         0);
    
    /* Vertex buffer strides */
    xrender2d.pass_lines.vBuffers.strides = xalloc(1*sizeof(UINT));
    xrender2d.pass_lines.vBuffers.strides[0] = sizeof(LineVertex);
    
    /* Vertex buffer offsets */
    xrender2d.pass_lines.vBuffers.offsets = xalloc(1*sizeof(UINT));
    xrender2d.pass_lines.vBuffers.offsets[0] = 0;
    
    /* Vertex shader constant buffers */
    xrender2d.pass_lines.vShaderCBufferCount = 1;
    xrender2d.pass_lines.vShaderCBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    
    xrender2d.pass_lines.vShaderCBuffers[0] = 
        xd11_buffer((D3D11_BUFFER_DESC)
                    {
                        sizeof(mat4f),
                        D3D11_USAGE_DEFAULT, 
                        D3D11_BIND_CONSTANT_BUFFER,
                        0,
                        0,
                        0,
                    },
                    0);
    
    /* Topology */
    xrender2d.pass_lines.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    
    /* Viewports */
    xrender2d.pass_lines.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&xrender2d.pass_lines.viewports, 
               &(D3D11_VIEWPORT){0,0,xd11.bbDim.x,xd11.bbDim.y, 0, 1});
}

void xrender2d_create_sprite_pass(void)
{
    /* Input format */
    D3D11_INPUT_ELEMENT_DESC inputFormat[] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "TEX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    /* Vertex shader */
    ID3DBlob *compiledVS;
    xrender2d.pass_sprites.vShader = 
        xd11_compile_vshader(spritesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    xrender2d.pass_sprites.pShader = 
        xd11_compile_pshader(spritesPS, &compiledPS);
    
    /* Input layout */
    xrender2d.pass_sprites.inputLayout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    xrender2d.pass_sprites.targetView = xrender2d.target_views[0];
    
    /* Blend state */
    xrender2d.pass_sprites.blendState = xd11_blend_state((D3D11_BLEND_DESC)
                                                         {
                                                             false,
                                                             false,
                                                             .RenderTarget[0] = (D3D11_RENDER_TARGET_BLEND_DESC){
                                                                 true,
                                                                 D3D11_BLEND_SRC_ALPHA,
                                                                 D3D11_BLEND_INV_SRC_ALPHA,
                                                                 D3D11_BLEND_OP_ADD,
                                                                 D3D11_BLEND_ONE,
                                                                 D3D11_BLEND_ZERO,
                                                                 D3D11_BLEND_OP_ADD,
                                                                 D3D11_COLOR_WRITE_ENABLE_ALL,
                                                             },
                                                         });
    
    /* Rasterizer state */
    xrender2d.pass_sprites.rasterizerState = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
                                                                   {
                                                                       // D3D11_FILL_WIREFRAME,
                                                                       D3D11_FILL_SOLID, 
                                                                       D3D11_CULL_BACK, 
                                                                       false,
                                                                       0, 
                                                                       0, 
                                                                       0, 
                                                                       true, 
                                                                       true, 
                                                                       false, 
                                                                       false,
                                                                   });
    
    /* Sampler state */
    xrender2d.pass_sprites.samplerState = xd11_sampler_state(xd11_sampler_desc());
    
    /* Vertex buffers */
    xrender2d.pass_sprites.vBuffers.count = 1;
    xrender2d.pass_sprites.vBuffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    xrender2d.pass_sprites.vBuffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
                                                           {
                                                               XRENDER2D_TEXTUREDVERTEX_MAX*sizeof(TexturedVertex), 
                                                               D3D11_USAGE_DYNAMIC, 
                                                               D3D11_BIND_VERTEX_BUFFER,
                                                               D3D11_CPU_ACCESS_WRITE, 
                                                               0, 
                                                               sizeof(TexturedVertex)
                                                           },
                                                           0);
    /* Vertex buffers strides */
    xrender2d.pass_sprites.vBuffers.strides = xalloc(1*sizeof(UINT));
    xrender2d.pass_sprites.vBuffers.strides[0] = sizeof(TexturedVertex);
    
    /* Vertex buffers offsets */
    xrender2d.pass_sprites.vBuffers.offsets = xalloc(1*sizeof(UINT));
    xrender2d.pass_sprites.vBuffers.offsets[0] = 0;
    
    /* Vertex Shader Constant buffers */
    xrender2d.pass_sprites.vShaderCBufferCount = 1;
    xrender2d.pass_sprites.vShaderCBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    xrender2d.pass_sprites.vShaderCBuffers[0] = 
        xd11_buffer((D3D11_BUFFER_DESC)
                    {
                        sizeof(mat4f),
                        D3D11_USAGE_DEFAULT, 
                        D3D11_BIND_CONSTANT_BUFFER,
                        0,
                        0,
                        0,
                    },
                    0);
    
    /* Topology */
    xrender2d.pass_sprites.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    /* Viewports */
    xrender2d.pass_sprites.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&xrender2d.pass_sprites.viewports,
               &(D3D11_VIEWPORT){0,0,xd11.bbDim.x,xd11.bbDim.y, 0, 1});
}

void xrender2d_create_resources(void)
{
    /* Create resources */
    
    /* white.png */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"images/white.png");
        sprite_white = xrender2d_sprite_from_png(full_path);
    }
    
    /* circle.png */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"images/circle.png");
        sprite_circle = xrender2d_sprite_from_png(full_path);
    }
    
    /* arrow.png */
    {
        wchar_t full_path[512];
        xwin_path_abs(full_path, 512, L"images/arrow.png");
        sprite_arrow = xrender2d_sprite_from_png(full_path);
    }
    
    /* Make circle quadrants */
    s32 circleWidth = sprite_circle.dim.x;
    s32 circleHeight = sprite_circle.dim.y;
    f32 circleQuadrantWidth = circleWidth/2.0f;
    f32 circleQuadrantHeight = circleHeight/2.0f;
    
    f32 circleUVWidth = sprite_circle.uvs.max.x - sprite_circle.uvs.min.x;
    f32 circleUVHeight = sprite_circle.uvs.max.y - sprite_circle.uvs.min.y;
    
    f32 circleQuadrantUVWidth = circleUVWidth/2;
    f32 circleQuadrantUVHeight = circleUVHeight/2;
    
    sprite_circle_tl.uvs.min = (v3f){sprite_circle.uvs.min.x, sprite_circle.uvs.min.y, sprite_circle.uvs.min.z};
    sprite_circle_tl.uvs.max = (v3f){
        sprite_circle.uvs.min.x + circleQuadrantUVWidth, 
        sprite_circle.uvs.min.y + circleQuadrantUVHeight,
        sprite_circle.uvs.min.z
    };
    
    sprite_circle_bl.uvs.min = (v3f){sprite_circle.uvs.min.x, sprite_circle.uvs.min.y + circleQuadrantUVHeight};
    sprite_circle_bl.uvs.max = (v3f){sprite_circle.uvs.min.x + circleQuadrantUVWidth, sprite_circle.uvs.max.y};
    
    sprite_circle_br.uvs.min = (v3f){
        sprite_circle.uvs.min.x + circleQuadrantUVWidth, 
        sprite_circle.uvs.min.y + circleQuadrantUVHeight,
        sprite_circle.uvs.min.z
    };
    sprite_circle_br.uvs.max = (v3f){sprite_circle.uvs.max.x, sprite_circle.uvs.max.y, sprite_circle.uvs.min.z};
    
    sprite_circle_tr.uvs.min = (v3f){
        sprite_circle.uvs.min.x + circleQuadrantUVWidth, 
        sprite_circle.uvs.min.y,
        sprite_circle.uvs.min.z
    };
    sprite_circle_tr.uvs.max = (v3f){
        sprite_circle.uvs.max.x, 
        sprite_circle.uvs.min.y + circleQuadrantUVHeight,
        sprite_circle.uvs.min.z};
    
    //xd11_texture2d_update(&xrender2d.texture_atlas.texture, xrender2d.texture_atlas.bytes);
}

void xrender2d_free_passes(void)
{
    // Lines
    xfree(xrender2d.pass_lines.vBuffers.array);
    xfree(xrender2d.pass_lines.vBuffers.strides);
    xfree(xrender2d.pass_lines.vBuffers.offsets);
    xfree(xrender2d.pass_lines.vShaderCBuffers);
    Array_free(&xrender2d.pass_lines.viewports);
    
    // XSprites
    xfree(xrender2d.pass_sprites.vBuffers.array);
    xfree(xrender2d.pass_sprites.vBuffers.strides);
    xfree(xrender2d.pass_sprites.vBuffers.offsets);
    xfree(xrender2d.pass_sprites.vShaderCBuffers);
    Array_free(&xrender2d.pass_sprites.viewports);
    
    // Core
    xfree(xrender2d.target_views);
}

void xrender2d_push_rect_vertices(Array_T *array, v2f pos, v2f size, rect3f uv, v4f color)
{
    /* Upper triangle */
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x, pos.y, 0}, 
                   (v3f){uv.min.x, uv.min.y, uv.min.z}, 
                   color});
    
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x + size.x, pos.y, 0}, 
                   (v3f){uv.max.x, uv.min.y, uv.min.z}, 
                   color});
    
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x + size.x, pos.y + size.y, 0}, 
                   (v3f){uv.max.x, uv.max.y, uv.min.z}, 
                   color});
    
    /* Lower triangle */
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x, pos.y, 0}, 
                   (v3f){uv.min.x, uv.min.y, uv.min.z}, 
                   color});
    
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x + size.x, pos.y + size.y, 0}, 
                   (v3f){uv.max.x, uv.max.y, uv.min.z}, 
                   color});
    
    Array_push(array, &(TexturedVertex){
                   (v3f){pos.x, pos.y + size.y, 0}, 
                   (v3f){uv.min.x, uv.max.y, uv.min.z}, 
                   color});
}

void xrender2d_produce_vertices(XRenderBatch *batch, 
                                Array_T *lineVertices, 
                                Array_T *texVertices)
{
    for (u32 i=0; i < batch->command_index; ++i)
    {
        XRenderCommand *c = batch->commands + i;
        switch (c->type)
        {
            case XRenderCommandType_line:
            {
                Array_push(lineVertices, 
                           &(LineVertex){(v3f){c->line.a.x, c->line.a.y, 0}, c->line.color});
                Array_push(lineVertices, 
                           &(LineVertex){(v3f){c->line.b.x, c->line.b.y, 0}, c->line.color});
            } break;
            
            case XRenderCommandType_rect:
            {
                v2f pos = c->rect.position;
                v2f size = c->rect.size;
                v4f color = c->rect.color;
                rect3f uvs = sprite_white.uvs;
                xrender2d_push_rect_vertices(texVertices, pos, size, uvs, color);
            } break;
            
            case XRenderCommandType_sprite:
            {
                v2f pos = c->sprite.position;
                v2f size = c->sprite.size;
                rect3f uvs = c->sprite.sprite.uvs;
                v4f color = c->sprite.color;
                xrender2d_push_rect_vertices(texVertices, pos, size, uvs, color);
            } break;
            
            case XRenderCommandType_text:
            {
                u32 lastCodePoint = 0;
                wchar_t *at = c->text.text;
                v4f color = c->text.color;
                v2f pos = c->text.position;
                while (*at != 0) {
                    XFontGlyph *glyph = c->text.font->glyphs + c->text.font->glyphIndexFromCodePoint[*at];
                    if (glyph) {
                        XSprite *sprite = &glyph->sprite;
                        v2i dim = sprite->dim;
                        rect3f uvs = sprite->uvs;
                        u32 codePoint = (u32)*at;
                        
                        /* Advance based on kerning */
                        if (lastCodePoint) {
                            pos.x += c->text.font->horizontalAdvance[lastCodePoint*c->text.font->maxGlyphCount + codePoint];
                        }
                        
                        /* glyph alignments */
                        v2f glyph_pos = pos;
                        glyph_pos.x -= sprite->align.x*dim.x;
                        
                        glyph_pos.y -= dim.y - sprite->align.y*dim.y;
                        glyph_pos.y += c->text.font->metrics.tmAscent;
                        
                        xrender2d_push_rect_vertices(texVertices, glyph_pos, (v2f){(f32)dim.x,(f32)dim.y}, uvs, color);
                        lastCodePoint = codePoint;
                    }
                    
                    ++at;
                }
                
                xfree(c->text.text);
                
            } break;
            
            default:
            {
                assert(!"Should not happen!");
            } break;
        }
    }
}

void xrender2d_update_line_pass(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.bbDim.x, 0, 0, -1,
        0, -2.0f / xd11.bbDim.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    /* Update constant buffers */
    ID3D11Buffer *cbuffer = xrender2d.pass_lines.vShaderCBuffers[0];
    xd11_update_subres(cbuffer, &matrixProjection);
    
    /* Set render target */
    xd11_set_render_target(xrender2d.pass_lines.targetView, xrender2d.pass_lines.depthStencil.view);
    
    /* Clear */
    xd11_clear_rtv(xrender2d.pass_lines.targetView, xrender2d.clear_color);
    xd11_clear_dsv(xrender2d.pass_lines.depthStencil.view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    /* Update viewport */
    Array_set(xrender2d.pass_lines.viewports, 0, 
              &(D3D11_VIEWPORT){0,0,xd11.bbDim.x,xd11.bbDim.y, 0, 1});
    
    
    LineVertex data[2] =
    {
        {{0,0,0}, {1,1,1,1}},
        {{xd11.bbDim.x,xd11.bbDim.y,0}, {1,1,1,1}},
    };
}

void xrender2d_update_sprite_pass(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.bbDim.x, 0, 0, -1,
        0, -2.0f / xd11.bbDim.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xd11_update_subres(xrender2d.pass_sprites.vShaderCBuffers[0], &matrixProjection);
    
    /* Update viewport and scissor rect*/
    Array_set(xrender2d.pass_sprites.viewports, 0,
              &(D3D11_VIEWPORT){0,0,xd11.bbDim.x,xd11.bbDim.y, 0, 1});
}

void xd11_resized(void)
{
    /* Release target views */
    for (u32 i=0; i<xrender2d.target_view_count; ++i)
        ID3D11RenderTargetView_Release((ID3D11RenderTargetView *)xrender2d.target_views[i]);
    
    /* Default Target Depth Stencil Texture and View */
    ID3D11DepthStencilView_Release(xrender2d.target_default.depth_stencil.view);
    ID3D11Texture2D_Release       (xrender2d.target_default.depth_stencil.texture);
    
    /* Lines and XSprites passes pointers */
    xrender2d.pass_lines.depthStencil.view = 0;
    xrender2d.pass_lines.depthStencil.texture = 0;
    xrender2d.pass_sprites.depthStencil.view = 0;
    xrender2d.pass_sprites.depthStencil.texture = 0;
    
    /* Default Target Texture */
    ID3D11Texture2D_Release(xrender2d.target_default.texture);
    
    /* Resize the swapchain buffers */
    IDXGISwapChain_ResizeBuffers(xd11.swapChain,
                                 2, 
                                 (UINT)xd11.bbDim.x,
                                 (UINT)xd11.bbDim.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get default target backbuffer texture from swap chain */
    xrender2d.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create default target depth stencil texture */
    xrender2d.target_default.depth_stencil.texture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
                                                                    {
                                                                        (s32)xd11.bbDim.x,
                                                                        (s32)xd11.bbDim.y,
                                                                        0, 
                                                                        1,
                                                                        DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                                        (DXGI_SAMPLE_DESC){1,0}, 
                                                                        D3D11_USAGE_DEFAULT,
                                                                        D3D11_BIND_DEPTH_STENCIL, 
                                                                        0, 
                                                                        0
                                                                    },
                                                                    0);
    
    /* Create depth stencil view */
    xrender2d.target_default.depth_stencil.view = 
        xd11_depth_stencil_view(xrender2d.target_default.depth_stencil.texture,
                                (D3D11_DEPTH_STENCIL_VIEW_DESC)
                                {
                                    DXGI_FORMAT_D24_UNORM_S8_UINT, 
                                    D3D11_DSV_DIMENSION_TEXTURE2D, 
                                    .Texture2D = (D3D11_TEX2D_DSV){0}
                                });
    
    /* Lines and XSprites passes pointers */
    xrender2d.pass_lines.depthStencil.view = xrender2d.target_default.depth_stencil.view;
    xrender2d.pass_lines.depthStencil.texture = xrender2d.target_default.depth_stencil.texture;
    xrender2d.pass_sprites.depthStencil.view = xrender2d.target_default.depth_stencil.view;
    xrender2d.pass_sprites.depthStencil.texture = xrender2d.target_default.depth_stencil.texture;
    
    /* Create  render target view */
    xrender2d.target_views[0] = xd11_target_view(xrender2d.target_default.texture);
    
    /* Lines and XSprites passes Target view pointers */
    xrender2d.pass_sprites.targetView = xrender2d.target_views[0];
    xrender2d.pass_lines.targetView = xrender2d.target_views[0];
}

u8 *xrender2d_load_png(wchar_t *path, v2i *dim) {
    u8 *r, *rowDst, *rowSrc;
    int w,h, i,j, nrChannels, dataSize;
    char *asciiPath;
    u32 *pxDst, *pxSrc, cr,cg,cb,ca;
    
    r=0;
    asciiPath = xstrtoascii(path);
    unsigned char *data = stbi_load(asciiPath, &w, &h, &nrChannels, 0);
    xfree(asciiPath);
    if (data)
    {
        *dim = (v2i){w,h};
        
        dataSize = w*h*nrChannels;
        r = (u8 *)xalloc(dataSize);
        rowDst = r;
        rowSrc = data;
        
        for (j=0; j<h; ++j)
        {
            pxDst = (u32 *)rowDst;
            pxSrc = (u32 *)rowSrc;
            for (i=0; i<w; ++i)
            {
                cr = (*pxSrc >> 16) & 0xFF;
                cg = (*pxSrc >> 8) & 0xFF;
                cb = (*pxSrc >> 0) & 0xFF;
                ca = (*pxSrc >> 24) & 0xFF;
#if 0
                if (premulalpha)
                {
                    f32 realA = (f32)ca / 255.0f;
                    cr=(s32)(cr*realA);
                    cg=(s32)(cg*realA);
                    cb=(s32)(cb*realA);
                }
#endif
                *pxDst = ((cr<<16) | (cg<<8) | (cb<<0) | (ca<<24));
                pxDst++;
                pxSrc++;
            }
            rowDst += w*4;
            rowSrc += w*4;
        }
        stbi_image_free(data);
    }
    else
        *dim = (v2i){0,0};
    
    return r;
}

XSprite xrender2d_sprite_from_bytes(u8 *b, v2i dim) {
    XSprite r = {0};
    r.dim = dim;
    r.uvs = xd11_texarray_put(&xrender2d.textures, b, dim);
    return r;
}

XSprite xrender2d_sprite_from_png(wchar_t *path) {
    XSprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = xrender2d_load_png(path, &dim);
    if (b)
    {
        r = xrender2d_sprite_from_bytes(b, dim);
        xfree(b);
    }
    return r;
}

XFont *
xrender2d_font(wchar_t *path, wchar_t *name, s32 pixelHeight)
{
    XFont *result = xalloc(sizeof *result);
    
    /* Init the atlas */
    v2i atlasDim = xrender2d.textures.dim;
    s32 uvZ = xrender2d.textures.at;
    xtexture_atlas_init(&result->atlas, atlasDim, uvZ);
    /* Allocate space for the cpu bytes */
    result->atlasBytes = xalloc(atlasDim.x*atlasDim.y*4);
    
    AddFontResourceExW(path, FR_PRIVATE, 0);
    result->handle = CreateFontW(pixelHeight, 0, 0, 0,
                                 FW_NORMAL, 
                                 false, 
                                 false, 
                                 false,
                                 DEFAULT_CHARSET,
                                 OUT_DEFAULT_PRECIS, 
                                 CLIP_DEFAULT_PRECIS, 
                                 CLEARTYPE_QUALITY,
                                 DEFAULT_PITCH|FF_DONTCARE,
                                 name);
    assert(result->handle && result->handle != INVALID_HANDLE_VALUE);
    
    SelectObject(globalFontDeviceContext, result->handle);
    
    
    SelectObject(globalFontDeviceContext, result->bitmap);
    GetTextMetricsW(globalFontDeviceContext, &result->metrics);
    
    result->charAvgWidth = (f32)result->metrics.tmAveCharWidth;
    result->lineHeight = (f32)result->metrics.tmAscent + (f32)result->metrics.tmDescent;
    
    result->minCodePoint = INT_MAX;
    result->maxCodePoint = 0;
    
    result->maxGlyphCount = 350;
    result->glyphCount = 0;
    
    u32 glyphIndexFromCodePointSize = ONE_PAST_MAX_FONT_CODEPOINT*sizeof(u32);
    result->glyphIndexFromCodePoint = (u32 *)xalloc(glyphIndexFromCodePointSize);
    memset(result->glyphIndexFromCodePoint, 0, glyphIndexFromCodePointSize);
    
    result->glyphs = (XFontGlyph *)xalloc(sizeof(XFontGlyph)*result->maxGlyphCount);
    u32 horizontalAdvanceSize = sizeof(f32)*result->maxGlyphCount*result->maxGlyphCount;
    result->horizontalAdvance = (f32 *)xalloc(horizontalAdvanceSize);
    memset(result->horizontalAdvance, 0, horizontalAdvanceSize);
    
    result->onePastHighestCodepoint = 0;
    
    // NOTE(casey): Reserve space for the null glyph
    result->glyphCount = 1;
    result->glyphs[0].unicodeCodePoint = 0;
    
    /* D11 some glyphs from the ASCII range */
    for (u32 i=32; i<=255; ++i)
    {
        result->glyphIndexFromCodePoint[i] = i;
        result->glyphs[i].unicodeCodePoint = i;
        result->glyphs[i].sprite = xrender2d_font_glyph(result, i);
    }
    
    /* Put the texture into the array */
    result->atlasUvs = xd11_texarray_put(&xrender2d.textures, 
                                         result->atlasBytes, 
                                         atlasDim);
    
    xfree(result->atlasBytes);
    
    xstrcpy(result->path, MAX_PATH, path);
    
    /* FinalizeFontKerning */
    DWORD KerningPairCount = GetKerningPairsW(globalFontDeviceContext, 0, 0);
    
    KERNINGPAIR *KerningPairs = (KERNINGPAIR *)xalloc(KerningPairCount*sizeof(KERNINGPAIR));
    
    GetKerningPairsW(globalFontDeviceContext, KerningPairCount, KerningPairs);
    
    for (DWORD KerningPairIndex = 0;
         KerningPairIndex < KerningPairCount;
         ++KerningPairIndex)
    {
        KERNINGPAIR *Pair = KerningPairs + KerningPairIndex;
        if ((Pair->wFirst < ONE_PAST_MAX_FONT_CODEPOINT) &&
            (Pair->wSecond < ONE_PAST_MAX_FONT_CODEPOINT))
        {
            u32 First = result->glyphIndexFromCodePoint[Pair->wFirst];
            u32 Second = result->glyphIndexFromCodePoint[Pair->wSecond];
            if ((First != 0) && (Second != 0))
            {
                result->horizontalAdvance[First*result->maxGlyphCount + Second] += (f32)Pair->iKernAmount;
            }
        }
    }    
    
    xfree(KerningPairs);
    
    return result;
}

function void
initialize_font_dc(void)
{
    globalFontDeviceContext = CreateCompatibleDC(GetDC(0));
    
    BITMAPINFO Info = {0};
    Info.bmiHeader.biSize = sizeof(Info.bmiHeader);
    Info.bmiHeader.biWidth = MAX_FONT_WIDTH;
    Info.bmiHeader.biHeight = MAX_FONT_HEIGHT;
    Info.bmiHeader.biPlanes = 1;
    Info.bmiHeader.biBitCount = 32;
    Info.bmiHeader.biCompression = BI_RGB;
    Info.bmiHeader.biSizeImage = 0;
    Info.bmiHeader.biXPelsPerMeter = 0;
    Info.bmiHeader.biYPelsPerMeter = 0;
    Info.bmiHeader.biClrUsed = 0;
    Info.bmiHeader.biClrImportant = 0;
    HBITMAP Bitmap = CreateDIBSection(globalFontDeviceContext, &Info, DIB_RGB_COLORS, &globalFontBits, 0, 0);
    SelectObject(globalFontDeviceContext, Bitmap);
    SetBkColor(globalFontDeviceContext, RGB(0, 0, 0));
}

inline v4f
SRGB255ToLinear1(v4f C)
{
    v4f Result;
    
    f32 Inv255 = 1.0f / 255.0f;
    
    Result.r = sq(Inv255*C.r);
    Result.g = sq(Inv255*C.g);
    Result.b = sq(Inv255*C.b);
    Result.a = Inv255*C.a;
    
    return(Result);
}

inline v4f
Linear1ToSRGB255(v4f C)
{
    v4f Result;
    
    f32 One255 = 255.0f;
    
    Result.r = One255*sqrtf(C.r);
    Result.g = One255*sqrtf(C.g);
    Result.b = One255*sqrtf(C.b);
    Result.a = One255*C.a;
    
    return(Result);
}

void blit_unchecked(u8 *dst, v2i dest_size, u8 *src, v2i at, v2i dim)
{
    u8 *rowSrc, *rowDst;
    u32 *pxSrc, *pxDst;
    
    u32 dstStride = dest_size.x*4;
    
    rowSrc=src;
    rowDst=dst + at.y*dstStride + at.x*4;
    
    for (at.y=0; at.y<dim.y; ++at.y)
    {
        pxSrc=(u32 *)rowSrc;
        pxDst=(u32 *)rowDst;
        
        for (at.x=0; at.x<dim.x; ++at.x)
            *pxDst++ = *pxSrc++;
        
        rowSrc += 4*dim.x;
        rowDst += dstStride;
    }
}


XSprite xrender2d_font_glyph(XFont *font, u32 codePoint) {
    v2f alignPercentage;
    XSprite r = {0};
    
    if (font->minCodePoint > codePoint)
        font->minCodePoint = codePoint;
    
    if (font->maxCodePoint < codePoint)
        font->maxCodePoint = codePoint;
    
    u32 glyphIndex = font->glyphIndexFromCodePoint[codePoint];
    memset(globalFontBits, 0x00, MAX_FONT_WIDTH*MAX_FONT_HEIGHT*sizeof(u32));
    
    wchar_t cheesePoint = (wchar_t)codePoint;
    
    SIZE size;
    GetTextExtentPoint32W(globalFontDeviceContext, &cheesePoint, 1, &size);
    
    s32 preStepX = 128;
    
    s32 boundWidth = size.cx + 2*preStepX;
    if (boundWidth > MAX_FONT_WIDTH)
    {
        boundWidth = MAX_FONT_WIDTH;
    }
    
    int boundHeight = size.cy;
    if (boundHeight > MAX_FONT_HEIGHT)
    {
        boundHeight = MAX_FONT_HEIGHT;
    }
    
    SetTextColor(globalFontDeviceContext, RGB(255, 255, 255));
    TextOutW(globalFontDeviceContext, preStepX, 0, &cheesePoint, 1);
    
    s32 minX = 10000;
    s32 minY = 10000;
    s32 maxX = -10000;
    s32 maxY = -10000;
    
    u32 *row = (u32 *)globalFontBits + (MAX_FONT_HEIGHT - 1)*MAX_FONT_WIDTH;
    for(s32 y=0; y<boundHeight; ++y) {
        u32 *pixel = row;
        for (s32 x=0; x<boundWidth; ++x) {
            if (*pixel != 0) {
                if (minX > x) minX = x;
                if (minY > y) minY = y;
                if (maxX < x) maxX = x;
                if (maxY < y) maxY = y;
            }
            ++pixel;
        }
        row -= MAX_FONT_WIDTH;
    }
    
    f32 kerningChange = 0;
    if (minX <= maxX) {
        int width = (maxX - minX) + 1;
        int height = (maxY - minY) + 1;
        
        int resultWidth = width + 2;
        int resultHeight = height + 2;
        
        r.dim = (v2i){resultWidth, resultHeight};
        s32 pitch = resultWidth * 4;
        void *memory = xalloc(resultHeight*pitch);
        
        /* For some reason Casey decided to make this overcomplicated
;  scenario where he fills the bitmap bottom up, so I can't
 ;  understand this and gave up on trying to flip it. */
        u8 *destRow = (u8 *)memory + (resultHeight-1-1)*pitch;
        u32 *sourceRow = (u32 *)globalFontBits + (MAX_FONT_HEIGHT-1-minY)*MAX_FONT_WIDTH;
        for (s32 y=minY; y<=maxY; ++y) {
            u32 *source = (u32 *)sourceRow + minX;
            u32 *dest = (u32 *)destRow + 1;
            for (s32 x=minX; x<=maxX; ++x) {
                u32 pixel = *source;
                f32 gray = (f32)(pixel & 0xFF);
                v4f texel = {255.0f, 255.0f, 255.0f, gray};
                //texel = SRGB255ToLinear1(texel);
                //texel.rgb *= texel.a;
                //texel = Linear1ToSRGB255(texel);
                *dest++ = (((u32)(texel.a + 0.5f) << 24) |
                           ((u32)(texel.r + 0.5f) << 16) |
                           ((u32)(texel.g + 0.5f) << 8) |
                           ((u32)(texel.b + 0.5f) << 0));
                ++source;
            }
            
            destRow -= pitch;
            sourceRow -= MAX_FONT_WIDTH;
        }
        
        alignPercentage.x = (1.0f) / (f32)resultWidth;
        alignPercentage.y = (1.0f + (maxY - (boundHeight - font->metrics.tmDescent))) /
        (f32)resultHeight;
        
        kerningChange = (f32)(minX - preStepX);
        
        XTextureAtlasCoords coords = xtexture_atlas_put(&font->atlas, r.dim);
        r.uvs = coords.uvs;
        
        blit_unchecked(font->atlasBytes, font->atlas.dim,
                       memory, coords.at, r.dim);
        
        /* Continue the madness by flipping the uvs */
        f32 tempMinY = r.uvs.min.y;
        r.uvs.min.y = r.uvs.max.y;
        r.uvs.max.y = tempMinY;
        
        r.align = alignPercentage;
        
        xfree(memory);
        
    }
    
    INT thisWidth;
    GetCharWidth32W(globalFontDeviceContext, codePoint, codePoint, &thisWidth);
    f32 charAdvance = (f32)thisWidth;
    
    for (u32 otherGlyphIndex = 0;
         otherGlyphIndex < font->maxGlyphCount;
         ++otherGlyphIndex) {
        
        font->horizontalAdvance[glyphIndex*font->maxGlyphCount + otherGlyphIndex] 
            += charAdvance - kerningChange;
        
        if (otherGlyphIndex != 0) {
            font->horizontalAdvance[otherGlyphIndex*font->maxGlyphCount + glyphIndex]
                += kerningChange;
        }
    }
    
    return r;
}

void xrender2d_font_free(XFont *f)
{
    xfree(f->glyphs);
    xfree(f->horizontalAdvance);
    xfree(f->glyphIndexFromCodePoint);
    
    DeleteObject(f->bitmap);
    DeleteObject(f->handle);
    RemoveFontResourceW(f->path);
    
    xfree(f);
}

v2f
xrender2d_font_dim(XFont *font, wchar_t *text) {
    u32 lastCodePoint = 0;
    wchar_t *at = text;
    v2f atP = (v2f){0,font->lineHeight};
    while (*at != 0)
    {
        u32 codePoint = (u32)*at;
        if (lastCodePoint) 
            atP.x += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + codePoint];
        lastCodePoint = codePoint;
        ++at;
    }
    
    atP.x += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + 0];
    
    return (v2f){atP.x, atP.y};
}


/* NOTE: It is assumed that lines buffer will hold maxLineCount strings of maxLineCharCount length.
;        Is is assumed that maxLineCharCount is big enough to fit a line as big as maxWidth.
;        This function will not break lines based on maxLineCharCount!
;
;        Each line will break at the last space before the next word would make the width of the line
;        be past maxWidth. Lines will hold an array with the resulting lines and lineCount will hold
;        the number of lines the string got broken into. */

void xrender2d_break_text(wchar_t *lines, u32 *lengths, u32 *lineCount,
                          u32 maxLineCount, u32 maxLineCharCount,
                          XFont *font, wchar_t *str, f32 maxWidth) {
    wchar_t *lastSpace = 0;
    u32 lastCodePoint = 0;
    wchar_t *at = str;
    u32 lineCharIndex = 0;
    u32 lineLastSpace = 0;
    
    *lineCount = 0;
    memset(lines, 0, maxLineCount*maxLineCharCount*sizeof(wchar_t));
    
    /* Break text into lines */
    v2f p = {0,0};
    wchar_t *lineStart = at;
    while (*at != 0) {
        u32 codePoint = (u32)*at;
        
        /* If there was a glyph before this, advance the position based on kerning */
        if (lastCodePoint)
            p.x += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + codePoint];
        
        /* If the position is greated than the maximum width allowed */
        if (p.x > maxWidth) {
            /* Reset the x position and advance the y position */
            p.x = 0;
            
            /* Return the iterator to one past the last space position */
            at = lastSpace;
            ++at;
            
            /* Update the code point */
            codePoint = (u32)*at;
            
            /* Make current line stop at last space */
            *(lines + *lineCount * maxLineCharCount + (lineLastSpace-1)) = 0; 
            
            /* Save the length */
            lengths[*lineCount] = (u32)(at - lineStart);
            lineStart = at;
            
            /* Advance to next line */
            *lineCount = *lineCount + 1;
            
            lineCharIndex = 0;
        }
        
        *(lines + *lineCount * maxLineCharCount + lineCharIndex++) = *at;
        
        /* Save the lastSpace position */
        if (codePoint == 32) {
            lastSpace = at;
            lineLastSpace = lineCharIndex;
        }
        
        /* Save codePoint */
        lastCodePoint = codePoint;
        
        ++at;
    }
    
    /* Save the last length */
    lengths[*lineCount] = (u32)(at - lineStart);
    
    *lineCount = *lineCount + 1;
}


#endif