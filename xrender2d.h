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
    v2f board_pos = sub2f(mul2f(.5f, xd11.back_buffer_size), mul2f(.5f,board_size));
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

typedef struct
{
    XD11Texture texture;
    u8 *bytes;
    s32 bottom;
    v2i at;
} XTextureAtlas;

typedef struct
{
    v2f size;
    rect2f uv;
    v2f align;
} XSprite;

typedef struct XGlyphHashNode
{
    wchar_t key;
    XSprite *value;
    struct XGlyphHashNode *next;
    
} XGlyphHashNode;

#define XGLYPH_HASH_LENGTH 512

typedef struct
{
    XGlyphHashNode *storage[XGLYPH_HASH_LENGTH];
    
} XGlyphHashTable;

u32 xglyph_hash(wchar_t unicode)
{
    // TODO: Better hash function !!
    u32 result = unicode & (XGLYPH_HASH_LENGTH-1);
    return result;
}

XSprite *
xglyph_get(XGlyphHashTable *table, wchar_t key)
{
    u32 hash = xglyph_hash(key);
    assert(hash < XGLYPH_HASH_LENGTH);
    
    /* Search for a node with the same key */
    XGlyphHashNode *found = 0, *at = table->storage[hash];
    
    while (at)
    {
        if (at->key == key)
        {
            found = at;
            break;
        }
        
        at = at->next;
    }
    
    if (found)
        return found->value;
    else
        return 0;
}

void
xglyph_set(XGlyphHashTable *table, wchar_t key, XSprite *value)
{
    u32 hash = xglyph_hash(key);
    assert(hash < XGLYPH_HASH_LENGTH);
    
    /* Make a new node */
    XGlyphHashNode *new_node = xalloc(sizeof *new_node);
    new_node->key = key;
    new_node->value = value;
    
    /* Search for a node with the same key */
    XGlyphHashNode *found = 0, *at = table->storage[hash];
    
    while (at)
    {
        if (at->key == key)
        {
            found = at;
            break;
        }
        
        at = at->next;
    }
    
    new_node->next = table->storage[hash];
    table->storage[hash] = new_node;
}

typedef struct
{
    f32 lineadvance, charwidth, maxdescent;
    wchar_t path[MAX_PATH];
    HFONT handle;
    HBITMAP bitmap;
    TEXTMETRICW metrics;
    VOID *bytes;
    XGlyphHashTable glyphs;
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
} XRenderBatch;

typedef struct
{
    xd11_tx2d texture;
    XD11DepthStencil depth_stencil;
} XRenderTarget;

typedef struct
{
    /* Core variables */
    XRenderTarget target_default;
    
    v4f clear_color;
    
    u32 target_view_count;
    xd11_tgv *target_views;
    
    XTextureAtlas texture_atlas;
    
    /* Passes */
    XD11RenderPass pass_lines;
    XD11RenderPass pass_sprites;
    
    /* Resources */
    XSprite sprite_white;
    XSprite sprite_arrow;
} XRender2D;

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

global XRender2D xrender2d;

void xrender2d_initialize (v4f clear_color);
void xrender2d_shutdown   (void);
void xrender2d_pre_update (void);
void xrender2d_post_update(XRenderBatch *array, u32 count);

void draw_line  (XRenderBatch *batch, v2f a,   v2f b,    v4f color);
void draw_arrow (XRenderBatch *batch, v2f a,   v2f b,    v4f color, f32 head_size);
void draw_rect  (XRenderBatch *batch, v2f pos, v2f size, v4f color);
void draw_sprite(XRenderBatch *batch, v2f pos, v2f size, v4f color, XSprite sprite);
void draw_text  (XRenderBatch *batch, v2f pos, v4f color, XFont *font, wchar_t *text);
void draw_grid  (XRenderBatch *batch, v2f gap, v2f line_thickness, v4f color);

void xrender2d_reset_batch(XRenderBatch *batch);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

