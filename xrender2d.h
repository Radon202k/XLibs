#ifndef XRENDER2D_H
#define XRENDER2D_H

/* =========================================================================
   USAGE
   ========================================================================= */

/*

// Load Font from disk
{
    char full_path[512];
    xwin_path_abs(full_path, 512, "fonts\\Inconsolata.ttf");
    font = xrender2d_font(&xrender2d.texture_atlas,
                         full_path, "Inconsolata", 64);
}

// Load PNG from disk and make Sprite in Texture Atlas
{
    char full_path[512];
    xwin_path_abs(full_path, 512, "images\\naruto.png");
    sprite_naruto = xrender2d_sprite_from_png(&xrender2d.texture_atlas,
                                             full_path, false);
}

// Draw Line
draw_line(&layer1, (v2){0,0}, xwin.mouse.pos, (v4){1,1,1,1});

// Draw Rect
{
    v2 s = {200,20};
    v2 p = add2f(xwin.mouse.pos, mul2f(-.5f,s));
    draw_rect(&layer1, p, s, (v4){1,0,0,1});
}

// Draw Sprite
{
    v2 s = sprite_naruto.size;
    v2 p = add2f(xwin.mouse.pos, mul2f(-.5f,s));
    draw_sprite(&layer1, p, s, (v4){1,1,1,1}, sprite_naruto);
}

// Draw Text
{
    v2 p = {0,0};
    draw_text(&layer1, p, (v4){1,0,0,1}, &font, "Test");
}

// Example draw board
void draw_board(v2 board_size, f32 line_thickness)
{
    v2 board_pos = sub2f(mul2f(.5f, xd11.bbDim), mul2f(.5f,board_size));
    v2i line_count = {9,9};
    v2 line_size = {line_thickness,board_size[1]+line_thickness};
    v2 line_space = (v2){board_size[0]/(line_count[0]-1), board_size[1]/(line_count[1]-1)};
    for (s32 x=0; x<line_count[0]; ++x)
        draw_rect(&layer1, 
                  add2f(board_pos, (v2){x*line_space[0], 0}),
                  line_size, (v4){0,0,0,1});
    
    line_size = (v2){board_size[0]+line_thickness,line_thickness};
    for (s32 y=0; y<line_count[1]; ++y)
        draw_rect(&layer1, 
                  add2f(board_pos, (v2){0, y*line_space[1]}),
                  line_size, (v4){0,0,0,1});
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
    Rect3 uvs;
    v2 align;
} XSprite;

typedef struct
{
    u32 unicodeCodePoint;
    XSprite sprite;
} XFontGlyph;

typedef struct
{
    char path[MAX_PATH];
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
    Rect3 atlasUvs;
} XFont;

typedef struct
{
    v2 a;
    v2 b;
    v4 color;
} XRenderCommandLine;

typedef struct
{
    v2 position;
    v2 size;
    v4 color;
} XRenderCommandRect;

typedef struct
{
    v2 position;
    v2 size;
    v4 color;
    XSprite sprite;
} XRenderCommandSprite;

typedef struct
{
    v2 position;
    v4 color;
    XFont *font;
    u32 length;
    char *text;
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
    
    Rect2 *scissor;
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
    
    v4 clear_color;
    
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

void xrender2d_initialize (v4 clear_color);
void xrender2d_shutdown   (void);
void xrender2d_pre_update (void);
void xrender2d_post_update(XRenderBatch *array, u32 count);

void draw_line   (XRenderBatch *batch, v2 a,   v2 b,    v4 color);
void draw_arrow  (XRenderBatch *batch, v2 a,   v2 b,    v4 color, f32 head_size);
void draw_rect   (XRenderBatch *batch, v2 pos, v2 size, v4 color);

void draw_sprite (XRenderBatch *batch, v2 pos, v2 size, v4 color, XSprite sprite);

void draw_circle (XRenderBatch *batch, v2 pos, v4 color, f32 radius);

void draw_rect_rounded (XRenderBatch *batch, v2 pos, v2 size, v4 color, f32 radius);

void draw_text   (XRenderBatch *batch, v2 pos, v4 color, XFont *font, char *text);
void draw_grid   (XRenderBatch *batch, v2 gap, v2 line_thickness, v4 color);

void xrender2d_reset_batch(XRenderBatch *batch);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XFont *  xrender2d_font        (char *path, char *name, int heightpoints);
XSprite  xrender2d_font_glyph  (XFont *font, u32 c);
void     xrender2d_font_free   (XFont *font);
void     xrender2d_font_dim    (XFont *font, char *text, v2 dest);

void     xrender2d_break_text  (char *lines, u32 *lengths, u32 *lineCount, 
                                u32 maxLineCount, u32 maxLineCharCount, 
                                XFont *font, char *str, f32 maxWidth);




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

u8 *      xrender2d_load_png(char *path, v2i dim);
XSprite   xrender2d_sprite_from_png   (char *path);
XSprite   xrender2d_sprite_from_bytes (u8 *b, v2i dim);

/* =========================================================================
   END OF INTERFACE
   ========================================================================= */













