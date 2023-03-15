#ifndef XLIB_D3D11
#define XLIB_D3D11

/* =========================================================================
   DEPENDENCIES
   ========================================================================= */

#include "xbase.h"
#include "xmemory.h"
#include "xmath.h"
#include "xarray.h"
#include "xlist.h"
#include "xtable.h"
#include "xwindows.h"

/* =========================================================================
   DATA TYPES INTERNAL TO THE RENDERER THAT THE USER DOESN'T CARE
   ========================================================================= */


/* =========================================================================
    DIRECT3D 11 LESS VERBOSE AND STRUCTURED TYPES
   ========================================================================= */

#define XD11_RTIO DXGI_RATIONAL
#define XD11_BMPI BITMAPINFO
#define XD11_SDAT D3D11_SUBRESOURCE_DATA

#define XD11_TGBD D3D11_RENDER_TARGET_BLEND_DESC
#define XD11_BLDD D3D11_BLEND_DESC
#define XD11_BUFD D3D11_BUFFER_DESC
#define XD11_RASD D3D11_RASTERIZER_DESC
#define XD11_SMPD D3D11_SAMPLER_DESC
#define XD11_DPSD D3D11_DEPTH_STENCIL_DESC
#define XD11_T2DD D3D11_TEXTURE2D_DESC
#define XD11_BUFD D3D11_BUFFER_DESC
#define XD11_DSVD D3D11_DEPTH_STENCIL_VIEW_DESC
#define XD11_SRVD D3D11_SHADER_RESOURCE_VIEW_DESC

#define xd11_tx2d ID3D11Texture2D *
#define xd11_buff ID3D11Buffer *
#define xd11_tgvw ID3D11RenderTargetView *
#define xd11_dsvw ID3D11DepthStencilView *
#define xd11_srvw ID3D11ShaderResourceView *
#define xd11_dsst ID3D11DepthStencilState *
#define xd11_blst ID3D11BlendState *
#define xd11_rtst ID3D11RasterizerState *
#define xd11_smst ID3D11SamplerState *
#define xd11_vsdr ID3D11VertexShader *
#define xd11_psdr ID3D11PixelShader *
#define xd11_iply ID3D11InputLayout *

/* =========================================================================
   DATA TYPES THAT THE USER CARES ABOUT
   ========================================================================= */

typedef struct
{
    WNDPROC  wndproc;
    u32 wndclass_style;
    u32 wndclass_style_ex;
    wchar_t* window_title;
    v2f window_pos;
    v2f window_size;
    s32 glyph_maker_size;
} XD11Config;

typedef struct
{
    u32 count;
    xd11_buff *array; 
    u32 *strides;
    u32 *offsets;
} XD11VertexBuffers;

typedef struct
{
    xd11_tx2d texture;
    xd11_dsvw view;
    xd11_dsst state;
} XD11DepthStencil;

typedef struct
{
    /* States */
    xd11_blst blend_state; 
    xd11_rtst rasterizer_state;
    xd11_smst sampler_state;
    XD11DepthStencil depth_stencil;
    
    /* Vertex Buffers */
    XD11VertexBuffers vertex_buffers;
    
    /* Shaders */
    xd11_vsdr vertex_shader;
    xd11_psdr pixel_shader;
    xd11_iply input_layout;

    /* Shaders resources */
    Array_T vs_cbuffers;  /* ID3D11Buffer */
    Array_T ps_resources; /* ID3D11ShaderResourceView */
    
    /* D3D11_VIEWPORT */
    Array_T viewports;
    
    /* D3D11_RECT */
    Array_T scissors;
    
    /* Other */
    xd11_tgvw target_view;
    D3D11_PRIMITIVE_TOPOLOGY topology;
} XD11RenderPass;