XFont    xrender2d_font (XTextureAtlas *a, wchar_t *path, wchar_t *name, int heightpoints);
XSprite  xrender2d_font_glyph (XTextureAtlas *a, XFont font, wchar_t c, rect2f *tBounds, s32 *tDescent);
void     xrender2d_font_free   (XFont font);
s32      xrender2d_font_height (s32 pointHeight);

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


u8 *     xrender2d_load_png(wchar_t *path, v2i *dim, bool premulAlpha);

XSprite   xrender2d_sprite_from_png  (XTextureAtlas *atlas, wchar_t *path, bool premulalpha);
XSprite   xrender2d_sprite_from_bytes(XTextureAtlas *atlas, u8 *b, v2i dim);

s32      xrender2d_font_height(s32 pointHeight);
void     xrender2d_font_free(XFont f);

void     xrender2d_blit_simple_unchecked(u8 *dest, v2i dest_size, u8 *src, v2i at, v2i dim);

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
    v2f uvs;
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
"  float2 uv             : TEX;"
"  float4 color          : COL;"
"};"

"struct vs_out"
"{"
"  float4 position_clip : SV_POSITION;"
"  float2 uv            : TEXCOORD;"
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
"  float2 uv            : TEXCOORD;"
"  float4 color         : COLOR;"
"};"

