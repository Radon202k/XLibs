#ifndef RENDERER_H
#define RENDERER_H

/* =========================================================================
   INTERFACE
   ========================================================================= */

typedef struct
{
    XD11Texture texture;
    u8 *bytes;
    s32 bottom;
    v2i at;
} TextureAtlas;

typedef struct
{
    v2f size;
    rect2f uv;
    v2f align;
} Sprite;

typedef struct
{
    f32 lineadvance, charwidth, maxdescent;
    wchar_t path[MAX_PATH];
    HFONT handle;
    HBITMAP bitmap;
    TEXTMETRICW metrics;
    VOID *bytes;
    Table_T glyphs;
} Font;

typedef struct
{
    v2f position;
    v2f size;
    v4f color;
} RenderCommandRect;

typedef struct
{
    v2f position;
    v2f size;
    v4f color;
    Sprite sprite;
} RenderCommandSprite;

typedef struct RenderCommand
{
    union
    {
        RenderCommandRect rect;
        RenderCommandSprite sprite;
    };
    
    struct RenderCommand *next_free;
} RenderCommand;

typedef struct
{
    RenderCommand storage[4096];
    u32 index;
    RenderCommand *free_list;
} RenderCommandPool;

typedef struct
{
    xd11_tx2d texture;
    XD11DepthStencil depth_stencil;
} RenderTarget;

typedef struct
{
    /* Core variables */
    RenderTarget target_default;
    
    Array_T target_views; // ID3D11RenderTargetView
    
    TextureAtlas texture_atlas;
    
    /* Batches */
    
    /* Passes */
    XD11RenderPass pass_lines;
    XD11RenderPass pass_sprites;

    /* Resources */
    Sprite sprite_white;
    Sprite sprite_arrow;
} Renderer;

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

global Renderer renderer;

#include "shaders.h"

void renderer_initialize(void);
void renderer_shutdown(void);
void renderer_pre_update(void);
void renderer_post_update(void);

/* =========================================================================
   FONTS / GLYPHS
   ========================================================================= */

Font    renderer_font (TextureAtlas *a, wchar_t *path, wchar_t *name, int heightpoints);
Sprite  renderer_glyph_from_char (TextureAtlas *a, Font font, wchar_t *c, rect2f *tBounds, s32 *tDescent);
void    renderer_font_free   (Font font);
s32     renderer_font_height (s32 pointHeight);

/* =========================================================================
   TEXTURE / TEXTURE ATLAS / SPRITES
   ========================================================================= */

/*  A D11 Batch is composed of many vertices representing triangles texture
mapped to a single gpu texture that is called the texture atlas. A Sprite is a
smaller rectangle than the whole texture atlas that represents a texture in it
self, i.e., the texture atlas is composed of many different smaller textures.
  The point of that is faster performance. xspritepng creates a sprite from a
png file located at path, i.e., it loads the png and copies the bytes of the
texture into the texture atlas and fills XSprite with the corresponding uv coo
rdinates and other info about the source image. */


u8 *     renderer_load_png(wchar_t *path, v2i *dim, bool premulAlpha);

Sprite   renderer_sprite_from_png  (TextureAtlas *atlas, wchar_t *path, bool premulalpha);
Sprite   renderer_sprite_from_bytes(TextureAtlas *atlas, u8 *b, v2i dim);

u32      renderer_hash_unicode(const void *k);
bool     renderer_cmp_glyph_unicodes(const void *a, const void *b);

s32      renderer_font_height(s32 pointHeight);
void     renderer_font_free(Font f);

void     renderer_blit_simple_unchecked(u8 *dest, v2i dest_size, u8 *src, v2i at, v2i dim);

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

void renderer_create_line_pass(void);
void renderer_create_sprite_pass(void);
void renderer_create_resources(void);
void renderer_produce_textured_vertices(TexturedVertex **vertices, u32 *count);
void renderer_update_line_pass(void);
void renderer_update_sprite_pass(void);
void renderer_free_passes(void);













/* =========================================================================
   IMPLEMENTATION
   ========================================================================= */

void renderer_pre_update(void)
{
    MSG message;
    while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    
    renderer_update_line_pass();
    renderer_update_sprite_pass();
}

void renderer_post_update(void)
{
    xd11_update();
    xwin_update(true, xd11.window_size);
}