typedef struct
{
    /* Configuration values */
    s32 max_simul_sprites;
    s32 max_simul_lines;
    s32 glyph_maker_size;
    
    bool topDown;
    DWORD wndclass_style_ex;
    DWORD wndclass_style;
    v2f back_buffer_size;
    v2f window_pos;
    v2f window_size;
    wchar_t window_title[256];
    v4f clear_color;
    
    /* State values */
    bool running;
    f32 dt;
    
    /* Core values */
    HDC glyph_maker_dc;
    HWND window_handle;
    LARGE_INTEGER last_counter;
    IDXGISwapChain *swap_chain;
    ID3D11Device *device;
    ID3D11DeviceContext *device_context;
    ID3D11Debug *debug;
} XD11;

typedef struct
{
    char file_name[512];
    v2i size;
    xd11_tx2d handle;
    xd11_srvw shader_res_view;
} XD11Texture;

/* =========================================================================
   MAIN FUNCTIONS
   ========================================================================= */

void xd11_initialize   (XD11Config config);
void xd11_shutdown     (void);
void xd11_update       (void);
void xd11_resized      (void);

v2f  xd11_monitor_size (void);

/* =========================================================================
    DIRECT3D 11 HELPER FUNCTIONS (Less verbose than D3D11 API)
   ========================================================================= */

XD11_BLDD xd11_blend_desc   (void);
XD11_RASD xd11_raster_desc  (void);
XD11_SMPD xd11_sampler_desc (void);
XD11_TGBD xd11_target_blend_desc  (void);
XD11_DPSD xd11_depth_stencil_desc (void);
XD11_BUFD xd11_const_buffer_desc  (s32 size);
XD11_DSVD xd11_depth_stencil_view_desc (DXGI_FORMAT format, D3D11_DSV_DIMENSION dim);
XD11_SRVD xd11_shader_res_view_desc    (DXGI_FORMAT format, D3D11_SRV_DIMENSION dim);

XD11_RTIO xd11_rational      (u32 numerator, u32 denominator);
XD11_BMPI xd11_bitmap_info   (s32 width, s32 height);
void      xd11_update_subres (void *resource, void *data);

void      xd11_set_target  (xd11_tgvw target_view, xd11_dsvw ds_view);
void      xd11_set_targets (u32 count, xd11_tgvw *target_views, xd11_dsvw ds_view);

xd11_tx2d xd11_swap_chain_buffer (void);

xd11_buff xd11_buffer    (XD11_BUFD desc, XD11_SDAT *data);
xd11_tx2d xd11_texture2d (XD11_T2DD desc, XD11_SDAT *data);

xd11_tgvw xd11_target_view        (xd11_tx2d texture);
xd11_dsvw xd11_depth_stencil_view (xd11_tx2d ds_texture, XD11_DSVD ds_desc);
xd11_srvw xd11_shader_res_view    (xd11_tx2d texture, XD11_SRVD desc);

void      xd11_clear_target_view        (xd11_tgvw target_view, v4f clear_color);
void      xd11_clear_depth_stencil_view (xd11_dsvw ds_view, u32 flags, UINT8 min, UINT8 max);

xd11_dsst xd11_depth_stencil_state (XD11_DPSD desc);
xd11_blst xd11_blend_state         (XD11_BLDD desc);
xd11_rtst xd11_rasterizer_state    (XD11_RASD desc);
xd11_smst xd11_sampler_state       (XD11_SMPD desc);

void      xd11_texture2d_update (XD11Texture texture, u8 *bytes);
v2f       xd11_monitor_size (void);

xd11_vsdr xd11_compile_vertex_shader (char *source, ID3DBlob **compiledVS);
xd11_psdr xd11_compile_pixel_shader  (char *source, ID3DBlob **compiledPS);

xd11_iply xd11_input_layout  (ID3DBlob *vs, D3D11_INPUT_ELEMENT_DESC *a, u32 c);
void      xd11_buffer_update (ID3D11Buffer *buffer, void *data, u32 size);
void      xd11_render_pass   (XD11RenderPass *pass, u32 vertex_count);
void      xd11_swap_chain_resize (void);

/* =========================================================================
   End of Interface
   ========================================================================= */