/* =========================================================================
   IMPLEMENTATION INTERFACE
   ========================================================================= */

typedef struct
{
    v3 position;
    v4 color;
} LineVertex;

typedef struct
{
    v3 position;
    v3 uvs;
    v4 color;
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

static void initialize_font_dc(void);

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
"  pos[2] += 500;"

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
"  pos[2] += 500;"

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

void draw_line(XRenderBatch *batch, v2 a, v2 b, v4 color) {
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_line;
    v2_copy(a, command->line.a);
    v2_copy(b, command->line.b);
    v4_copy(color, command->line.color);
}

void draw_arrow(XRenderBatch *batch, v2 a, v2 b, v4 color, f32 headSize) {
    if (a[0] != b[0] || a[1] != b[1]) {
        draw_line(batch, a, b, color);
        
        v2 dir;
        v2_sub(b, a, dir);
        v2_normalize(dir);
        v2 normal = {-dir[1], dir[0]};
        
        v2 headSizeTimesDir;
        v2_mul(headSize, dir, headSizeTimesDir);
        
        v2 back;
        v2_sub(b, headSizeTimesDir, back);
        
        v2 posHeadSizeTimesNormal, negHeadSizeTimesNormal;
        v2 headA, headB;
        
        v2_mul(+headSize, normal, posHeadSizeTimesNormal);
        v2_mul(-headSize, normal, negHeadSizeTimesNormal);
        
        v2_add(back, posHeadSizeTimesNormal, headA);
        v2_add(back, negHeadSizeTimesNormal, headB);
        
        draw_line(batch, b, headA, color);
        draw_line(batch, b, headB, color);
    }
}

void draw_rect(XRenderBatch *batch, v2 pos, v2 size, v4 color) {
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_rect;
    v2_copy(pos, command->sprite.position);
    v2_copy(size, command->sprite.size);
    v4_copy(color, command->sprite.color);
}

void draw_sprite(XRenderBatch *batch, v2 pos, v2 size, v4 color, XSprite sprite) {
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_sprite;
    v2_copy(pos, command->sprite.position);
    v2_copy(size, command->sprite.size);
    v4_copy(color, command->sprite.color);
    command->sprite.sprite = sprite;
}

void
draw_circle(XRenderBatch *batch, v2 pos, v4 color, f32 radius) {
    v2 posMinusRadius;
    v2_sub(pos, (v2){radius,radius}, posMinusRadius);
    draw_sprite(batch, posMinusRadius, (v2){2*radius,2*radius}, 
                color, sprite_circle);
}

void
draw_rect_rounded(XRenderBatch *batch, v2 pos, v2 size, v4 color, f32 radius) {
    v2 dim;
    v2_copy((v2){radius,radius}, dim);
    draw_sprite(batch, pos, dim, color, sprite_circle_tl);
    
    v2 posPlusSizeX;
    v2_add(pos, (v2){size[0]-radius,0}, posPlusSizeX);
    
    draw_sprite(batch, posPlusSizeX, dim, color, sprite_circle_tr);
    
    v2 posPlusSizeY;
    v2_add(pos, (v2){0,size[1]-radius}, posPlusSizeY);
    draw_sprite(batch, posPlusSizeY, dim, color, sprite_circle_bl);
    
    v2 posPlusSizeXY;
    v2_add(pos, (v2){size[0]-radius,size[1]-radius}, posPlusSizeXY);
    draw_sprite(batch, posPlusSizeXY, dim, color, sprite_circle_br);
    
    f32 posXPlusRadius = (pos[0] + radius);
    f32 posYPlusRadius = (pos[1] + radius);
    
    /* Center */
    draw_rect(batch,
              (v2){posXPlusRadius, pos[1]},
              (v2){(size[0]-2*radius),size[1]},
              color);
    
    /* Left strip */
    draw_rect(batch,
              (v2){pos[0], posYPlusRadius},
              (v2){(radius),(size[1]-2*radius)},
              color);
    
    v2 lastP;
    v2_add(pos, (v2){size[0]-radius,radius}, lastP);
    /* Right strip */
    draw_rect(batch, lastP, (v2){radius,(size[1]-2*radius)}, color);
}


void draw_text(XRenderBatch *batch, v2 pos, v4 color, XFont *font, char *text) {
    XRenderCommand *command = xrender2d_push_command(batch);
    command->type = XRenderCommandType_text;
    v2_copy(pos, command->text.position);
    v4_copy(color, command->text.color);
    command->text.font = font;
    command->text.length = xstrlen(text);
    command->text.text = xstrnew(text);
}

void draw_grid(XRenderBatch *batch, v2 gap, v2 line_thickness, v4 color)
{
    v2i grid_horizontal_line_count = 
    {
        (s32)ceilf(xd11.bbDim[0] / gap[0]),
        (s32)ceilf(xd11.bbDim[1] / gap[1]),
    };
    
    /* Draw vertical lines */
    for (s32 x=0; x<grid_horizontal_line_count[0]; ++x)
        draw_rect(batch, 
                  (v2){x*gap[0], 0}, 
                  (v2){line_thickness[0], xd11.bbDim[1]}, 
                  color);
    
    /* Draw horizontal lines */
    for (s32 y=0; y<grid_horizontal_line_count[1]; ++y)
        draw_rect(batch, 
                  (v2){0, y*gap[1]}, 
                  (v2){xd11.bbDim[0], line_thickness[1]}, 
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

void xrender2d_initialize(v4 clear_color)
{
    initialize_font_dc();
    
    v4_copy(clear_color, xrender2d.clear_color);
    
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
                                (s32)xd11.bbDim[0], // Width
                                (s32)xd11.bbDim[1], // Height
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
        u32 textureArraySide = 2048;
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
        
        xrender2d.textures.tex.dim[0] = textureArraySide;
        xrender2d.textures.tex.dim[1] = textureArraySide;
        xrender2d.textures.dim[0] = textureArraySide;
        xrender2d.textures.dim[1] = textureArraySide;
        
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
                        sizeof(mat4),
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
               &(D3D11_VIEWPORT){0,0,xd11.bbDim[0],xd11.bbDim[1], 0, 1});
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
                        sizeof(mat4),
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
               &(D3D11_VIEWPORT){0,0,xd11.bbDim[0],xd11.bbDim[1], 0, 1});
}

void xrender2d_create_resources(void)
{
    /* Create resources */
    
    /* white.png */
    {
        char full_path[512];
        xwin_path_abs(full_path, 512, "images/white.png");
        sprite_white = xrender2d_sprite_from_png(full_path);
        /* Since we stretch a lot this by using raw rect, we need to 
 cheat a little and give some padding to the uv coords to avoid
 artifactcs in the borders of rects */ 
        sprite_white.uvs.min[0] += 0.3f * (sprite_white.uvs.max[0]-sprite_white.uvs.min[0]);
        sprite_white.uvs.min[1] += 0.3f * (sprite_white.uvs.max[1]-sprite_white.uvs.min[1]);
        sprite_white.uvs.max[0] -= 0.3f * (sprite_white.uvs.max[0]-sprite_white.uvs.min[0]);
        sprite_white.uvs.max[1] -= 0.3f * (sprite_white.uvs.max[1]-sprite_white.uvs.min[1]);
    }
    
    /* circle.png */
    {
        char full_path[512];
        xwin_path_abs(full_path, 512, "images/circle.png");
        sprite_circle = xrender2d_sprite_from_png(full_path);
    }
    
    /* arrow.png */
    {
        char full_path[512];
        xwin_path_abs(full_path, 512, "images/arrow.png");
        sprite_arrow = xrender2d_sprite_from_png(full_path);
    }
    
    /* Make circle quadrants */
    s32 circleWidth = sprite_circle.dim[0];
    s32 circleHeight = sprite_circle.dim[1];
    f32 circleQuadrantWidth = circleWidth/2.0f;
    f32 circleQuadrantHeight = circleHeight/2.0f;
    
    f32 uu = (sprite_circle.uvs.max[0]-sprite_circle.uvs.min[0])/2;
    f32 vv = (sprite_circle.uvs.max[1]-sprite_circle.uvs.min[1])/2;
    
    /* Top left */
    f32 u = sprite_circle.uvs.min[0];
    f32 v = sprite_circle.uvs.min[1];
    f32 w = sprite_circle.uvs.min[2];
    v3_copy((v3){u,v,w},         sprite_circle_tl.uvs.min);
    v3_copy((v3){u+uu, v+vv, w}, sprite_circle_tl.uvs.max);
    v3_copy((v3){u,v+vv,w},      sprite_circle_bl.uvs.min);
    v3_copy((v3){u+uu,v, w},  sprite_circle_bl.uvs.max);
    v3_copy((v3){u+uu, v+vv, w}, sprite_circle_br.uvs.min);
    v3_copy((v3){u,v,w},         sprite_circle_br.uvs.max);
    v3_copy((v3){u+uu, v, w},    sprite_circle_tr.uvs.min);
    v3_copy((v3){u, v+vv, w},    sprite_circle_tr.uvs.max);
    
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

void xrender2d_push_rect_vertices(Array_T *array, v2 pos, v2 size, Rect3 uv, v4 color)
{
    f32 minx = (f32)((s32)pos[0]);
    f32 miny = (f32)((s32)pos[1]);
    f32 maxx = (f32)((s32)pos[0] + (s32)size[0]);
    f32 maxy = (f32)((s32)pos[1] + (s32)size[1]);
    
    /* Upper triangle */
    TexturedVertex a;
    v3_copy((v3){minx, miny, 0}, a.position);
    v3_copy((v3){uv.min[0], uv.min[1], uv.min[2]}, a.uvs);
    v4_copy(color, a.color);
    Array_push(array, &a);
    
    TexturedVertex b;
    v3_copy((v3){maxx, miny, 0}, b.position);
    v3_copy((v3){uv.max[0], uv.min[1], uv.min[2]}, b.uvs);
    v4_copy(color, b.color);
    Array_push(array, &b);
    
    TexturedVertex c;
    v3_copy((v3){maxx, maxy, 0}, c.position);
    v3_copy((v3){uv.max[0], uv.max[1], uv.min[2]}, c.uvs);
    v4_copy(color, c.color);
    Array_push(array, &c);
    
    /* Lower triangle */
    TexturedVertex d;
    v3_copy((v3){minx, miny, 0}, d.position);
    v3_copy((v3){uv.min[0], uv.min[1], uv.min[2]}, d.uvs);
    v4_copy(color, d.color);
    Array_push(array, &d);
    
    TexturedVertex e;
    v3_copy((v3){maxx, maxy, 0}, e.position);
    v3_copy((v3){uv.max[0], uv.max[1], uv.min[2]}, e.uvs);
    v4_copy(color, e.color);
    Array_push(array, &e);
    
    TexturedVertex f;
    v3_copy((v3){minx, maxy, 0}, f.position);
    v3_copy((v3){uv.min[0], uv.max[1], uv.min[2]}, f.uvs);
    v4_copy(color, f.color);
    Array_push(array, &f);
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
                LineVertex a, b;
                v3_copy((v3){c->line.a[0], c->line.a[1], 0}, a.position);
                v4_copy(c->line.color, a.color);
                v3_copy((v3){c->line.b[0], c->line.b[1], 0}, b.position);
                v4_copy(c->line.color, b.color);
                
                Array_push(lineVertices, &a);
                Array_push(lineVertices, &b);
            } break;
            
            case XRenderCommandType_rect:
            {
                v2 pos, size;
                v2_copy(c->rect.position, pos);
                v2_copy(c->rect.size, size);
                v4 color;
                v4_copy(c->rect.color, color);
                Rect3 uvs = sprite_white.uvs;
                xrender2d_push_rect_vertices(texVertices, pos, size, uvs, color);
            } break;
            
            case XRenderCommandType_sprite:
            {
                v2 pos, size;
                v2_copy(c->sprite.position, pos);
                v2_copy(c->sprite.size, size);
                Rect3 uvs = c->sprite.sprite.uvs;
                v4 color;
                v4_copy(c->sprite.color, color);
                xrender2d_push_rect_vertices(texVertices, pos, size, uvs, color);
            } break;
            
            case XRenderCommandType_text:
            {
                u32 lastCodePoint = 0;
                char *at = c->text.text;
                v4 color;
                v4_copy(c->text.color, color);
                v2 pos;
                v2_copy(c->text.position, pos);
                while (*at != 0) {
                    XFontGlyph *glyph = c->text.font->glyphs + c->text.font->glyphIndexFromCodePoint[*at];
                    if (glyph) {
                        XSprite *sprite = &glyph->sprite;
                        v2i dim;
                        v2i_copy(sprite->dim, dim);
                        Rect3 uvs = sprite->uvs;
                        u32 codePoint = (u32)*at;
                        
                        /* Advance based on kerning */
                        if (lastCodePoint) {
                            pos[0] += c->text.font->horizontalAdvance[lastCodePoint*c->text.font->maxGlyphCount + codePoint];
                        }
                        
                        /* glyph alignments */
                        v2 glyph_pos;
                        v2_copy(pos, glyph_pos);
                        glyph_pos[0] -= sprite->align[0]*dim[0];
                        
                        glyph_pos[1] -= dim[1] - sprite->align[1]*dim[1];
                        glyph_pos[1] += c->text.font->metrics.tmAscent;
                        
                        xrender2d_push_rect_vertices(texVertices, glyph_pos, (v2){(f32)dim[0],(f32)dim[1]}, uvs, color);
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
    mat4 matrixProjection = {
        2.0f / xd11.bbDim[0], 0, 0, -1,
        0, -2.0f / xd11.bbDim[1], 0, 1.f,
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
              &(D3D11_VIEWPORT){0,0,xd11.bbDim[0],xd11.bbDim[1], 0, 1});
    
    
    LineVertex data[2] =
    {
        {{0,0,0}, {1,1,1,1}},
        {{xd11.bbDim[0],xd11.bbDim[1],0}, {1,1,1,1}},
    };
}

void xrender2d_update_sprite_pass(void)
{
    mat4 matrixProjection = {
        2.0f / xd11.bbDim[0], 0, 0, -1,
        0, -2.0f / xd11.bbDim[1], 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xd11_update_subres(xrender2d.pass_sprites.vShaderCBuffers[0], &matrixProjection);
    
    /* Update viewport and scissor rect*/
    Array_set(xrender2d.pass_sprites.viewports, 0,
              &(D3D11_VIEWPORT){0,0,xd11.bbDim[0],xd11.bbDim[1], 0, 1});
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
                                 (UINT)xd11.bbDim[0],
                                 (UINT)xd11.bbDim[1], 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get default target backbuffer texture from swap chain */
    xrender2d.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create default target depth stencil texture */
    xrender2d.target_default.depth_stencil.texture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
                                                                    {
                                                                        (s32)xd11.bbDim[0],
                                                                        (s32)xd11.bbDim[1],
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

u8 *xrender2d_load_png(char *path, v2i dim) {
    u8 *r, *rowDst, *rowSrc;
    int w,h, i,j, nrChannels, dataSize;
    u32 *pxDst, *pxSrc, cr,cg,cb,ca;
    
    r=0;
    unsigned char *data = stbi_load(path, &w, &h, &nrChannels, 0);
    if (data)
    {
        v2i_copy((v2i){w,h}, dim);
        
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
        v2i_copy((v2i){0,0}, dim);
    
    return r;
}

XSprite xrender2d_sprite_from_bytes(u8 *b, v2i dim) {
    XSprite r = {0};
    v2i_copy(dim, r.dim);
    r.uvs = xd11_texarray_put(&xrender2d.textures, b, dim);
    return r;
}

XSprite xrender2d_sprite_from_png(char *path) {
    XSprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = xrender2d_load_png(path, dim);
    if (b)
    {
        r = xrender2d_sprite_from_bytes(b, dim);
        xfree(b);
    }
    return r;
}

XFont *
xrender2d_font(char *path, char *name, s32 pixelHeight)
{
    XFont *result = xalloc(sizeof *result);
    
    /* Init the atlas */
    v2i atlasDim;
    v2i_copy(xrender2d.textures.dim, atlasDim);
    s32 uvZ = xrender2d.textures.at;
    xtexture_atlas_init(&result->atlas, atlasDim, uvZ);
    /* Allocate space for the cpu bytes */
    result->atlasBytes = xalloc(atlasDim[0]*atlasDim[1]*4);
    
    AddFontResourceExA(path, FR_PRIVATE, 0);
    result->handle = CreateFontA(pixelHeight, 0, 0, 0,
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
    
    xstrcpy(result->path, path);
    
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

static void
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

inline void
SRGB255ToLinear1(v4 C, v4 dest) {
    f32 Inv255 = 1.0f / 255.0f;
    
    dest[0] = f32_square(Inv255*C[0]);
    dest[1] = f32_square(Inv255*C[1]);
    dest[2] = f32_square(Inv255*C[2]);
    dest[3] = Inv255*C[3];
}

inline void
Linear1ToSRGB255(v4 C, v4 dest) {
    f32 One255 = 255.0f;
    
    dest[0] = One255*sqrtf(C[0]);
    dest[1] = One255*sqrtf(C[1]);
    dest[2] = One255*sqrtf(C[2]);
    dest[3] = One255*C[3];
}

void blit_unchecked(u8 *dst, v2i dest_size, u8 *src, v2i at, v2i dim) {
    u8 *rowSrc, *rowDst;
    u32 *pxSrc, *pxDst;
    
    u32 dstStride = dest_size[0]*4;
    
    rowSrc=src;
    rowDst=dst + at[1]*dstStride + at[0]*4;
    
    for (at[1]=0; at[1]<dim[1]; ++at[1])
    {
        pxSrc=(u32 *)rowSrc;
        pxDst=(u32 *)rowDst;
        
        for (at[0]=0; at[0]<dim[0]; ++at[0])
            *pxDst++ = *pxSrc++;
        
        rowSrc += 4*dim[0];
        rowDst += dstStride;
    }
}


XSprite xrender2d_font_glyph(XFont *font, u32 codePoint) {
    v2 alignPercentage;
    XSprite r = {0};
    
    if (font->minCodePoint > codePoint)
        font->minCodePoint = codePoint;
    
    if (font->maxCodePoint < codePoint)
        font->maxCodePoint = codePoint;
    
    u32 glyphIndex = font->glyphIndexFromCodePoint[codePoint];
    memset(globalFontBits, 0x00, MAX_FONT_WIDTH*MAX_FONT_HEIGHT*sizeof(u32));
    
    char cheesePoint = (char)codePoint;
    
    SIZE size;
    GetTextExtentPoint32A(globalFontDeviceContext, &cheesePoint, 1, &size);
    
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
    TextOutA(globalFontDeviceContext, preStepX, 0, &cheesePoint, 1);
    
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
        
        v2i_copy((v2i){resultWidth, resultHeight}, r.dim);
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
                v4 texel = {255.0f, 255.0f, 255.0f, gray};
                //texel = SRGB255ToLinear1(texel);
                //texel.rgb *= texel.a;
                //texel = Linear1ToSRGB255(texel);
                *dest++ = (((u32)(texel[3] + 0.5f) << 24) |
                           ((u32)(texel[0] + 0.5f) << 16) |
                           ((u32)(texel[1] + 0.5f) << 8) |
                           ((u32)(texel[2] + 0.5f) << 0));
                ++source;
            }
            
            destRow -= pitch;
            sourceRow -= MAX_FONT_WIDTH;
        }
        
        alignPercentage[0] = (1.0f) / (f32)resultWidth;
        alignPercentage[1] = (1.0f + (maxY - (boundHeight - font->metrics.tmDescent))) /
        (f32)resultHeight;
        
        kerningChange = (f32)(minX - preStepX);
        
        XTextureAtlasCoords coords = xtexture_atlas_put(&font->atlas, r.dim);
        r.uvs = coords.uvs;
        
        blit_unchecked(font->atlasBytes, font->atlas.dim,
                       memory, coords.at, r.dim);
        
        /* Continue the madness by flipping the uvs */
        f32 tempMinY = r.uvs.min[1];
        r.uvs.min[1] = r.uvs.max[1];
        r.uvs.max[1] = tempMinY;
        
        v2_copy(alignPercentage, r.align);
        
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
    RemoveFontResourceA(f->path);
    
    xfree(f);
}

void
xrender2d_font_dim(XFont *font, char *text, v2 dest) {
    u32 lastCodePoint = 0;
    char *at = text;
    v2 atP;
    v2_copy((v2){0,font->lineHeight}, atP);
    while (*at != 0)
    {
        u32 codePoint = (u32)*at;
        if (lastCodePoint) 
            atP[0] += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + codePoint];
        lastCodePoint = codePoint;
        ++at;
    }
    
    atP[0] += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + 0];
    
    dest[0] = atP[0];
    dest[1] = atP[1];
}


/* NOTE: It is assumed that lines buffer will hold maxLineCount strings of maxLineCharCount length.
;        Is is assumed that maxLineCharCount is big enough to fit a line as big as maxWidth.
;        This static will not break lines based on maxLineCharCount!
;
;        Each line will break at the last space before the next word would make the width of the line
;        be past maxWidth. Lines will hold an array with the resulting lines and lineCount will hold
;        the number of lines the string got broken into. */

void xrender2d_break_text(char *lines, u32 *lengths, u32 *lineCount,
                          u32 maxLineCount, u32 maxLineCharCount,
                          XFont *font, char *str, f32 maxWidth) {
    char *lastSpace = 0;
    u32 lastCodePoint = 0;
    char *at = str;
    u32 lineCharIndex = 0;
    u32 lineLastSpace = 0;
    
    *lineCount = 0;
    memset(lines, 0, maxLineCount*maxLineCharCount*sizeof(char));
    
    /* Break text into lines */
    v2 p = {0,0};
    char *lineStart = at;
    while (*at != 0) {
        u32 codePoint = (u32)*at;
        
        /* If there was a glyph before this, advance the position based on kerning */
        if (lastCodePoint)
            p[0] += font->horizontalAdvance[lastCodePoint*font->maxGlyphCount + codePoint];
        
        /* If the position is greated than the maximum width allowed */
        if (p[0] > maxWidth) {
            /* Reset the x position and advance the y position */
            p[0] = 0;
            
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