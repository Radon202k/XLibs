#ifndef RENDERER_H
#define RENDERER_H

/* ============================================================================
INTERFACE
 ============================================================================*/

typedef struct LineVertex {v3f position; v4f color;} LineVertex;

typedef struct TexturedVertex {v3f position; v2f uvs; v4f color;} TexturedVertex;

typedef struct Renderer
{
    /* Core variables */
    ID3D11Texture2D *backBufferTexture;
    ID3D11Texture2D *depthStencilTexture;
    u32 renderTargetViewCount;
    ID3D11RenderTargetView **renderTargetViews;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11DepthStencilState *depthStencilState;
    XD11TextureAtlas textureAtlas;
    
    /* Batches */
    XD11Batch lines;
    XD11Batch sprites;
    
    /* Resources */
    XD11Sprite whiteSprite;
    XD11Sprite arrowSprite;
} Renderer;

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

global Renderer renderer;

#include "shaders.h"

void renderer_initialize(void);
void renderer_shutdown(void);
void renderer_pre_update(void);
void renderer_post_update(void);

/* ============================================================================
END OF INTERFACE
 ============================================================================*/













/* ============================================================================
IMPLEMENTATION
 ============================================================================*/

void renderer_create_line_batch(void);
void renderer_create_sprite_batch(void);
void renderer_create_resources(void);
void renderer_produce_textured_vertices(TexturedVertex **vertices, u32 *count);
void renderer_update_line_batch(void);
void renderer_update_sprite_batch(void);
void renderer_free_batches(void);

void renderer_pre_update(void)
{
    MSG message;
    while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    
    renderer_update_line_batch();
    renderer_update_sprite_batch();
}

void renderer_post_update(void)
{
    xd11_update();
    xwindow_update(true, xd11.windowSize);
}

void renderer_initialize(void)
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config){window_proc, 0, 0, L"My Window"});
    
    /* Initialize Input */
    xwindow_init((XWindowConfig){LoadCursor(NULL, IDC_ARROW), xd11.windowHandle});
    
    /* Get back buffer texture from swap chain */    
    renderer.backBufferTexture = xd11_swapchain_get_buffer();
    
    /* Create depth stencil buffer texture */
    {
        D3D11_TEXTURE2D_DESC desc =
        {
            (s32)xd11.backBufferSize.x, // Width
            (s32)xd11.backBufferSize.y, // Height
            0, // Mip levels
            1, // Array size
            DXGI_FORMAT_D24_UNORM_S8_UINT, // Format
            (DXGI_SAMPLE_DESC){1,0}, // Sample desc
            D3D11_USAGE_DEFAULT, // Usage
            D3D11_BIND_DEPTH_STENCIL, // Bind flags
            0, // Cpu access flags
            0 // Misc flags
        };
        renderer.depthStencilTexture = xd11_texture2d(desc, 0);
    }
    
    /* Create render target view */
    renderer.renderTargetViewCount = 1;
    renderer.renderTargetViews     = xalloc(1*sizeof(ID3D11RenderTargetView *));
    renderer.renderTargetViews[0]  = xd11_render_target_view(renderer.backBufferTexture);
    
    /* Create depth stencil view */
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC desc =
        {
            DXGI_FORMAT_D24_UNORM_S8_UINT, // format
            D3D11_DSV_DIMENSION_TEXTURE2D, // dim
            .Texture2D = (D3D11_TEX2D_DSV){0}
        };
        renderer.depthStencilView = xd11_depth_stencil_view(renderer.depthStencilTexture, desc);
    }
    
    /* Create depth stencil state */            
    {
        D3D11_DEPTH_STENCIL_DESC desc = 
        {
            true, // Depth enable
            D3D11_DEPTH_WRITE_MASK_ALL, // Depth write mask
            D3D11_COMPARISON_GREATER_EQUAL, // Depth func
            false, // Stencil enable
            0, // Stencil read mask
            0, // Stencil write mask
            {0}, // Front face
            {0}, // Back face
        };
        renderer.depthStencilState = xd11_depth_stencil_state(desc);
    }
    
    /* Texture atlas texture */
    {
        /* Create D3D11 texture */
        renderer.textureAtlas.size = 1024;
        D3D11_TEXTURE2D_DESC desc =
        {
            renderer.textureAtlas.size, // Width
            renderer.textureAtlas.size, // Height
            1, // Mip levels 
            1, // Array size
            DXGI_FORMAT_R8G8B8A8_UNORM, // Format
            {1,0}, // Sample
            D3D11_USAGE_DYNAMIC, // Usage 
            D3D11_BIND_SHADER_RESOURCE, // Bind flags
            D3D11_CPU_ACCESS_WRITE, // Cpu access flags
            0 // Misc flags
        };
        renderer.textureAtlas.texture.handle = xd11_texture2d(desc, 0);
        
        /* Create CPU memory bitmap */
        s32 bitmapSize = renderer.textureAtlas.size*renderer.textureAtlas.size*4;
        renderer.textureAtlas.texture.size   = fil2i(renderer.textureAtlas.size);
        renderer.textureAtlas.texture.bytes  = (u8*)xalloc(bitmapSize);
    }
    
    /* Create batches */
    renderer_create_line_batch();
    renderer_create_sprite_batch();
    
    /* Create resources */
    renderer_create_resources();
}