/* Intended white space */







/* =========================================================================
   Implementation
   ========================================================================= */

XD11 xd11;

#define RGBA(r,g,b,a) (((a)<<24) | ((r)<<16) | ((g)<<8) | (b))


XD11_TGBD xd11_target_blend_desc()
{
    XD11_TGBD r = {
        true,
        D3D11_BLEND_SRC_ALPHA,
        D3D11_BLEND_INV_SRC_ALPHA,
        D3D11_BLEND_OP_ADD,
        D3D11_BLEND_ONE,
        D3D11_BLEND_ZERO,
        D3D11_BLEND_OP_ADD,
        D3D11_COLOR_WRITE_ENABLE_ALL,
    };
    return r;
}

XD11_BLDD xd11_blend_desc()
{
    XD11_BLDD r = {
        false,
        false,
    };
    
    r.RenderTarget[0] = xd11_target_blend_desc();
    
    return r;   
}

XD11_RASD xd11_raster_state(void)
{
    XD11_RASD r =
    {
        D3D11_FILL_SOLID, D3D11_CULL_BACK, false,
        0, 0, 0, true, true, false, false,
    };
    return r;
}

XD11_SMPD xd11_sampler_desc(void)
{
    XD11_SMPD r =
    {
        D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_TEXTURE_ADDRESS_WRAP,
        D3D11_COMPARISON_NEVER,
        0,
        D3D11_FLOAT32_MAX,
    };
    return r;
}

DXGI_RATIONAL xd11_rational(u32 numerator, u32 denominator)
{
	DXGI_RATIONAL r = {
        numerator, denominator
    };
	return r;
}

D3D11_BUFFER_DESC xd11_const_buffer_desc(s32 size)
{
    D3D11_BUFFER_DESC r = {
        size, D3D11_USAGE_DEFAULT, 
        D3D11_BIND_CONSTANT_BUFFER,
        0, 0, 0,
    };
    return r;
}

BITMAPINFO xd11_bitmap_info(int width, int height)
{
    BITMAPINFOHEADER h = {
        sizeof(h), width, height, 1, 32
    };
    
    BITMAPINFO r = {
        h
    };
    
    return r;
}

XD11_DPSD xd11_depth_stencil_desc(void)
{
    XD11_DPSD r = 
    {
        .DepthEnable = true,
        D3D11_DEPTH_WRITE_MASK_ALL,
        D3D11_COMPARISON_GREATER_EQUAL,
        .StencilEnable = false,
        .StencilReadMask = 0,
        .StencilWriteMask = 0,
        .FrontFace = {0},
        .BackFace = {0},
    };
    return r;
}

XD11_DSVD xd11_depth_stencil_view_desc(DXGI_FORMAT format, D3D11_DSV_DIMENSION dim)
{
    D3D11_TEX2D_DSV t = {
        0
    };
    
    XD11_DSVD r = {
        format, dim,
    };
    
    if (dim == D3D11_DSV_DIMENSION_TEXTURE2D) {
        r.Texture2D = t;
    } 
    
    return r;
}

XD11_SRVD xd11_shader_res_view_desc(DXGI_FORMAT format, D3D11_SRV_DIMENSION dim)
{
    XD11_SRVD r =
    {
        format,
        dim,
    };
    
    if (dim == D3D_SRV_DIMENSION_TEXTURE2D) {
        r.Texture2D.MostDetailedMip = 0;
        r.Texture2D.MipLevels = 1;
    }
    
    return r;
}

void xd11_set_render_target(xd11_tgvw view, xd11_dsvw ds_view)
{
    xd11_tgvw views[1] = 
    {
        view
    };
    
    ID3D11DeviceContext_OMSetRenderTargets(xd11.device_context,
                                           1,
                                           views,
                                           ds_view);
}

void xd11_set_targets(u32 count, xd11_tgvw *target_views, xd11_dsvw ds_view)
{
    ID3D11DeviceContext_OMSetRenderTargets(xd11.device_context,
                                           count,
                                           target_views,
                                           ds_view);
}

