#define STB_IMAGE_IMPLEMENTATION
#include "w:/libs/stb_image.h"

#include "w:/libs/xlibs/xdirect3d11.h"

LRESULT window_proc(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

typedef struct RendererCore RendererCore;
typedef struct RendererBatch RendererBatch;
typedef struct LineVertex LineVertex;
typedef struct TexturedVertex TexturedVertex;

struct LineVertex
{
    v3f position;
    v4f color;
};

struct TexturedVertex
{
    v3f position;
    v2f uvs;
    v4f color;
};

/* Renderer core variables */
struct RendererCore
{
    ID3D11Texture2D *backBufferTexture;
    ID3D11Texture2D *depthStencilTexture;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11DepthStencilState *depthStencilState;
    XD11TextureAtlas textureAtlas;
};

/* Per batch variables*/
struct RendererBatch
{
    XD11Batch batch;
    ID3DBlob *compiledVS;
    ID3DBlob *compiledPS;
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *inputLayout;
};

global RendererCore core;

RendererBatch lineBatch;
RendererBatch spriteBatch;

XWINMAIN()
{
    /* Initialize Direct3D 11 */
    xd11_initialize((XD11Config)
                    {
                        window_proc,
                        0,
                        0,
                        L"My Window"
                    });
    
    /* Initialize Input */
    xwindow_init((XWindowConfig)
                 {
                     LoadCursor(NULL, IDC_ARROW),
                     xd11.windowHandle
                 });
    
    /* Get back buffer texture from swap chain */    
    core.backBufferTexture = xd11_swapchain_get_buffer();
    
    /* Create depth stencil buffer texture */    
    core.depthStencilTexture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
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
    
    /* Create render target view */
    core.renderTargetView = xd11_render_target_view(core.backBufferTexture);
    
    /* Create depth stencil view */
    core.depthStencilView = xd11_depth_stencil_view(core.depthStencilTexture,
                                                    (D3D11_DEPTH_STENCIL_VIEW_DESC)
                                                    {
                                                        DXGI_FORMAT_D24_UNORM_S8_UINT, 
                                                        D3D11_DSV_DIMENSION_TEXTURE2D, 
                                                        .Texture2D = (D3D11_TEX2D_DSV){0}
                                                    });
    
    /* Create depth stencil state */            
    core.depthStencilState = xd11_depth_stencil_state((D3D11_DEPTH_STENCIL_DESC)
                                                      {
                                                          .DepthEnable = true,
                                                          D3D11_DEPTH_WRITE_MASK_ALL,
                                                          D3D11_COMPARISON_GREATER_EQUAL,
                                                          .StencilEnable = false,
                                                          .StencilReadMask = 0,
                                                          .StencilWriteMask = 0,
                                                          .FrontFace = {0},
                                                          .BackFace = {0},
                                                      });
    
    /* texture atlas texture */
    core.textureAtlas.size = 1024;
    core.textureAtlas.texture.size = (v2i){core.textureAtlas.size, core.textureAtlas.size};
    (core.textureAtlas.texture.bytes = (u8*)xalloc(core.textureAtlas.size*core.textureAtlas.size*4));
    
    core.textureAtlas.texture.handle = xd11_texture2d((D3D11_TEXTURE2D_DESC)
                                                      {
                                                          core.textureAtlas.size, 
                                                          core.textureAtlas.size,
                                                          1, 
                                                          1, 
                                                          DXGI_FORMAT_R8G8B8A8_UNORM,
                                                          {1,0}, 
                                                          D3D11_USAGE_DYNAMIC, 
                                                          D3D11_BIND_SHADER_RESOURCE, 
                                                          D3D11_CPU_ACCESS_WRITE, 
                                                          0
                                                      },
                                                      0);
    
    /* Create line batch */
    {
        /* Vertex shader */
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
        
        /* Pixel shader */
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
        
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
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
        
        /* Compile the shaders */
        lineBatch.vertexShader = xd11_compile_vertex_shader(linesVS, &lineBatch.compiledVS);
        lineBatch.pixelShader = xd11_compile_pixel_shader(linesPS, &lineBatch.compiledPS);
        lineBatch.inputLayout = xd11_input_layout(lineBatch.compiledVS, 
                                                  inputElementDesc, 
                                                  narray(inputElementDesc));
        
        /* Create the XD11Batch */
        lineBatch.batch = (XD11Batch){
            core.backBufferTexture,
            core.renderTargetView,
            core.depthStencilState,
            core.depthStencilView,
            core.depthStencilTexture,
            xd11_blend_state((D3D11_BLEND_DESC)
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
                             }),
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
                                  }),
            xd11_sampler_state((D3D11_SAMPLER_DESC)
                               {
                                   D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_COMPARISON_NEVER,
                                   0,
                                   D3D11_FLOAT32_MAX,
                               }),
            1,                // Vertex buffer count
            &(ID3D11Buffer *) // Vertex buffers
            {
                xd11_buffer((D3D11_BUFFER_DESC)
                            {
                                256*sizeof(LineVertex), 
                                D3D11_USAGE_DYNAMIC, 
                                D3D11_BIND_VERTEX_BUFFER,
                                D3D11_CPU_ACCESS_WRITE, 
                                0, 
                                sizeof(LineVertex)
                            },
                            0),
            },
            &(UINT)           // Vertex Buffer Strides
            {
                sizeof(LineVertex),
            },
            &(UINT)           // Vertex Buffer Offsets
            {
                0
            },
            1,                // Constant buffer count
            &(ID3D11Buffer *) // Constant buffers
            {
                xd11_buffer((D3D11_BUFFER_DESC)
                            {
                                sizeof(mat4f),
                                D3D11_USAGE_DEFAULT, 
                                D3D11_BIND_CONSTANT_BUFFER,
                                0,
                                0,
                                0,
                            },
                            0),
            },
            1,                            // Shader resource view count
            &(ID3D11ShaderResourceView *) // Shader resource views
            {
                xd11_shader_resource_view(core.textureAtlas.texture.handle,
                                          (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                          {
                                              DXGI_FORMAT_R8G8B8A8_UNORM,
                                              D3D_SRV_DIMENSION_TEXTURE2D,
                                              .Texture2D = (D3D11_TEX2D_SRV){.MostDetailedMip=0, .MipLevels=1}
                                          }),
            },
            D3D_PRIMITIVE_TOPOLOGY_LINELIST,
            lineBatch.vertexShader,
            lineBatch.pixelShader,
            lineBatch.inputLayout,
            1, // Viewport count
            &(D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1},
            1, // Scissor rect count
            &(D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y},
        };
    }
    
    /* Create sprite batch */
    {
        /* Vertex shader */
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
        
        
        /* Pixel shader */
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
        
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
                D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            
            { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
                D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            
            { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
                D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        
        /* Compile the shaders */
        spriteBatch.vertexShader = xd11_compile_vertex_shader(spritesVS, &spriteBatch.compiledVS);
        spriteBatch.pixelShader = xd11_compile_pixel_shader(spritesPS, &spriteBatch.compiledPS);
        spriteBatch.inputLayout = xd11_input_layout(spriteBatch.compiledVS, 
                                                    inputElementDesc, 
                                                    narray(inputElementDesc));
        
        /* Create the XD11Batch */
        spriteBatch.batch = (XD11Batch){
            core.backBufferTexture,
            core.renderTargetView,
            core.depthStencilState,
            core.depthStencilView,
            core.depthStencilTexture,
            xd11_blend_state((D3D11_BLEND_DESC)
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
                             }),
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
                                  }),
            xd11_sampler_state((D3D11_SAMPLER_DESC)
                               {
                                   D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_TEXTURE_ADDRESS_WRAP,
                                   D3D11_COMPARISON_NEVER,
                                   0,
                                   D3D11_FLOAT32_MAX,
                               }),
            1,                // Vertex buffer count
            &(ID3D11Buffer *) // Vertex buffers
            {
                xd11_buffer((D3D11_BUFFER_DESC)
                            {
                                256*sizeof(TexturedVertex), 
                                D3D11_USAGE_DYNAMIC, 
                                D3D11_BIND_VERTEX_BUFFER,
                                D3D11_CPU_ACCESS_WRITE, 
                                0, 
                                sizeof(TexturedVertex)
                            },
                            0),
            },
            &(UINT)           // Vertex Buffer Strides
            {
                sizeof(TexturedVertex),
            },
            &(UINT)           // Vertex Buffer Offsets
            {
                0
            },
            1,                // Constant buffer count
            &(ID3D11Buffer *) // Constant buffers
            {
                xd11_buffer((D3D11_BUFFER_DESC)
                            {
                                sizeof(mat4f),
                                D3D11_USAGE_DEFAULT, 
                                D3D11_BIND_CONSTANT_BUFFER,
                                0,
                                0,
                                0,
                            },
                            0),
            },
            1,                            // Shader resource view count
            &(ID3D11ShaderResourceView *) // Shader resource views
            {
                xd11_shader_resource_view(core.textureAtlas.texture.handle,
                                          (D3D11_SHADER_RESOURCE_VIEW_DESC)
                                          {
                                              DXGI_FORMAT_R8G8B8A8_UNORM,
                                              D3D_SRV_DIMENSION_TEXTURE2D,
                                              .Texture2D = (D3D11_TEX2D_SRV){.MostDetailedMip=0, .MipLevels=1}
                                          }),
            },
            D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
            spriteBatch.vertexShader,
            spriteBatch.pixelShader,
            spriteBatch.inputLayout,
            1, // Viewport count
            &(D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1},
            1, // Scissor rect count
            &(D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y},
        };
    }
    
    /* Create resources */
    
    // TODO: Load pngs and update atlas
    v2i whiteDim;
    u8 *whiteBytes = xd11_load_png(L"images/white.png", &whiteDim, false);
    XD11Sprite whiteSprite = xd11_sprite_from_bytes(&core.textureAtlas, whiteBytes, whiteDim);
    
    XD11Sprite arrowSprite = xd11_sprite_from_png(&core.textureAtlas, L"images/arrow128.png", false);
    
    xd11_texture_update(core.textureAtlas.texture, core.textureAtlas.texture.bytes);
    
    while (xd11.running)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        
        /* Do stuff */
        // xline((v2f){0,0}, xrnd.backBufferSize, red4f, 0);
        
        mat4f matrixProjection =
        {
            2.0f / xd11.backBufferSize.x, 0, 0, -1,
            0, -2.0f / xd11.backBufferSize.y, 0, 1.f,
            0, 0, .001f, 0,
            0, 0, 0, 1,
        };
        
        /* Update constant buffers */
        xd11_update_subresource(lineBatch.batch.vsConstantBuffers[0], &matrixProjection);
        xd11_update_subresource(spriteBatch.batch.vsConstantBuffers[0], &matrixProjection);
        
        ID3D11DeviceContext_OMSetRenderTargets(xd11.deviceContext,
                                               1, 
                                               &(ID3D11RenderTargetView *)
                                               {
                                                   core.renderTargetView
                                               },
                                               core.depthStencilView);
        
        ID3D11DeviceContext_ClearRenderTargetView(xd11.deviceContext, core.renderTargetView, eme4f.e);
        ID3D11DeviceContext_ClearDepthStencilView(xd11.deviceContext, core.depthStencilView, 
                                                  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0, 0);
        
        /* Render lines */
        LineVertex data[2] =
        {
            {{0,0,0}, {1,1,1,1}},
            {{xd11.backBufferSize.x,xd11.backBufferSize.y,0}, {1,1,1,1}},
        };
        
        xd11_buffer_update(lineBatch.batch.vertexBuffers[0], data, 2*sizeof(LineVertex));
        xd11_batch_draw(&lineBatch.batch, 2);
        
        /* Render sprites */
        TexturedVertex tdata[6] =
        {
            {{  0,  0,  0}, {arrowSprite.uv.min.x, arrowSprite.uv.min.y}, {1,1,1,1}},
            {{100,  0,  0}, {arrowSprite.uv.max.x, arrowSprite.uv.min.y}, {1,1,1,1}},
            {{100,100,  0}, {arrowSprite.uv.max.x, arrowSprite.uv.max.y}, {1,1,1,1}},
            
            {{100,100,  0}, {arrowSprite.uv.max.x, arrowSprite.uv.max.y}, {1,1,1,1}},
            {{  0,100,  0}, {arrowSprite.uv.min.x, arrowSprite.uv.max.y}, {1,1,1,1}},
            {{  0,  0,  0}, {arrowSprite.uv.min.x, arrowSprite.uv.min.y}, {1,1,1,1}},
        };
        
        xd11_buffer_update(spriteBatch.batch.vertexBuffers[0], tdata, 6*sizeof(TexturedVertex));
        
        xd11_batch_draw(&spriteBatch.batch, 6);
        
        xd11_update();
        xwindow_update(true, xd11.windowSize);
    }
    
    /* Free resources */
    xfree(core.textureAtlas.texture.bytes);
    
    xd11_shutdown();
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
    ID3D11RenderTargetView_Release(core.renderTargetView);
    ID3D11DepthStencilView_Release(core.depthStencilView);
    ID3D11Texture2D_Release(core.backBufferTexture);
    ID3D11Texture2D_Release(core.depthStencilTexture);
    
    /* Resize the swapchain buffers */
    IDXGISwapChain_ResizeBuffers(xd11.swapChain,
                                 2, 
                                 (UINT)xd11.backBufferSize.x,
                                 (UINT)xd11.backBufferSize.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 
                                 0);
    
    /* Get backbuffer texture from swap chain */
    core.backBufferTexture = xd11_swapchain_get_buffer();
    
    /* Create render target view */
    core.renderTargetView = xd11_render_target_view(core.backBufferTexture);
    
    /* Create depth stencil texture */
    core.depthStencilTexture = xd11_texture2d((D3D11_TEXTURE2D_DESC)
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
    core.depthStencilView = xd11_depth_stencil_view(core.depthStencilTexture,
                                                    (D3D11_DEPTH_STENCIL_VIEW_DESC)
                                                    {
                                                        DXGI_FORMAT_D24_UNORM_S8_UINT, 
                                                        D3D11_DSV_DIMENSION_TEXTURE2D, 
                                                        .Texture2D = (D3D11_TEX2D_DSV){0}
                                                    });
    
    
    /* Update viewport and scissor rect*/
    lineBatch.batch.viewports[0] = (D3D11_VIEWPORT){0,0,xd11.backBufferSize.x,xd11.backBufferSize.y, 0, 1};
    lineBatch.batch.scissors[0]  = (D3D11_RECT){0,0,(LONG)xd11.backBufferSize.x,(LONG)xd11.backBufferSize.y};
}