"Texture2D tex;"
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
        (s32)ceilf(xd11.back_buffer_size.x / gap.x),
        (s32)ceilf(xd11.back_buffer_size.y / gap.y),
    };
    
    /* Draw vertical lines */
    for (s32 x=0; x<grid_horizontal_line_count.x; ++x)
        draw_rect(batch, 
                  (v2f){x*gap.x, 0}, 
                  (v2f){line_thickness.x, xd11.back_buffer_size.y}, 
                  color);
    
    /* Draw horizontal lines */
    for (s32 y=0; y<grid_horizontal_line_count.y; ++y)
        draw_rect(batch, 
                  (v2f){0, y*gap.y}, 
                  (v2f){xd11.back_buffer_size.x, line_thickness.y}, 
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
        xd11_buffer_update(xrender2d.pass_lines.vertex_buffers.array[0], 
                           lineVertexArray, lineVertexCount*sizeof(LineVertex));
        
        xd11_buffer_update(xrender2d.pass_sprites.vertex_buffers.array[0], 
                           texVertexArray, texVertexCount*sizeof(TexturedVertex));
        
        /* Execute render passes */
        xd11_render_pass(&xrender2d.pass_lines, lineVertexCount);
        xd11_render_pass(&xrender2d.pass_sprites, texVertexCount);
        
        /* Free C arrays and Array_Ts */
        xfree(lineVertexArray);
        Array_free(&lineVertices);
        
        xfree(texVertexArray);
        Array_free(&texVertices);
    }
    
    
    xd11_update();
    xwin_update(true, xd11.window_size);
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
    xrender2d.clear_color = clear_color;
    
    /* Get back buffer texture from swap chain */    
    xrender2d.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    xrender2d.target_view_count = 1;
    xrender2d.target_views = xalloc(1*sizeof(xd11_tgv));
    xrender2d.target_views[0] = xd11_target_view(xrender2d.target_default.texture);
    
    /* Depth stencil */
    xrender2d_depth_stencil(&xrender2d.target_default.depth_stencil,
                            (D3D11_TEXTURE2D_DESC)
                            {
                                (s32)xd11.back_buffer_size.x, // Width
                                (s32)xd11.back_buffer_size.y, // Height
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
    xrender2d.pass_lines.depth_stencil = xrender2d.target_default.depth_stencil;
    xrender2d.pass_sprites.depth_stencil = xrender2d.target_default.depth_stencil;
    
    /* Texture atlas texture */
    {
        u32 atlas_size = 2048;
        u32 bytes_size = atlas_size*atlas_size*4;
        
        /* Create D3D11 texture */
        D3D11_TEXTURE2D_DESC desc =
        {
            atlas_size, // Width
            atlas_size, // Height
            1, // Mip levels 
            1, // Array size
            DXGI_FORMAT_R8G8B8A8_UNORM, // Format
            {1,0}, // Sample
            D3D11_USAGE_DYNAMIC, // Usage 
            D3D11_BIND_SHADER_RESOURCE, // Bind flags
            D3D11_CPU_ACCESS_WRITE, // Cpu access flags
            0 // Misc flags
        };
        xrender2d.texture_atlas.texture.handle = xd11_texture2d(desc, 0);
        
        xrender2d.texture_atlas.texture.size   = fil2i(atlas_size);
        
        /* Create CPU memory bitmap */
        s32 bitmap_size = xrender2d.texture_atlas.texture.size.x * 
            xrender2d.texture_atlas.texture.size.y * 4;
        
        
        xrender2d.texture_atlas.bytes  = (u8*)xalloc(bitmap_size);
    }
    
    /* Create passes */
    xrender2d_create_line_pass();
    xrender2d_create_sprite_pass();
    
    /* Create resources */
    xrender2d_create_resources();
}

void xrender2d_shutdown(void)
{
    xrender2d_free_passes();
    xfree(xrender2d.texture_atlas.bytes);
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
    xrender2d.pass_lines.vertex_shader = xd11_compile_vertex_shader(linesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    xrender2d.pass_lines.pixel_shader = xd11_compile_pixel_shader(linesPS, &compiledPS);
    
    /* Input layout */
    xrender2d.pass_lines.input_layout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    xrender2d.pass_lines.target_view = xrender2d.target_views[0];
    
    /* Blend state */
    xrender2d.pass_lines.blend_state = xd11_blend_state((D3D11_BLEND_DESC)
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
    xrender2d.pass_lines.rasterizer_state = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    xrender2d.pass_lines.sampler_state = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    xrender2d.pass_lines.vertex_buffers.count = 1;
    xrender2d.pass_lines.vertex_buffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    xrender2d.pass_lines.vertex_buffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    xrender2d.pass_lines.vertex_buffers.strides = xalloc(1*sizeof(UINT));
    xrender2d.pass_lines.vertex_buffers.strides[0] = sizeof(LineVertex);
    
    /* Vertex buffer offsets */
    xrender2d.pass_lines.vertex_buffers.offsets = xalloc(1*sizeof(UINT));
    xrender2d.pass_lines.vertex_buffers.offsets[0] = 0;
    
    /* Vertex shader constant buffers */
    xrender2d.pass_lines.vs_cbuffer_count = 1;
    xrender2d.pass_lines.vs_cbuffers = xalloc(1*sizeof(xd11_buf));
    
    xrender2d.pass_lines.vs_cbuffers[0] = xd11_buffer((D3D11_BUFFER_DESC)
                                                      {
                                                          sizeof(mat4f),
                                                          D3D11_USAGE_DEFAULT, 
                                                          D3D11_BIND_CONSTANT_BUFFER,
                                                          0,
                                                          0,
                                                          0,
                                                      },
                                                      0);
    
    /* Pixel shader resources */
    xrender2d.pass_lines.ps_resource_count = 0;
    xrender2d.pass_lines.ps_resources = 0;
    
    /* Topology */
    xrender2d.pass_lines.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    
    /* Viewports */
    xrender2d.pass_lines.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&xrender2d.pass_lines.viewports, 
               &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    /* Scissors */
    xrender2d.pass_lines.scissors = Array_new(1, sizeof(D3D11_RECT));
    Array_push(&xrender2d.pass_lines.scissors,
               &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
}

void xrender2d_create_sprite_pass(void)
{
    /* Input format */
    D3D11_INPUT_ELEMENT_DESC inputFormat[] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    
    /* Vertex shader */
    ID3DBlob *compiledVS;
    xrender2d.pass_sprites.vertex_shader = xd11_compile_vertex_shader(spritesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    xrender2d.pass_sprites.pixel_shader = xd11_compile_pixel_shader(spritesPS, &compiledPS);
    
    /* Input layout */
    xrender2d.pass_sprites.input_layout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    xrender2d.pass_sprites.target_view = xrender2d.target_views[0];
    
    /* Blend state */
    xrender2d.pass_sprites.blend_state = xd11_blend_state((D3D11_BLEND_DESC)
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
    xrender2d.pass_sprites.rasterizer_state = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    xrender2d.pass_sprites.sampler_state = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    xrender2d.pass_sprites.vertex_buffers.count = 1;
    xrender2d.pass_sprites.vertex_buffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    xrender2d.pass_sprites.vertex_buffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    xrender2d.pass_sprites.vertex_buffers.strides = xalloc(1*sizeof(UINT));
    xrender2d.pass_sprites.vertex_buffers.strides[0] = sizeof(TexturedVertex);
    
    /* Vertex buffers offsets */
    xrender2d.pass_sprites.vertex_buffers.offsets = xalloc(1*sizeof(UINT));
    xrender2d.pass_sprites.vertex_buffers.offsets[0] = 0;
    
    /* Vertex Shader Constant buffers */
    xrender2d.pass_sprites.vs_cbuffer_count = 1;
    xrender2d.pass_sprites.vs_cbuffers = xalloc(1*sizeof(xd11_buf));
    xrender2d.pass_sprites.vs_cbuffers[0] = xd11_buffer((XD11_BUFD)
                                                        {
                                                            sizeof(mat4f),
                                                            D3D11_USAGE_DEFAULT, 
                                                            D3D11_BIND_CONSTANT_BUFFER,
                                                            0,
                                                            0,
                                                            0,
                                                        },
                                                        0);
    
    /* Pixel Shader Resources */
    xrender2d.pass_sprites.ps_resource_count = 1;
    xrender2d.pass_sprites.ps_resources = xalloc(1*sizeof(xd11_srv));
    xrender2d.pass_sprites.ps_resources[0] = xd11_shader_res_view(xrender2d.texture_atlas.texture.handle,
                                                                  (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                                                  {
                                                                      DXGI_FORMAT_R8G8B8A8_UNORM,
                                                                      D3D_SRV_DIMENSION_TEXTURE2D,
                                                                      .Texture2D = (D3D11_TEX2D_SRV){0,1}
                                                                  });
    
    /* Topology */
    xrender2d.pass_sprites.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    /* Viewports */
    xrender2d.pass_sprites.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&xrender2d.pass_sprites.viewports,
               &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    /* Scissors */
    xrender2d.pass_sprites.scissors = Array_new(1, sizeof(D3D11_RECT));
    Array_push(&xrender2d.pass_sprites.scissors,
               &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
}

void xrender2d_create_resources(void)
{
    /* Create resources */
    wchar_t full_path[512];
    xwin_path_abs(full_path, 512, L"images/white.png");
    v2i white_dim;
    u8 *white_bytes = xrender2d_load_png(full_path, &white_dim, false);
    if (!white_bytes)
        assert(!"Need to have a white.png in images directory!");
    xrender2d.sprite_white = xrender2d_sprite_from_bytes(&xrender2d.texture_atlas, white_bytes, white_dim);
    xfree(white_bytes);
    
    xrender2d.sprite_arrow = xrender2d_sprite_from_png(&xrender2d.texture_atlas, L"images/arrow128.png", false);
    
    xd11_texture2d_update(xrender2d.texture_atlas.texture, xrender2d.texture_atlas.bytes);
}

void xrender2d_free_passes(void)
{
    // Lines
    xfree(xrender2d.pass_lines.vertex_buffers.array);
    xfree(xrender2d.pass_lines.vertex_buffers.strides);
    xfree(xrender2d.pass_lines.vertex_buffers.offsets);
    xfree(xrender2d.pass_lines.vs_cbuffers);
    Array_free(&xrender2d.pass_lines.viewports);
    Array_free(&xrender2d.pass_lines.scissors);
    
    // XSprites
    xfree(xrender2d.pass_sprites.vertex_buffers.array);
    xfree(xrender2d.pass_sprites.vertex_buffers.strides);
    xfree(xrender2d.pass_sprites.vertex_buffers.offsets);
    xfree(xrender2d.pass_sprites.vs_cbuffers);
    xfree(xrender2d.pass_sprites.ps_resources);
    Array_free(&xrender2d.pass_sprites.viewports);
    Array_free(&xrender2d.pass_sprites.scissors);
    
    // Core
    xfree(xrender2d.target_views);
}

void xrender2d_push_rect_vertices(Array_T *array, v2f pos, v2f size, rect2f uv, v4f color)
{
    /* Upper triangle */
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x, pos.y, 0}, (v2f){uv.min.x, uv.max.y}, color});
    
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x + size.x, pos.y, 0}, (v2f){uv.max.x, uv.max.y}, color});
    
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x + size.x, pos.y + size.y, 0}, (v2f){uv.max.x, uv.min.y}, color});
    
    /* Lower triangle */
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x, pos.y, 0}, (v2f){uv.min.x, uv.max.y}, color});
    
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x + size.x, pos.y + size.y, 0}, (v2f){uv.max.x, uv.min.y}, color});
    
    Array_push(array, &(TexturedVertex)
               {(v3f){pos.x, pos.y + size.y, 0}, (v2f){uv.min.x, uv.min.y}, color});
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
                rect2f uv = xrender2d.sprite_white.uv;
                xrender2d_push_rect_vertices(texVertices, pos, size, uv, color);
            } break;
            
            case XRenderCommandType_sprite:
            {
                v2f pos = c->sprite.position;
                v2f size = c->sprite.size;
                rect2f uv = c->sprite.sprite.uv;
                v4f color = c->sprite.color;
                xrender2d_push_rect_vertices(texVertices, pos, size, uv, color);
            } break;
            
            case XRenderCommandType_text:
            {
                v2f pos   = c->text.position;
                v4f color = c->text.color;
                
                wchar_t *at = c->text.text;
                while (*at != 0)
                {
                    XSprite *sprite = xglyph_get(&c->text.font->glyphs, *at);
                    if (sprite)
                    {
                        v2f size = sprite->size;
                        rect2f uv = sprite->uv;
                        
                        v2f glyph_pos = sub2f(pos, sprite->align);
                        
                        xrender2d_push_rect_vertices(texVertices, glyph_pos, size, uv, color);
                        
                        pos.x += size.x + sprite->align.x;
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
        2.0f / xd11.back_buffer_size.x, 0, 0, -1,
        0, -2.0f / xd11.back_buffer_size.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    /* Update constant buffers */
    xd11_buf cbuffer = xrender2d.pass_lines.vs_cbuffers[0];
    xd11_update_subres(cbuffer, &matrixProjection);
    
    /* Set render target */
    xd11_set_render_target(xrender2d.pass_lines.target_view, xrender2d.pass_lines.depth_stencil.view);
    
    /* Clear */
    xd11_clear_rtv(xrender2d.pass_lines.target_view, xrender2d.clear_color);
    xd11_clear_dsv(xrender2d.pass_lines.depth_stencil.view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    /* Update viewport and scissor rect*/
    Array_set(xrender2d.pass_lines.viewports, 0, 
              &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    Array_set(xrender2d.pass_lines.scissors, 0,
              &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
    
    LineVertex data[2] =
    {
        {{0,0,0}, {1,1,1,1}},
        {{xd11.back_buffer_size.x,xd11.back_buffer_size.y,0}, {1,1,1,1}},
    };
}

void xrender2d_update_sprite_pass(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.back_buffer_size.x, 0, 0, -1,
        0, -2.0f / xd11.back_buffer_size.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xd11_update_subres(xrender2d.pass_sprites.vs_cbuffers[0], &matrixProjection);
    
    /* Update viewport and scissor rect*/
    Array_set(xrender2d.pass_sprites.viewports, 0,
              &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    Array_set(xrender2d.pass_sprites.scissors, 0,
              &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
}

void xd11_resized(void)
{
    /* Release target views */
    for (u32 i=0; i<xrender2d.target_view_count; ++i)
        ID3D11RenderTargetView_Release((xd11_tgv)xrender2d.target_views[i]);
    
    /* Default Target Depth Stencil Texture and View */
    ID3D11DepthStencilView_Release(xrender2d.target_default.depth_stencil.view);
    ID3D11Texture2D_Release       (xrender2d.target_default.depth_stencil.texture);
    
    /* Lines and XSprites passes pointers */
    xrender2d.pass_lines.depth_stencil.view = 0;
    xrender2d.pass_lines.depth_stencil.texture = 0;
    xrender2d.pass_sprites.depth_stencil.view = 0;
    xrender2d.pass_sprites.depth_stencil.texture = 0;
    
    /* Default Target Texture */
    ID3D11Texture2D_Release(xrender2d.target_default.texture);
    
    /* Resize the swapchain buffers */
    IDXGISwapChain_ResizeBuffers(xd11.swap_chain,
                                 2, 
                                 (UINT)xd11.back_buffer_size.x,
                                 (UINT)xd11.back_buffer_size.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get default target backbuffer texture from swap chain */
    xrender2d.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create default target depth stencil texture */
    xrender2d.target_default.depth_stencil.texture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
                                                                    {
                                                                        (s32)xd11.back_buffer_size.x,
                                                                        (s32)xd11.back_buffer_size.y,
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
    xrender2d.pass_lines.depth_stencil.view = xrender2d.target_default.depth_stencil.view;
    xrender2d.pass_lines.depth_stencil.texture = xrender2d.target_default.depth_stencil.texture;
    xrender2d.pass_sprites.depth_stencil.view = xrender2d.target_default.depth_stencil.view;
    xrender2d.pass_sprites.depth_stencil.texture = xrender2d.target_default.depth_stencil.texture;
    
    /* Create  render target view */
    xrender2d.target_views[0] = xd11_target_view(xrender2d.target_default.texture);
    
    /* Lines and XSprites passes Target view pointers */
    xrender2d.pass_sprites.target_view = xrender2d.target_views[0];
    xrender2d.pass_lines.target_view = xrender2d.target_views[0];
}

u8 *xrender2d_load_png(wchar_t *path, v2i *dim, bool premulalpha)
{
    u8 *r, *rowDst, *rowSrc;
    int w,h, i,j, nrChannels, dataSize;
    char *asciiPath;
    u32 *pxDst, *pxSrc, cr,cg,cb,ca;
    f32 realA;
    
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
                if (premulalpha)
                {
                    realA = (f32)ca / 255.0f;
                    cr=(s32)(cr*realA);
                    cg=(s32)(cg*realA);
                    cb=(s32)(cb*realA);
                }
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

void xrender2d_blit_simple_unchecked(u8 *dst, v2i dest_size,
                                     u8 *src, v2i at, v2i dim)
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

XSprite xrender2d_sprite_from_bytes(XTextureAtlas *a, u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    XSprite r;
    
    m = 1;
    
    if ((a->at.x + dim.x + m) > a->texture.size.x)
        a->at = (v2i){0, a->bottom};
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->texture.size.y);
    
    dst = a->bytes;
    src = b;
    xrender2d_blit_simple_unchecked(a->bytes, a->texture.size, src, a->at, dim);
    
    r.uv.min = (v2f){a->at.x / (f32)a->texture.size.x,
        ( (a->at.y + dim.y) ) / (f32)a->texture.size.y};
    
    r.uv.max = (v2f){(a->at.x + dim.x) / (f32)a->texture.size.x, 
        ( a->at.y ) / (f32)a->texture.size.y};
    
    r.size = ini2fs(dim.x,dim.y);
    
    a->at.x += dim.x+m;
    
    return r;
}

XSprite xrender2d_sprite_from_png(XTextureAtlas *a, wchar_t *path, bool premulalpha)
{
    XSprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = xrender2d_load_png(path, &dim, premulalpha);
    if (b)
    {
        r = xrender2d_sprite_from_bytes(a, b, dim);
        xfree(b);
    }
    return r;
}

XFont xrender2d_font(XTextureAtlas *atlas, wchar_t *path, wchar_t *name, int heightpoints)
{
    XFont result;
    memset(&result, 0, sizeof(result));
    
    v2i maxGlyphSize;
    s32 maxDescent;
    
    s32 i;
    
    wchar_t c;
    XSprite *v = 0;
    
    /* Add font resource to Windows */
    s32 temp = AddFontResourceW(path);
    assert(temp == 1);
    
    /* Create the font */
    result.handle = CreateFontW(xrender2d_font_height(heightpoints), 
                                0, 0, 0, FW_NORMAL, false, false, 
                                false, DEFAULT_CHARSET,
                                OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, 
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, name);
    
    assert(result.handle && result.handle != INVALID_HANDLE_VALUE);
    
    /* Create a bitmap to d11 glyphs into */
    BITMAPINFO info = xd11_bitmap_info(xd11.glyph_maker_size, -xd11.glyph_maker_size);
    result.bitmap = CreateDIBSection(xd11.glyph_maker_dc, &info, 
                                     DIB_RGB_COLORS, &result.bytes, 0, 0);
    assert(xd11.glyph_maker_size>0);
    memset(result.bytes, 0, xd11.glyph_maker_size*xd11.glyph_maker_size*4);
    
    /* Configure the bitmap drawing to use black and white */
    SelectObject(xd11.glyph_maker_dc, result.bitmap);
    SelectObject(xd11.glyph_maker_dc, result.handle);
    SetBkColor(xd11.glyph_maker_dc, RGB(0,0,0));
    SetTextColor(xd11.glyph_maker_dc, RGB(255,255,255));
    
    /* Get text metrics for font */
    TEXTMETRICW metrics = {0};
    GetTextMetricsW(xd11.glyph_maker_dc, &metrics);
    result.metrics = metrics;
    result.lineadvance = (f32)metrics.tmHeight - metrics.tmInternalLeading;
    /* Use this if nothing else, but search for better value later */
    result.charwidth = (f32)metrics.tmAveCharWidth;
    
    /* D11 some glyphs from the ASCII range */
    maxGlyphSize.x = maxGlyphSize.y = maxDescent = -10000;
    for (i=32; i<=126; ++i)
    {
        c=(wchar_t)i;
        
        rect2f tightBounds;
        s32 tightDescent;
        
        v = xalloc(sizeof *v);
        *v = xrender2d_font_glyph(atlas, result, c, &tightBounds, &tightDescent);
        
        /* Spaces wont have anything so tightBounds wont be found  */
        if (tightBounds.max.x!=0 && tightBounds.max.y!=0)
        {
            /* Calculate tight size */
            v2i tightSize =
            {
                (s32)(tightBounds.max.x - tightBounds.min.x),
                (s32)(tightBounds.max.y - tightBounds.min.y)
            };
            
            /* Book keep maximum glyph size and maximum descent */
            if (maxGlyphSize.x < tightSize.x) maxGlyphSize.x = tightSize.x;
            if (maxGlyphSize.y < tightSize.y) maxGlyphSize.y = tightSize.y;
            if (maxDescent < tightDescent) maxDescent = tightDescent;
        }
        
        /* Set the glyph in the hash table */
        xglyph_set(&result.glyphs, c, v);
    }
    
    result.lineadvance = (f32)maxGlyphSize.y;
    if (maxGlyphSize.x > 0)
        result.charwidth = (f32)maxGlyphSize.x;
    
    result.maxdescent = (f32)maxDescent;
    xstrcpy(result.path, MAX_PATH, path);
    
    return result;
}

XSprite xrender2d_font_glyph(XTextureAtlas *atlas, XFont font, wchar_t c, 
                             rect2f *tBounds, s32 *tDescent)
{
    u8 *bytes, *dstRow, *srcRow;
    s32 charsz, i,j, x,y;
    u32 *dstPx, *srcPx, *px, color, a, pre_step;
    v2f dim, align;
    XSprite r;
    SIZE size;
    rect2f bounds;
    
    wchar_t full_char[2] = {c, 0};
    
    pre_step = (s32)(0.3f*font.lineadvance);
    
    GetTextExtentPoint32W(xd11.glyph_maker_dc, full_char, 1, &size);
    align = (v2f){0,0};
    dim = ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(full_char);
    bounds = inir2f(0,0, dim.x, dim.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(xd11.glyph_maker_dc, pre_step,0, full_char, charsz);
    
    bool foundTBox = false;
    for (j=0; j<xd11.glyph_maker_size; ++j)
    {
        for (i=0; i<xd11.glyph_maker_size; ++i)
        {
            px = (u32 *)((u8 *)font.bytes + j*xd11.glyph_maker_size*4 + i*4);
            if (*px != 0)
            {
                foundTBox = true;
                if (tBounds->min.x>i) tBounds->min.x=(f32)i;
                if (tBounds->min.y>j) tBounds->min.y=(f32)j;
                if (tBounds->max.x<i) tBounds->max.x=(f32)i;
                if (tBounds->max.y<j) tBounds->max.y=(f32)j;
            }
        }
    }
    
    if (foundTBox) {
        --tBounds->min.x;
        --tBounds->min.y;
        ++tBounds->max.x;
        ++tBounds->max.y;
        
        dim.x = tBounds->max.x-tBounds->min.x+1;
        dim.y = tBounds->max.y-tBounds->min.y+1;
        
        *tDescent = font.metrics.tmDescent-(font.metrics.tmHeight-(s32)tBounds->max.y);
        align.x = tBounds->min.x - pre_step;
        align.y = (f32)font.metrics.tmAscent-tBounds->min.y-font.metrics.tmHeight/2;
    }
    
    bytes = (u8 *)xalloc((s32)(dim.x*dim.y)*4);
    
    if (foundTBox) {
        dstRow = bytes;
        srcRow = ((u8 *)font.bytes + (s32)tBounds->min.y*xd11.glyph_maker_size*4 + (s32)tBounds->min.x*4);
        for (y=(s32)tBounds->min.y; y<(s32)tBounds->max.y; ++y)
        {
            srcPx = (u32 *)srcRow;
            dstPx = (u32 *)dstRow;
            for (x=(s32)tBounds->min.x; x<(s32)tBounds->max.x; ++x)
            {
                color = *srcPx++;
                a = ((color >> 16) & 0xff);
                *dstPx++ =  RGBA(255,255,255, a);
            }
            dstRow += 4*(s32)dim.x;
            srcRow += 4*xd11.glyph_maker_size;
        }
    }
    
    r = xrender2d_sprite_from_bytes(atlas, bytes, ini2if(dim.x,dim.y));
    r.align = align;
    xfree(bytes);
    return r;
}

s32 xrender2d_font_height(s32 pointHeight)
{
    s32 result = MulDiv(pointHeight, 
                        GetDeviceCaps(xd11.glyph_maker_dc, LOGPIXELSY), 
                        GetDeviceCaps(xd11.glyph_maker_dc, LOGPIXELSX));
    return result;
}

void xrender2d_font_free(XFont f)
{
    s32 i;
    XGlyphHashNode *n;
    
    DeleteObject(f.bitmap);
    DeleteObject(f.handle);
    RemoveFontResourceW(f.path);
    
    for (i=0; i<XGLYPH_HASH_LENGTH; ++i)
        for (n=f.glyphs.storage[i]; n; n=n->next)
    {
        xfree(n);
        xfree(n->value);
    }
}

#endif