void xd11_update_subres(void *resource, void *data)
{
    ID3D11DeviceContext_UpdateSubresource(xd11.device_context, 
                                          (ID3D11Resource *)resource, 
                                          0, 
                                          NULL, 
                                          data, 
                                          0, 
                                          0);
}

xd11_tx2d xd11_swapchain_get_buffer(void)
{
    xd11_tx2d result = 0;
    if (FAILED(IDXGISwapChain_GetBuffer(xd11.swap_chain, 0, &IID_ID3D11Texture2D, (void**)&result)))
        assert(!"Can't fail");
    return result;
}

xd11_buff xd11_buffer(D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA *data)
{
    xd11_buff result = 0;
    if (FAILED(ID3D11Device_CreateBuffer(xd11.device, &desc, 0, &result)))
        assert(!"Can't fail");
    return result;
}

xd11_tx2d xd11_texture2d(XD11_T2DD desc, XD11_SDAT *data)
{
    xd11_tx2d result = 0;
    if (FAILED(ID3D11Device_CreateTexture2D(xd11.device, &desc, data, &result)))
        assert(!"Can't fail");
    return result;
}

xd11_tgvw xd11_target_view(xd11_tx2d target_texture)
{
    xd11_tgvw result = 0;
    if (FAILED(ID3D11Device_CreateRenderTargetView(xd11.device, 
                                                   (ID3D11Resource* )target_texture, 
                                                   0, &result)))
        assert(!"Can't fail");
    return result;
}

xd11_dsvw xd11_depth_stencil_view(xd11_tx2d texture, XD11_DSVD desc)
{
    xd11_dsvw result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilView(xd11.device, 
                                                   (ID3D11Resource *)texture, 
                                                   &desc,
                                                   &result)))
        assert(!"Can't fail");
    return result;
}

xd11_srvw xd11_shader_res_view(xd11_tx2d texture, XD11_SRVD desc)
{
    xd11_srvw result = 0;
    if (FAILED(ID3D11Device_CreateShaderResourceView(xd11.device, 
                                                     (ID3D11Resource*)texture, 
                                                     &desc,
                                                     &result)))
        assert(!"Can't fail");
    
    return result;
}