void renderer_depth_stencil(XD11RenderPass *pass,
                            D3D11_TEXTURE2D_DESC desc_texture,
                            D3D11_DEPTH_STENCIL_VIEW_DESC desc_view,
                            D3D11_DEPTH_STENCIL_DESC desc_state)
{
    /* Create depth stencil buffer texture */
    pass->depth_stencil.texture = xd11_texture2d(desc_texture, 0);
    
    /* Create depth stencil view */
    pass->depth_stencil.view = xd11_depth_stencil_view(pass->depth_stencil.texture, desc_view);
    
    /* Create depth stencil state */            
    pass->depth_stencil.state = xd11_depth_stencil_state(desc_state);
}

void renderer_initialize(void)
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    
    /* Initialize Input */
    xwin_initialize((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.window_handle});
    
    /* Get back buffer texture from swap chain */    
    renderer.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    renderer.target_views = Array_new(1, sizeof(xd11_tgvw *));
    Array_push(&renderer.target_views, xd11_target_view(renderer.target_default.texture));
    
    /* Create depth stencil texture/view/state */
    renderer_depth_stencil(&renderer.pass_sprites,
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
    
    /* Texture atlas texture */
    {
        u32 atlas_size = 1024;
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
        renderer.texture_atlas.texture.handle = xd11_texture2d(desc, 0);

        renderer.texture_atlas.texture.size   = fil2i(atlas_size);

        /* Create CPU memory bitmap */
        s32 bitmap_size = renderer.texture_atlas.texture.size.x * 
            renderer.texture_atlas.texture.size.y * 4;


        renderer.texture_atlas.bytes  = (u8*)xalloc(bitmap_size);
    }
    
    /* Create passes */
    renderer_create_line_pass();
    renderer_create_sprite_pass();
    
    /* Create resources */
    renderer_create_resources();
}

void renderer_shutdown(void)
{
    renderer_free_passes();
    xfree(renderer.texture_atlas.bytes);
    xd11_shutdown();
}