void renderer_shutdown(void)
{
    renderer_free_batches();
    xfree(renderer.textureAtlas.texture.bytes);
    xd11_shutdown();
}

void renderer_create_line_batch(void)
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
    renderer.lines.vertexShader = xd11_compile_vertex_shader(linesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    renderer.lines.pixelShader = xd11_compile_pixel_shader(linesPS, &compiledPS);
    
    /* Input layout */
    renderer.lines.inputLayout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Back buffer */
    renderer.lines.backBufferTexture = renderer.backBufferTexture;
    
    /* Render target view */
    renderer.lines.renderTargetView = renderer.renderTargetViews[0];
    
    /* Depth stencil state/view/texture */
    // TODO: Each batch should have its own depth stencil setup?
    renderer.lines.depthStencilState = renderer.depthStencilState;
    renderer.lines.depthStencilView = renderer.depthStencilView;
    renderer.lines.depthStencilTexture = renderer.depthStencilTexture;
    
    /* Blend state */
    renderer.lines.stateBlend = xd11_blend_state((D3D11_BLEND_DESC)
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
    renderer.lines.stateRasterizer = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    renderer.lines.stateSampler = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    renderer.lines.vertexBufferCount = 1;
    renderer.lines.vertexBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.lines.vertexBuffers[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    renderer.lines.vertexBufferStrides = xalloc(1*sizeof(UINT));
    renderer.lines.vertexBufferStrides[0] = sizeof(LineVertex);
    
    /* Vertex buffer offsets */
    renderer.lines.vertexBufferOffsets = xalloc(1*sizeof(UINT));
    renderer.lines.vertexBufferOffsets[0] = 0;
    
    /* Vertex shader constant buffers */
    renderer.lines.vsConstantBufferCount = 1;
    renderer.lines.vsConstantBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.lines.vsConstantBuffers[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    renderer.lines.psResourceCount = 1;
    renderer.lines.psResources = xalloc(1*sizeof(ID3D11ShaderResourceView *));
    renderer.lines.psResources[0] = xd11_shader_resource_view(renderer.textureAtlas.texture.handle,
                                                              (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                                              {
                                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                                  D3D_SRV_DIMENSION_TEXTURE2D,
                                                                  .Texture2D = (D3D11_TEX2D_SRV){.MostDetailedMip=0, .MipLevels=1}
                                                              });
    
    /* Topology */
    renderer.lines.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
    
    /* Vertex shader */
    renderer.lines.vertexShader = renderer.lines.vertexShader;
    
    /* Pixel shader */
    renderer.lines.pixelShader = renderer.lines.pixelShader;
    
    /* Input layout */
    renderer.lines.inputLayout = renderer.lines.inputLayout;
    
    /* Viewports */
    renderer.lines.viewportCount = 1;
    renderer.lines.viewports = xalloc(1*sizeof(D3D11_VIEWPORT)); 
    renderer.lines.viewports[0] = (D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1};
    
    /* Scissors */
    renderer.lines.scissorCount = 1;
    renderer.lines.scissors = xalloc(1*sizeof(D3D11_RECT));
    renderer.lines.scissors[0] = (D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y};
}

void renderer_create_sprite_batch(void)
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
    renderer.sprites.vertexShader = xd11_compile_vertex_shader(spritesVS, &compiledVS);
    
    /* Pixel shader */
    ID3DBlob *compiledPS;
    renderer.sprites.pixelShader = xd11_compile_pixel_shader(spritesPS, &compiledPS);
    
    /* Input layout */
    renderer.sprites.inputLayout = xd11_input_layout(compiledVS, inputFormat, narray(inputFormat));
    
    /* Back buffer */
    renderer.sprites.backBufferTexture = renderer.backBufferTexture;
    
    /* Render target view */
    renderer.sprites.renderTargetView = renderer.renderTargetViews[0];
    
    /* Depth stencil state/view/texture */
    renderer.sprites.depthStencilState = renderer.depthStencilState;
    renderer.sprites.depthStencilView = renderer.depthStencilView;
    renderer.sprites.depthStencilTexture = renderer.depthStencilTexture;
    
    /* Blend state */
    renderer.sprites.stateBlend = xd11_blend_state((D3D11_BLEND_DESC)
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
    renderer.sprites.stateRasterizer = xd11_rasterizer_state((D3D11_RASTERIZER_DESC)
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
    renderer.sprites.stateSampler = xd11_sampler_state((D3D11_SAMPLER_DESC)
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
    renderer.sprites.vertexBufferCount = 1;
    renderer.sprites.vertexBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.sprites.vertexBuffers[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    renderer.sprites.vertexBufferStrides = xalloc(1*sizeof(UINT));
    renderer.sprites.vertexBufferStrides[0] = sizeof(TexturedVertex);
    
    /* Vertex buffers offsets */
    renderer.sprites.vertexBufferOffsets = xalloc(1*sizeof(UINT));
    renderer.sprites.vertexBufferOffsets[0] = 0;
    
    /* Vertex Shader Constant buffers */
    renderer.sprites.vsConstantBufferCount = 1;
    renderer.sprites.vsConstantBuffers = xalloc(1*sizeof(ID3D11Buffer *));
    renderer.sprites.vsConstantBuffers[0] = xd11_buffer((D3D11_BUFFER_DESC)
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
    renderer.sprites.psResourceCount = 1;
    renderer.sprites.psResources = xalloc(1*sizeof(ID3D11ShaderResourceView *));
    renderer.sprites.psResources[0] = xd11_shader_resource_view(renderer.textureAtlas.texture.handle,
                                                                (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                                                {
                                                                    DXGI_FORMAT_R8G8B8A8_UNORM,
                                                                    D3D_SRV_DIMENSION_TEXTURE2D,
                                                                    .Texture2D = (D3D11_TEX2D_SRV){0,1}
                                                                });
    
    /* Topology */
    renderer.sprites.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    
    /* Viewports */
    renderer.sprites.viewportCount = 1;
    renderer.sprites.viewports = xalloc(1*sizeof(D3D11_VIEWPORT));
    renderer.sprites.viewports[0] = (D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1};
    
    /* Scissors */
    renderer.sprites.scissorCount = 1;
    renderer.sprites.scissors = xalloc(1*sizeof(D3D11_RECT));
    renderer.sprites.scissors[0] = (D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y};
}

void renderer_create_resources(void)
{
    /* Create resources */
    v2i whiteDim;
    u8 *whiteBytes = xd11_load_png(L"images/white.png", &whiteDim, false);
    renderer.whiteSprite = xd11_sprite_from_bytes(&renderer.textureAtlas, whiteBytes, whiteDim);
    xfree(whiteBytes);
    
    renderer.arrowSprite = xd11_sprite_from_png(&renderer.textureAtlas, L"images/arrow128.png", false);
    
    xd11_texture_update(renderer.textureAtlas.texture, renderer.textureAtlas.texture.bytes);
}

void renderer_free_batches(void)
{
    // Lines
    xfree(renderer.lines.vertexBuffers);
    xfree(renderer.lines.vertexBufferStrides);
    xfree(renderer.lines.vertexBufferOffsets);
    xfree(renderer.lines.vsConstantBuffers);
    xfree(renderer.lines.psResources);
    xfree(renderer.lines.viewports);
    xfree(renderer.lines.scissors);
    
    // Sprites
    xfree(renderer.sprites.vertexBuffers);
    xfree(renderer.sprites.vertexBufferStrides);
    xfree(renderer.sprites.vertexBufferOffsets);
    xfree(renderer.sprites.vsConstantBuffers);
    xfree(renderer.sprites.psResources);
    xfree(renderer.sprites.viewports);
    xfree(renderer.sprites.scissors);
    
    // Core
    xfree(renderer.renderTargetViews);
}

void renderer_produce_textured_vertices(TexturedVertex **vertices, u32 *count)
{
    v2f p = xwin.mouse.pos;
    
    TexturedVertex data[6] =
    {
        {{    p.x,     p.y,  0}, {renderer.arrowSprite.uv.min.x, renderer.arrowSprite.uv.min.y}, {1,1,1,1}},
        {{p.x+100,     p.y,  0}, {renderer.arrowSprite.uv.max.x, renderer.arrowSprite.uv.min.y}, {1,1,1,1}},
        {{p.x+100, p.y+100,  0}, {renderer.arrowSprite.uv.max.x, renderer.arrowSprite.uv.max.y}, {1,1,1,1}},
        
        {{p.x+100, p.y+100,  0}, {renderer.arrowSprite.uv.max.x, renderer.arrowSprite.uv.max.y}, {1,1,1,1}},
        {{    p.x, p.y+100,  0}, {renderer.arrowSprite.uv.min.x, renderer.arrowSprite.uv.max.y}, {1,1,1,1}},
        {{    p.x,     p.y,  0}, {renderer.arrowSprite.uv.min.x, renderer.arrowSprite.uv.min.y}, {1,1,1,1}},
    };
    
    // TODO: if there is any
    *vertices = xalloc(6*sizeof(TexturedVertex));
    
    memcpy(*vertices, data, 6*sizeof(TexturedVertex));
    *count = 6;
}

void renderer_update_line_batch(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.backBufferSize.x, 0, 0, -1,
        0, -2.0f / xd11.backBufferSize.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    /* Update constant buffers */
    xd11_update_subresource(renderer.lines.vsConstantBuffers[0], &matrixProjection);
    
    /* Set render target */
    xd11_set_render_target(renderer.lines.renderTargetView, renderer.depthStencilView);
    
    /* Clear */
    xd11_clear_rtv(renderer.lines.renderTargetView, eme4f);
    xd11_clear_dsv(renderer.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
    
    /* Update viewport and scissor rect*/
    renderer.lines.viewports[0] = (D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1};
    renderer.lines.scissors[0]  = (D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y};
    
    LineVertex data[2] =
    {
        {{0,0,0}, {1,1,1,1}},
        {{xd11.backBufferSize.x,xd11.backBufferSize.y,0}, {1,1,1,1}},
    };
    
    xd11_buffer_update(renderer.lines.vertexBuffers[0], data, 2*sizeof(LineVertex));
    xd11_batch_draw(&renderer.lines, 2);
}

void renderer_update_sprite_batch(void)
{
    mat4f matrixProjection =
    {
        2.0f / xd11.backBufferSize.x, 0, 0, -1,
        0, -2.0f / xd11.backBufferSize.y, 0, 1.f,
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xd11_update_subresource(renderer.sprites.vsConstantBuffers[0], &matrixProjection);
    
    /* Update viewport and scissor rect*/
    renderer.sprites.viewports[0] = (D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1};
    renderer.sprites.scissors[0]  = (D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y};
    
    u32 vertexCount = 0;
    TexturedVertex *vertices = 0;
    renderer_produce_textured_vertices(&vertices, &vertexCount);
    if (vertexCount > 0)
        xfree(vertices);
    
    xd11_buffer_update(renderer.sprites.vertexBuffers[0], vertices, vertexCount*sizeof(TexturedVertex));
    xd11_batch_draw(&renderer.sprites, vertexCount);
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
    for (u32 i=0; i<renderer.renderTargetViewCount; ++i)
        ID3D11RenderTargetView_Release(renderer.renderTargetViews[i]);
    
    
    ID3D11DepthStencilView_Release(renderer.depthStencilView);
    ID3D11Texture2D_Release(renderer.backBufferTexture);
    ID3D11Texture2D_Release(renderer.depthStencilTexture);
    
    /* Resize the swapchain buffers */
    IDXGISwapChain_ResizeBuffers(xd11.swapChain,
                                 2, 
                                 (UINT)xd11.backBufferSize.x,
                                 (UINT)xd11.backBufferSize.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get backbuffer texture from swap chain */
    renderer.backBufferTexture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    renderer.renderTargetViews[0] = xd11_render_target_view(renderer.backBufferTexture);
    renderer.lines.renderTargetView = renderer.renderTargetViews[0];
    renderer.sprites.renderTargetView = renderer.renderTargetViews[0];
    
    /* Create depth stencil texture */
    renderer.depthStencilTexture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
                                                  {
                                                      (s32)xd11.backBufferSize.x,
                                                      (s32)xd11.backBufferSize.y,
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
    renderer.depthStencilView = xd11_depth_stencil_view(renderer.depthStencilTexture,
                                                        (D3D11_DEPTH_STENCIL_VIEW_DESC)
                                                        {
                                                            DXGI_FORMAT_D24_UNORM_S8_UINT, 
                                                            D3D11_DSV_DIMENSION_TEXTURE2D, 
                                                            .Texture2D = (D3D11_TEX2D_DSV){0}
                                                        });
}

#endif //RENDERER_H