ID3D11DepthStencilState *xd11_depth_stencil_state(D3D11_DEPTH_STENCIL_DESC desc)
{
    ID3D11DepthStencilState *result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilState(xd11.device, 
                                                    &desc, 
                                                    &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11BlendState *xd11_blend_state(D3D11_BLEND_DESC desc)
{
    ID3D11BlendState *result = 0;
    if (FAILED(ID3D11Device_CreateBlendState(xd11.device, 
                                             &desc, 
                                             &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11RasterizerState *xd11_rasterizer_state(D3D11_RASTERIZER_DESC desc)
{
    ID3D11RasterizerState *result = 0;
    if (FAILED(ID3D11Device_CreateRasterizerState(xd11.device,
                                                  &desc,
                                                  &result)))
        assert(!"Can't fail");
    return result;
}

ID3D11SamplerState *xd11_sampler_state(D3D11_SAMPLER_DESC desc)
{
    ID3D11SamplerState *result = 0;
    if (FAILED(ID3D11Device_CreateSamplerState(xd11.device, 
                                               &desc, 
                                               &result)))
        assert(!"Can't fail");
    return result;
}

void xd11_texture2d_update(XD11Texture texture, u8 *bytes)
{
    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;
    ID3D11DeviceContext_Map(xd11.device_context, (ID3D11Resource *)texture.handle, 
                            0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_sub_resource);
    
    memcpy(mapped_sub_resource.pData, bytes, texture.size.x*texture.size.y*4);
    
    ID3D11DeviceContext_Unmap(xd11.device_context, (ID3D11Resource *)texture.handle, 0);
}

v2f xd11_monitor_size(void)
{
    v2f r;
    
    r=ini2fs(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    
    return r;
}

ID3D11VertexShader *xd11_compile_vertex_shader(char *source, ID3DBlob **compiledVS)
{
    ID3D11VertexShader *result = 0;
    
    // Compile the shader
    ID3DBlob *errorVertexMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "vs_main", "vs_5_0",
                          0, 0, compiledVS, &errorVertexMessages)))
    {
        if (errorVertexMessages)
        {
            wchar_t *msg = (wchar_t *)
            (ID3D10Blob_GetBufferPointer(errorVertexMessages));
            OutputDebugStringW(msg);
            assert(!"Failed to compile shader");
        }
        else
        {
            assert(!"Failed to compile shader");
        }
    }
    
    // Create the vertex shader if compilation went ok
    ID3D11Device_CreateVertexShader(xd11.device,
                                    ID3D10Blob_GetBufferPointer(*compiledVS),
                                    ID3D10Blob_GetBufferSize(*compiledVS),
                                    NULL, &result);
    
    return result;
}

ID3D11PixelShader *xd11_compile_pixel_shader(char *source, ID3DBlob **compiledPS)
{
    ID3D11PixelShader *result = 0;
    
    // And compilation of the pixel shader
    ID3DBlob *errorPixelMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "ps_main", "ps_5_0",
                          0, 0, compiledPS, &errorPixelMessages)))
    {
        if (errorPixelMessages)
        {
            char *msg = (char *)
            (ID3D10Blob_GetBufferPointer(errorPixelMessages));
            OutputDebugStringA(msg);
            assert(!"Failed to compile shader");
        }
        else
        {
            assert(!"Failed to compile shader");
        }
    }
    
    // Create the pixel shader if compilation went ok
    ID3D11Device_CreatePixelShader(xd11.device,
                                   ID3D10Blob_GetBufferPointer(*compiledPS),
                                   ID3D10Blob_GetBufferSize(*compiledPS),
                                   NULL, &result);
    
    return result;
}

xd11_iply xd11_input_layout(ID3DBlob *vs, D3D11_INPUT_ELEMENT_DESC *array, u32 count)
{    
    xd11_iply result = 0;
    // And create the input layout
    void *vsPointer = ID3D10Blob_GetBufferPointer(vs);
    u32 vsSize = (u32)ID3D10Blob_GetBufferSize(vs);
    
    if (FAILED(ID3D11Device_CreateInputLayout(xd11.device, array, count,
                                              vsPointer, vsSize, &result)))
        exit(1);
    
    return result;
}

void xd11_buffer_update(xd11_buff buffer, void *data, u32 size)
{
    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;
    ID3D11DeviceContext_Map(xd11.device_context, 
                            (ID3D11Resource *)buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_sub_resource);
    
    memcpy(mapped_sub_resource.pData, data, size); 
    
    ID3D11DeviceContext_Unmap(xd11.device_context, (ID3D11Resource *)buffer, 0);
}

void xd11_clear_rtv(ID3D11RenderTargetView *render_target_view, v4f clear_color)
{
    ID3D11DeviceContext_ClearRenderTargetView(xd11.device_context, 
                                              render_target_view, 
                                              clear_color.e);
}

void xd11_clear_dsv(xd11_dsvw ds_view, u32 flags, UINT8 min, UINT8 max)
{
    ID3D11DeviceContext_ClearDepthStencilView(xd11.device_context, 
                                              ds_view, 
                                              flags, min, max);
}

void xd11_render_pass(XD11RenderPass *pass, u32 vertex_count)
{
    ID3D11DeviceContext_IASetPrimitiveTopology(xd11.device_context, pass->topology);
    
    ID3D11DeviceContext_IASetInputLayout(xd11.device_context, pass->input_layout);
    
    /*
    ID3D11Buffer *vbs[1] = { pass->vertexBuffer };
    UINT vbss[1] = { pass->vertexBufferStride };
    UINT vbos[1] = { 0 };
    D3D11_VIEWPORT vps[1] =  { { pass->viewport.min.x, pass->viewport.min.y, pass->viewport.max.x, pass->viewport.max.y, 0.0f, 1.0f } };
    D3D11_RECT scs[1] = { { 0, 0, (LONG)xd11.backBufferSize.x, (LONG)xd11.backBufferSize.y } };

    */
    
    if (pass->vertex_buffers.count > 0)
    {
        ID3D11DeviceContext_IASetVertexBuffers(xd11.device_context, 0, pass->vertex_buffers.count, pass->vertex_buffers.array, 
                                               pass->vertex_buffers.strides, pass->vertex_buffers.offsets);
    }
    
    ID3D11DeviceContext_VSSetShader(xd11.device_context, pass->vertex_shader, NULL, 0);
    
    if (pass->vs_cbuffers.top > 0)
    {
        s32 count;
        ID3D11Buffer *array;
        Array_toarray(pass->vs_cbuffers, &array, &count);
        
        ID3D11DeviceContext_VSSetConstantBuffers(xd11.device_context, 0, 
                                                 count, &array);
        
        xfree(array);
    }
    
    ID3D11DeviceContext_PSSetShader(xd11.device_context, pass->pixel_shader, NULL, 0);
    ID3D11DeviceContext_PSSetSamplers(xd11.device_context, 0, 1, &pass->sampler_state);
    
    if (pass->ps_resources.top > 0)
    {
        s32 count;
        ID3D11ShaderResourceView *array;
        Array_toarray(pass->ps_resources, &array, &count);
        ID3D11DeviceContext_PSSetShaderResources(xd11.device_context, 0, 
                                                 count, &array);
        xfree(array);
    }
    
    {
        s32 count;
        D3D11_VIEWPORT *array;
        Array_toarray(pass->viewports, &array, &count);
        ID3D11DeviceContext_RSSetViewports(xd11.device_context, 
                                           count, array);
        xfree(array);
    }
    
    {
        s32 count;
        D3D11_RECT *array;
        Array_toarray(pass->scissors, &array, &count);
        ID3D11DeviceContext_RSSetScissorRects(xd11.device_context, 
                                              count, array);
    }
    
    ID3D11DeviceContext_RSSetState(xd11.device_context, pass->rasterizer_state);
    ID3D11DeviceContext_OMSetDepthStencilState(xd11.device_context, 
                                               pass->depth_stencil.state, 0);
    
    ID3D11DeviceContext_OMSetBlendState(xd11.device_context, 
                                        pass->blend_state, NULL, 
                                        0xffffffff);
    ID3D11DeviceContext_Draw(xd11.device_context, vertex_count, 0);
}

void xd11_swap_chain_resize()
{
    RECT rect;
    if (!GetClientRect(xd11.window_handle, &rect)) {
        exit(1);
    }
    
    v2f back_buffer_size = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)back_buffer_size.x != 0 && (UINT)back_buffer_size.y != 0) &&
        (((UINT)back_buffer_size.x != xd11.back_buffer_size.x) || 
         ((UINT)back_buffer_size.y != xd11.back_buffer_size.y)))
    {
        xd11.back_buffer_size = back_buffer_size;
        
        xd11_resized();
    }
}