void renderer_create_line_pass(void)
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
    renderer.pass_lines.vertex_shader = xd11_compile_vertex_shader(linesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    renderer.pass_lines.pixel_shader = xd11_compile_pixel_shader(linesPS, &compiledPS);
    
    /* Input layout */
    renderer.pass_lines.input_layout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    renderer.pass_lines.target_view = Array_get(renderer.target_views, 0);
    
    /* Blend state */
    renderer.pass_lines.blend_state = xd11_blend_state((D3D11_BLEND_DESC)
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
    renderer.pass_lines.rasterizer_state = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    renderer.pass_lines.sampler_state = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    renderer.pass_lines.vertex_buffers.count = 1;
    renderer.pass_lines.vertex_buffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.pass_lines.vertex_buffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
                                                  {
                                                      256*sizeof(LineVertex), 
                                                      D3D11_USAGE_DYNAMIC, 
                                                      D3D11_BIND_VERTEX_BUFFER,
                                                      D3D11_CPU_ACCESS_WRITE, 
                                                      0, 
                                                      sizeof(LineVertex)
                                                  },
                                                  0);
    
    /* Vertex buffer strides */
    renderer.pass_lines.vertex_buffers.strides = xalloc(1*sizeof(UINT));
    renderer.pass_lines.vertex_buffers.strides[0] = sizeof(LineVertex);
    
    /* Vertex buffer offsets */
    renderer.pass_lines.vertex_buffers.offsets = xalloc(1*sizeof(UINT));
    renderer.pass_lines.vertex_buffers.offsets[0] = 0;
    
    /* Vertex shader constant buffers */
    renderer.pass_lines.vs_cbuffers = Array_new(1, sizeof(xd11_buff));
    Array_push(&renderer.pass_lines.vs_cbuffers, xd11_buffer((D3D11_BUFFER_DESC)
                                                      {
                                                          sizeof(mat4f),
                                                          D3D11_USAGE_DEFAULT, 
                                                          D3D11_BIND_CONSTANT_BUFFER,
                                                          0,
                                                          0,
                                                          0,
                                                      },
                                                      0));
    
    /* Pixel shader resources */
    renderer.pass_lines.ps_resources = Array_new(1, sizeof(xd11_srvw));
    Array_push(&renderer.pass_lines.ps_resources, xd11_shader_res_view(renderer.texture_atlas.texture.handle,
                                                              (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                                              {
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                                  D3D_SRV_DIMENSION_TEXTURE2D,
                                                                  .Texture2D = (D3D11_TEX2D_SRV){0,1}
                                                              }));
    
    /* Topology */
    renderer.pass_lines.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

    /* Viewports */
    renderer.pass_lines.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&renderer.pass_lines.viewports, 
        &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    /* Scissors */
    renderer.pass_lines.scissors = Array_new(1, sizeof(D3D11_RECT));
    Array_push(&renderer.pass_lines.scissors,
        &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
}

void renderer_create_sprite_pass(void)
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
    renderer.pass_sprites.vertex_shader = xd11_compile_vertex_shader(spritesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    renderer.pass_sprites.pixel_shader = xd11_compile_pixel_shader(spritesPS, &compiledPS);
    
    /* Input layout */
    renderer.pass_sprites.input_layout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Render target view */
    renderer.pass_sprites.target_view = Array_get(renderer.target_views, 0);
    
    /* Blend state */
    renderer.pass_sprites.blend_state = xd11_blend_state((D3D11_BLEND_DESC)
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
    renderer.pass_sprites.rasterizer_state = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    renderer.pass_sprites.sampler_state = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    renderer.pass_sprites.vertex_buffers.count = 1;
    renderer.pass_sprites.vertex_buffers.array = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.pass_sprites.vertex_buffers.array[0] = xd11_buffer((D3D11_BUFFER_DESC)
                                                    {
                                                        256*sizeof(TexturedVertex), 
                                                        D3D11_USAGE_DYNAMIC, 
                                                        D3D11_BIND_VERTEX_BUFFER,
                                                        D3D11_CPU_ACCESS_WRITE, 
                                                        0, 
                                                        sizeof(TexturedVertex)
                                                    },
                                                    0);
    /* Vertex buffers strides */
    renderer.pass_sprites.vertex_buffers.strides = xalloc(1*sizeof(UINT));
    renderer.pass_sprites.vertex_buffers.strides[0] = sizeof(TexturedVertex);
    
    /* Vertex buffers offsets */
    renderer.pass_sprites.vertex_buffers.offsets = xalloc(1*sizeof(UINT));
    renderer.pass_sprites.vertex_buffers.offsets[0] = 0;
    
    /* Vertex Shader Constant buffers */
    renderer.pass_sprites.vs_cbuffers = Array_new(1, sizeof(xd11_buff));
    Array_push(&renderer.pass_sprites.vs_cbuffers,
        xd11_buffer((XD11_BUFD)
        {
            sizeof(mat4f),
            D3D11_USAGE_DEFAULT, 
            D3D11_BIND_CONSTANT_BUFFER,
            0,
            0,
            0,
        },
        0));

    /* Pixel Shader Resources */
    renderer.pass_sprites.ps_resources = Array_new(1, sizeof(xd11_srvw));
    Array_push(&renderer.pass_sprites.ps_resources,
        xd11_shader_res_view(renderer.texture_atlas.texture.handle,
        (D3D11_SHADER_RESOURCE_VIEW_DESC)
        {
            DXGI_FORMAT_R8G8B8A8_UNORM,
            D3D_SRV_DIMENSION_TEXTURE2D,
            .Texture2D = (D3D11_TEX2D_SRV){0,1}
        }));

    /* Topology */
    renderer.pass_sprites.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    /* Viewports */
    renderer.pass_sprites.viewports = Array_new(1, sizeof(D3D11_VIEWPORT));
    Array_push(&renderer.pass_sprites.viewports,
        &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});
    
    /* Scissors */
    renderer.pass_sprites.scissors = Array_new(1, sizeof(D3D11_RECT));
    Array_push(&renderer.pass_sprites.scissors,
        &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
}

void renderer_create_resources(void)
{
    /* Create resources */
    v2i white_dim;
    u8 *white_bytes = renderer_load_png(L"images/white.png", &white_dim, false);
    renderer.sprite_white = renderer_sprite_from_bytes(&renderer.texture_atlas, white_bytes, white_dim);
    xfree(white_bytes);
    
    renderer.sprite_arrow = renderer_sprite_from_png(&renderer.texture_atlas, L"images/arrow128.png", false);
    
    xd11_texture2d_update(renderer.texture_atlas.texture, renderer.texture_atlas.bytes);
}

void renderer_free_passes(void)
{
    // Lines
    xfree(renderer.pass_lines.vertex_buffers.array);
    xfree(renderer.pass_lines.vertex_buffers.strides);
    xfree(renderer.pass_lines.vertex_buffers.offsets);
    Array_free(&renderer.pass_lines.vs_cbuffers);
    Array_free(&renderer.pass_lines.ps_resources);
    Array_free(&renderer.pass_lines.viewports);
    Array_free(&renderer.pass_lines.scissors);
    
    // Sprites
    xfree(renderer.pass_sprites.vertex_buffers.array);
    xfree(renderer.pass_sprites.vertex_buffers.strides);
    xfree(renderer.pass_sprites.vertex_buffers.offsets);
    Array_free(&renderer.pass_sprites.vs_cbuffers);
    Array_free(&renderer.pass_sprites.ps_resources);
    Array_free(&renderer.pass_sprites.viewports);
    Array_free(&renderer.pass_sprites.scissors);
    
    // Core
    Array_free(&renderer.target_views);
}

void renderer_produce_textured_vertices(TexturedVertex **vertices, u32 *count)
{
    v2f p = xwin.mouse.pos;
    
    TexturedVertex data[6] =
    {
        {{    p.x,     p.y,  0}, {renderer.sprite_arrow.uv.min.x, renderer.sprite_arrow.uv.min.y}, {1,1,1,1}},
        {{p.x+100,     p.y,  0}, {renderer.sprite_arrow.uv.max.x, renderer.sprite_arrow.uv.min.y}, {1,1,1,1}},
        {{p.x+100, p.y+100,  0}, {renderer.sprite_arrow.uv.max.x, renderer.sprite_arrow.uv.max.y}, {1,1,1,1}},
        
        {{p.x+100, p.y+100,  0}, {renderer.sprite_arrow.uv.max.x, renderer.sprite_arrow.uv.max.y}, {1,1,1,1}},
        {{    p.x, p.y+100,  0}, {renderer.sprite_arrow.uv.min.x, renderer.sprite_arrow.uv.max.y}, {1,1,1,1}},
        {{    p.x,     p.y,  0}, {renderer.sprite_arrow.uv.min.x, renderer.sprite_arrow.uv.min.y}, {1,1,1,1}},
    };
    
    // TODO: if there is any
    *vertices = xalloc(6*sizeof(TexturedVertex));
    
    memcpy(*vertices, data, 6*sizeof(TexturedVertex));
    *count = 6;
}

void renderer_update_line_pass(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.back_buffer_size.x, 0, 0, -1,
        0, -2.0f / xd11.back_buffer_size.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    /* Update constant buffers */
    xd11_update_subres(Array_get(renderer.pass_lines.vs_cbuffers, 0), &matrixProjection);
    
    /* Set render target */
    xd11_set_render_target(renderer.pass_lines.target_view, renderer.pass_lines.depth_stencil.view);
    
    /* Clear */
    xd11_clear_rtv(renderer.pass_lines.target_view, eme4f);
    xd11_clear_dsv(renderer.pass_lines.depth_stencil.view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    /* Update viewport and scissor rect*/
    Array_set(renderer.pass_lines.viewports, 0, 
        &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});

    Array_set(renderer.pass_lines.scissors, 0,
        &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
    
    LineVertex data[2] =
    {
        {{0,0,0}, {1,1,1,1}},
        {{xd11.back_buffer_size.x,xd11.back_buffer_size.y,0}, {1,1,1,1}},
    };
    
    xd11_buffer_update(renderer.pass_lines.vertex_buffers.array[0], data, 2*sizeof(LineVertex));
    xd11_render_pass(&renderer.pass_lines, 2);
}

void renderer_update_sprite_pass(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.back_buffer_size.x, 0, 0, -1,
        0, -2.0f / xd11.back_buffer_size.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xd11_update_subres(Array_get(renderer.pass_sprites.vs_cbuffers, 0), &matrixProjection);
    
    /* Update viewport and scissor rect*/
    Array_set(renderer.pass_sprites.viewports, 0,
        &(D3D11_VIEWPORT){0,0,xd11.back_buffer_size.x,xd11.back_buffer_size.y, 0, 1});

    Array_set(renderer.pass_sprites.scissors, 0,
        &(D3D11_RECT){0,0,(LONG)xd11.back_buffer_size.x,(LONG)xd11.back_buffer_size.y});
    
    u32 vertexCount = 0;
    TexturedVertex *vertices = 0;
    renderer_produce_textured_vertices(&vertices, &vertexCount);
    
    if (vertexCount > 0)
        xfree(vertices);
    
    xd11_buffer_update(renderer.pass_sprites.vertex_buffers.array[0], vertices, vertexCount*sizeof(TexturedVertex));
    xd11_render_pass(&renderer.pass_sprites, vertexCount);
}

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (message) {
        XWNDPROC;
        
        default:
        {
            result = DefWindowProcW(window, message, wParam, lParam);
        } break;
    }
    return result;
}

void xd11_resized(void)
{
    /* Release views and textures */
    for (s32 i=0; i<renderer.target_views.top; ++i)
        ID3D11RenderTargetView_Release((xd11_tgvw)Array_get(renderer.target_views, i));
    
    ID3D11DepthStencilView_Release(renderer.pass_lines.depth_stencil.view);
    ID3D11Texture2D_Release       (renderer.pass_lines.depth_stencil.texture);

    ID3D11DepthStencilView_Release(renderer.pass_sprites.depth_stencil.view);
    ID3D11Texture2D_Release       (renderer.pass_sprites.depth_stencil.texture);

    ID3D11Texture2D_Release(renderer.target_default.texture);
    
    /* Resize the swapchain buffers */
    IDXGISwapChain_ResizeBuffers(xd11.swap_chain,
                                 2, 
                                 (UINT)xd11.back_buffer_size.x,
                                 (UINT)xd11.back_buffer_size.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get backbuffer texture from swap chain */
    renderer.target_default.texture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    Array_set(renderer.target_views, 0,
        xd11_target_view(renderer.target_default.texture));
    
    /* Create depth stencil texture */
    renderer.target_default.depth_stencil.texture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
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
    renderer.target_default.depth_stencil.view = 
    xd11_depth_stencil_view(renderer.target_default.depth_stencil.texture,
        (D3D11_DEPTH_STENCIL_VIEW_DESC)
        {
            DXGI_FORMAT_D24_UNORM_S8_UINT, 
            D3D11_DSV_DIMENSION_TEXTURE2D, 
            .Texture2D = (D3D11_TEX2D_DSV){0}
        });
}

u8 *renderer_load_png(wchar_t *path, v2i *dim, bool premulalpha)
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

void renderer_blit_simple_unchecked(u8 *dst, v2i dest_size,
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

Sprite renderer_sprite_from_bytes(TextureAtlas *a, u8 *b, v2i dim)
{
    s32 m;
    u8 *dst, *src;
    Sprite r;
    
    m = 1;
    
    if ((a->at.x + dim.x + m) > a->texture.size.x)
        a->at = (v2i){0, a->bottom};
    
    if (m+a->bottom < (a->at.y + dim.y))
        a->bottom = a->at.y + dim.y + m;
    
    assert(a->bottom <= a->texture.size.y);
    
    dst = a->bytes;
    src = b;
    renderer_blit_simple_unchecked(a->bytes, a->texture.size, src, a->at, dim);
    
    r.uv.min = (v2f){a->at.x / (f32)a->texture.size.x,
        ( (a->at.y + dim.y) ) / (f32)a->texture.size.y};
    
    r.uv.max = (v2f){(a->at.x + dim.x) / (f32)a->texture.size.x, 
        ( a->at.y ) / (f32)a->texture.size.y};
    
    r.size = ini2fs(dim.x,dim.y);
    
    a->at.x += dim.x+m;
    
    return r;
}

Sprite renderer_sprite_from_png(TextureAtlas *a, wchar_t *path, bool premulalpha)
{
    Sprite r;
    v2i dim;
    u8 *b;
    
    memset(&r, 0, sizeof(r));
    b = renderer_load_png(path, &dim, premulalpha);
    if (b)
    {
        r = renderer_sprite_from_bytes(a, b, dim);
        xfree(b);
    }
    return r;
}


bool renderer_cmp_glyph_unicodes(const void *a, const void *b)
{
    u32 ua, ub;
    
    ua = *(u32 *)a;
    ub = *(u32 *)b;
    return (ua == ub);
}

u32 renderer_hash_unicode(const void *k)
{
    u32 u;
    
    u = *(u32 *)k;
    u = (u & (512-1));
    return u;
}

Font renderer_font(TextureAtlas *atlas, wchar_t *path, wchar_t *name, int heightpoints)
{
    Font result;
    memset(&result, 0, sizeof(result));
    
    v2i maxGlyphSize;
    s32 maxDescent;
    
    s32 i;
    
    wchar_t c[2];
    u32 *k;
    Sprite *v;
    
    /* Add font resource to Windows */
    s32 temp = AddFontResourceW(path);
    assert(temp == 1);
    
    /* Create the font */
    result.handle = CreateFontW(renderer_font_height(heightpoints), 
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
    result.charwidth = (f32)metrics.tmAveCharWidth;
    result.glyphs = Table_new(512, renderer_cmp_glyph_unicodes, renderer_hash_unicode);
    
    /* D11 some glyphs from the ASCII range */
    maxGlyphSize.x = maxGlyphSize.y = maxDescent = -10000;
    for (i=32; i<=126; ++i)
    {
        c[0]=(wchar_t)i;
        c[1]='\0';
        
        /* Allocate memory for the hash table key/value pair */
        k = xalloc(sizeof *k);
        v = xalloc(sizeof *v);
        
        /* Store key, generate the glyph sprite and store value */
        *k = (u32)i;
        
        rect2f tightBounds;
        s32 tightDescent;
        *v = renderer_glyph_from_char(atlas, result, c, &tightBounds, &tightDescent);
        
        /* Spaces wont have anything so tightBounds wont be found  */
        if (tightBounds.max.x!=0 && tightBounds.max.y!=0)
        {
            /* Calculate tight size */
            v2i tightSize = (v2i){(s32)(tightBounds.max.x - tightBounds.min.x),
                (s32)(tightBounds.max.y - tightBounds.min.y)};
            
            /* Book keep maximum glyph size and maximum descent */
            if (maxGlyphSize.x < tightSize.x) maxGlyphSize.x = tightSize.x;
            if (maxGlyphSize.y < tightSize.y) maxGlyphSize.y = tightSize.y;
            if (maxDescent < tightDescent) maxDescent = tightDescent;
        }
        
        /* Set the glyph in the hash table */
        Table_set(result.glyphs, k, v);
    }
    
    result.lineadvance = (f32)maxGlyphSize.y;
    result.maxdescent = (f32)maxDescent;
    xstrcpy(result.path, MAX_PATH, path);
    
    return result;
}

Sprite renderer_glyph_from_char(TextureAtlas *atlas, Font font, wchar_t *c, 
                                rect2f *tBounds, s32 *tDescent)
{
    u8 *b, *dstRow, *srcRow;
    s32 charsz, i,j, x,y;
    u32 *dstPx, *srcPx, *px, color, a, ps;
    v2f d, al;
    Sprite r;
    SIZE size;
    rect2f bounds;
    
    ps=(s32)(0.3f*font.lineadvance);
    GetTextExtentPoint32W(xd11.glyph_maker_dc, c, 1, &size);
    al = (v2f){0,0};
    d = ini2fs(size.cx,size.cy);
    charsz=(s32)wcslen(c);
    bounds = inir2f(0,0, d.x,d.y);
    tBounds->min.x=tBounds->min.y=1000000;
    tBounds->max.x=tBounds->max.y=-1000000;
    
    TextOutW(xd11.glyph_maker_dc, ps,0, c, charsz);
    
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
        
        d.x=tBounds->max.x-tBounds->min.x+1;
        d.y=tBounds->max.y-tBounds->min.y+1;
        
        *tDescent = font.metrics.tmDescent-(font.metrics.tmHeight-(s32)tBounds->max.y);
        al.x = tBounds->min.x-ps;
        al.y = (f32)*tDescent;
    }
    
    b = (u8 *)xalloc((s32)(d.x*d.y)*4);
    
    if (foundTBox) {
        dstRow = b;
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
            dstRow += 4*(s32)d.x;
            srcRow += 4*xd11.glyph_maker_size;
        }
    }
    
    r = renderer_sprite_from_bytes(atlas, b, ini2if(d.x,d.y));
    r.align = al;
    xfree(b);
    return r;
}

s32 renderer_font_height(s32 pointHeight)
{
    s32 result = MulDiv(pointHeight, 
                        GetDeviceCaps(xd11.glyph_maker_dc, LOGPIXELSY), 
                        GetDeviceCaps(xd11.glyph_maker_dc, LOGPIXELSX));
    return result;
}

void renderer_font_free(Font f)
{
    s32 i;
    Table_node n;
    
    DeleteObject(f.bitmap);
    DeleteObject(f.handle);
    RemoveFontResourceW(f.path);
    
    for (i=0; i<f.glyphs->size; ++i)
        for (n=f.glyphs->storage[i]; n; n=n->next)
    {
        xfree(n->key);
        xfree(n->value);
    }
    
    Table_free(f.glyphs);
}


#endif //RENDERER_H