void xd11_initialize(XD11Config config)
{
    /* Create a DC to d11 glyphs with  */
    xd11.glyph_maker_dc = CreateCompatibleDC(GetDC(0));
    
    /* Register a window class */
    WNDCLASSEXW windowClass = xwin_wndclass(config.wndproc);
    if (RegisterClassExW(&windowClass) == 0) exit(1);
    
    /* Window style based on config */
    xd11.wndclass_style_ex =  config.wndclass_style_ex;
    xd11.wndclass_style = (config.wndclass_style == 0) ? 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE : config.wndclass_style;
    
    /* Window pos and size */
    xd11.window_pos = (config.window_pos.x==0 && config.window_pos.y==0) ? 
        xd11.window_pos = (v2f){0,0} : config.window_pos;
    
    xd11.window_size = (config.window_size.x==0 && config.window_size.y==0) ? 
        xd11.window_size = (v2f){800,600} : config.window_size;
    
    xd11.back_buffer_size = xd11.window_size;
    
    RECT size = 
    { 
        (u32)xd11.window_pos.x, (u32)xd11.window_pos.y,
        (u32)(xd11.window_pos.x + xd11.window_size.x), (u32)(xd11.window_pos.y + xd11.window_size.y)
    };
    
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    xd11.window_size = (v2f){(f32)(size.right - size.left), (f32)(size.bottom - size.top)};
    
    /* Window title */
    xstrcpy(xd11.window_title, 256, (config.window_title == 0) ? 
            L"XLib's xd11er" : config.window_title);
    
    /* More configuration */
    xd11.glyph_maker_size  = (config.glyph_maker_size  == 0) ? xd11.glyph_maker_size = 256   : config.glyph_maker_size;
    
    
    /* Create the window */
    xd11.window_handle = CreateWindowExW(xd11.wndclass_style_ex, L"xwindow_class", 
                                         xd11.window_title, xd11.wndclass_style,
                                         (s32)xd11.window_pos.x, (s32)xd11.window_pos.y, 
                                         (s32)xd11.window_size.x, (s32)xd11.window_size.y, NULL, NULL, GetModuleHandle(0), NULL);
    
    if (xd11.window_handle == NULL)
        exit(1);
    
    /* Request notification when the mouse leaves the non-client area */
    TRACKMOUSEEVENT trackMouseEvent;
    trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    trackMouseEvent.dwFlags = TME_NONCLIENT | TME_LEAVE;
    trackMouseEvent.hwndTrack = xd11.window_handle;
    TrackMouseEvent(&trackMouseEvent);
    
    /* Direct3D 11 backbuffer size, refresh rate, scale, swap chain, feature levels...  */
    DXGI_MODE_DESC mode_desc = 
    {
        (s32)xd11.back_buffer_size.x, (s32)xd11.back_buffer_size.y,
        xd11_rational(60,1), DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_CENTERED
    };
    
    DXGI_SWAP_CHAIN_DESC swap_chain_desc =
    {
        mode_desc, (DXGI_SAMPLE_DESC){1, 0}, 
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2, xd11.window_handle, true, 
        DXGI_SWAP_EFFECT_FLIP_DISCARD, 0
    };
    
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                             D3D11_CREATE_DEVICE_DEBUG, 
                                             feature_levels, narray(feature_levels), 
                                             D3D11_SDK_VERSION, &swap_chain_desc, &xd11.swap_chain,
                                             &xd11.device, NULL, &xd11.device_context)))
        exit(1);
    
    /* Direct3D 11 Debug interface */
    if (FAILED(ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11Debug, 
                                           (void**)&xd11.debug)))
        exit(1);
    
    ID3D11InfoQueue* infoQueue;
    ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11InfoQueue, (void**)&infoQueue);
    ID3D11InfoQueue_SetBreakOnSeverity(infoQueue, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(infoQueue);
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    xd11.running = true;
}

void xd11_update(void)
{
    // Measure fps
    LARGE_INTEGER counter = xwin_time();
    
    // Calculate dt
    xd11.dt = xwin_seconds(xd11.last_counter, counter);
    if (xd11.dt > 1000)
        xd11.dt = 0;
    
    // Save counter
    xd11.last_counter = counter;
    
    xd11_swap_chain_resize();
    
    IDXGISwapChain_Present(xd11.swap_chain, 1, 0);
}

void xd11_shutdown(void)
{
    xmemcheck();
    
    if (xd11.debug) 
        ID3D11Debug_Release(xd11.debug);
    
    if (xd11.swap_chain) 
        IDXGISwapChain_Release(xd11.swap_chain);
    
    if (xd11.device_context) 
        ID3D11DeviceContext_Release(xd11.device_context);
    
    if (xd11.device) 
        ID3D11Device_Release(xd11.device);
}

#endif