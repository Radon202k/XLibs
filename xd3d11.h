#ifndef XLIB_D3D11
#define XLIB_D3D11

/* =========================================================================
   DEPENDENCIES
   ========================================================================= 

Make sure to include these somewhere before this header:

#include "xbase.h"
#include "xmemory.h"
#include "xmath.h"
#include "xarray.h"
#include "xlist.h"
#include "xtable.h"
#include "xwindows.h"

/* =========================================================================
   DATA TYPES THAT THE USER CARES ABOUT
   ========================================================================= */

typedef struct
{
    WNDPROC wndproc;
    HICON icon;
    u32 wcStyle;
    u32 wcStyleEx;
    char wndTitle[512];
    v2 wndP;
    v2 wndDim;
} XD11Config;

typedef struct
{
    u32 count;
    ID3D11Buffer * *array; 
    u32 *strides;
    u32 *offsets;
} XD11VertexBuffers;

typedef struct
{
    ID3D11Texture2D * texture;
    ID3D11DepthStencilView * view;
    ID3D11DepthStencilState * state;
} XD11DepthStencil;

typedef struct
{
    /* States */
    ID3D11BlendState *blendState; 
    ID3D11RasterizerState *rasterizerState;
    ID3D11SamplerState *samplerState;
    XD11DepthStencil depthStencil;
    
    /* Vertex Buffers */
    XD11VertexBuffers vBuffers;
    
    /* Shaders */
    ID3D11VertexShader *vShader;
    ID3D11PixelShader *pShader;
    ID3D11InputLayout *inputLayout;
    
    /* Shaders resources */
    u32 vShaderCBufferCount;
    ID3D11Buffer **vShaderCBuffers;
    
    /* D3D11_VIEWPORT */
    Array_T viewports;
    
    /* Other */
    ID3D11RenderTargetView *targetView;
    D3D11_PRIMITIVE_TOPOLOGY topology;
} XD11RenderPass;

typedef struct
{
    /* Configuration values */
    s32 maxSpriteCount;
    s32 maxLineCount;
    
    DWORD wcStyleEx;
    DWORD wcStyle;
    v2 bbDim;
    v2 wndP;
    v2 wndDim;
    char wndTitle[256];
    v4 clearColor;
    
    /* State values */
    bool running;
    f32 dt;
    
    /* Core values */
    HWND wndHandle;
    LARGE_INTEGER lastCounter;
    IDXGISwapChain *swapChain;
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
#ifndef XD11_RELEASE_BUILD
    ID3D11Debug *debug;
#endif
    FILE *logFile;
} XD11;

typedef struct
{
    char file_name[512];
    v2i dim;
    ID3D11Texture2D * handle;
    ID3D11ShaderResourceView * view;
} XD11Texture;

typedef struct {
    v2i dim;
    u32 at;
    XD11Texture tex;
    u32 mipLevels;
} XD11TextureArray;


/* =========================================================================
   MAIN FUNCTIONS
   ========================================================================= */

void xd11_initialize   (XD11Config config);
void xd11_shutdown     (void);
void xd11_update       (void);
void xd11_resized      (void);

void xd11_monitor_size (v2 dest);

void xd11_log          (char *message);

/* =========================================================================
    DIRECT3D 11 DEFAULT  (Less verbose than D3D11 API)
   ========================================================================= */

/* NOTE: To make this interface somewhat readable we'll have to define temp 
;        symbols to refer to the incredibly long names of Direct3d 11 api. 
;        Because it is extremely confusing to rename the entire api, this is
;        not repeated anywhere else, it is merely used here to line up the
;        statics and make it easy to see relations between them. 
   ;
; IMPORTANT: Make sure to realize that the symbols are undefined and defined
;            again constantly so they really dont mean anything, e.g., XBD
;            is first defined as D3D11_BLEND_DESC to describe the first few
;            statics of the interface, and right after it is defined as
;            D3D11_BUFFER_DESC to describe the next set of statics. */


/* =========================================================================
    Blend, Rasterizer and Sampler states
   ========================================================================= */
#define XBD   D3D11_BLEND_DESC
#define XSD   D3D11_SAMPLER_DESC
#define XRD   D3D11_RASTERIZER_DESC
#define XDSD  D3D11_DEPTH_STENCIL_DESC
#define XBSP  ID3D11BlendState *
#define XSSP  ID3D11SamplerState *
#define XRSP  ID3D11RasterizerState *
#define XDSSP ID3D11DepthStencilState *
/* ====================================================================== */
XBD   xd11_blend_desc          (void);
XSD   xd11_sampler_desc        (void);
XRD   xd11_raster_desc         (void);
XDSD  xd11_depth_stencil_desc  (void);
XBSP  xd11_blend_state         (XBD  desc);
XSSP  xd11_sampler_state       (XSD  desc);
XRSP  xd11_rasterizer_state    (XRD  desc);
XDSSP xd11_depth_stencil_state (XDSD desc);

/* ====================================================================== */
#undef XBD
#undef XRD
#undef XSD
#undef XRSP
#undef XSSP
/* ====================================================================== */
#define XB    ID3DBlob *
#define XR    DXGI_RATIONAL
#define XF    DXGI_FORMAT
#define XTP   ID3D11Texture2D *
#define XBP   ID3D11Buffer *
#define XTD   D3D11_TEXTURE2D_DESC
#define XDD   D3D11_DSV_DIMENSION
#define XSD   D3D11_SRV_DIMENSION
#define XBD   D3D11_BUFFER_DESC
#define XRTBD D3D11_RENDER_TARGET_BLEND_DESC
#define XDSVD D3D11_DEPTH_STENCIL_VIEW_DESC
#define XSRVD D3D11_SHADER_RESOURCE_VIEW_DESC
#define XRTVP ID3D11RenderTargetView *
#define XDSVP ID3D11DepthStencilView *
#define XSRDP D3D11_SUBRESOURCE_DATA *
#define XSRVP ID3D11ShaderResourceView *
#define XVSP  ID3D11VertexShader *
#define XPSP  ID3D11PixelShader *
#define XILP  ID3D11InputLayout *
#define XIED  D3D11_INPUT_ELEMENT_DESC

XRTBD xd11_target_blend_desc       (void);
XTP   xd11_swapchain_buffer        (void);
void  xd11_swapchain_resize        (void); 
XBD   xd11_const_buffer_desc       (s32 size);
XRTVP xd11_target_view             (XTP texture);
XDSVD xd11_depth_stencil_view_desc (XF format, XDD dim);
XSRVD xd11_shader_res_view_desc    (XF format, XSD dim);
XBP   xd11_buffer                  (XBD desc, XSRDP data);
XTP   xd11_texture2d               (XTD desc, XSRDP data);
XDSVP xd11_depth_stencil_view      (XTP dsTexture, XDSVD dsDesc);
XSRVP xd11_shader_res_view         (XTP texture, XSRVD desc);
void  xd11_clear_targetView        (XRTVP targetView, v4 clearColor);
void  xd11_set_target              (XRTVP view, XDSVP dsView);
void  xd11_set_targets             (XRTVP *views, XDSVP dsView, u32 count);
void  xd11_clear_depthStencil_view (XDSVP dsView, u32 flags, UINT8 min, UINT8 max);
XVSP  xd11_compile_vshader         (char *source, XB *compiledVS);
XPSP  xd11_compile_pshader         (char *source, XB *compiledPS);
XILP  xd11_input_layout            (XB vs, XIED *a, u32 c);

#undef XB
#undef XR
#undef XF
#undef XTP
#undef XBP
#undef XTD
#undef XDD
#undef XSD
#undef XBD
#undef XDSD
#undef XBSP
#undef XRTBD
#undef XDSVD
#undef XSRVD
#undef XRTVP
#undef XDSVP
#undef XSRDP
#undef XSRVP 
#undef XDSSP
#undef XVSP
#undef XPSP
#undef XILP
#undef XIED

/* =========================================================================
   End of Interface
   ========================================================================= */







/* Intended white space */







/* =========================================================================
   Implementation
   ========================================================================= */

global XD11 xd11;
global HCURSOR cursor_select;
global HCURSOR cursor_link;
global HCURSOR cursor_horizontal_resize;
global HCURSOR cursor_palm;
global HCURSOR cursor_drag;

static Rect3 xd11_texarray_put(XD11TextureArray *a, u8 *b, v2i dim) {
    Rect3 uvs = {0};
    
    /* Calculate uvs */
    uvs = rect3_min_max((v3){0, 0, (f32)a->at},
                        (v3){dim[0]/(f32)a->dim[0], dim[1]/(f32)a->dim[1], (f32)a->at});
    
    /* Update texture */
    D3D11_SUBRESOURCE_DATA updateData = {
        .pSysMem = b,
        .SysMemPitch = dim[0]*4,
        .SysMemSlicePitch = 0
            
    };
    D3D11_BOX destRegion = {
        .left = 0,
        .top = 0,
        .front = 0,
        .right = dim[0],
        .bottom = dim[1],
        .back = 1,
    };
    UINT mipLevel = 0;
    UINT arraySlice = a->at;
    UINT destSubresource = mipLevel + arraySlice * a->mipLevels;
    ID3D11DeviceContext_UpdateSubresource(xd11.deviceContext, 
                                          (ID3D11Resource *)a->tex.handle,
                                          destSubresource,
                                          &destRegion,
                                          updateData.pSysMem,
                                          updateData.SysMemPitch,
                                          updateData.SysMemSlicePitch);
    
    a->at++;
    
    return uvs;
}




void xd11_generate_mips(XD11Texture *texture) {
    // Generate mip maps
    ID3D11DeviceContext_GenerateMips(xd11.deviceContext, texture->view);
}

void xd11_log(char *message) {
    /* Save message to the log file */
    if (xd11.logFile != NULL)
        fprintf(xd11.logFile, "%s\n", message);
}

void xd11_fatal(char *message) {
    // Output the error message to the console or log file
    xd11_log(message);
    
    /* Close the log file */
    fclose(xd11.logFile);
    
    assert(!"Fatal error. Check xd11.log for error messages.");
}

#define RGBA(r,g,b,a) (((a)<<24) | ((r)<<16) | ((g)<<8) | (b))

D3D11_RENDER_TARGET_BLEND_DESC xd11_target_blend_desc() {
    D3D11_RENDER_TARGET_BLEND_DESC r = {
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

D3D11_BLEND_DESC xd11_blend_desc() {
    D3D11_BLEND_DESC r = {
        false,
        false,
    };
    
    r.RenderTarget[0] = xd11_target_blend_desc();
    
    return r;   
}

D3D11_RASTERIZER_DESC xd11_raster_state(void) {
    D3D11_RASTERIZER_DESC r = {
        D3D11_FILL_SOLID, D3D11_CULL_BACK, false,
        0, 0, 0, true, true, false, false,
    };
    return r;
}

D3D11_SAMPLER_DESC xd11_sampler_desc(void) {
    D3D11_SAMPLER_DESC r = {
        .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
        .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
        .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        .MipLODBias = 0,
        .MaxAnisotropy = 0,
        .ComparisonFunc = D3D11_COMPARISON_NEVER,
        .BorderColor = {0,0,0,0},
        .MinLOD = 0,
        .MaxLOD = D3D11_FLOAT32_MAX,
    };
    return r;
}

D3D11_BUFFER_DESC xd11_const_buffer_desc(s32 size) {
    D3D11_BUFFER_DESC r = {
        size, D3D11_USAGE_DEFAULT, 
        D3D11_BIND_CONSTANT_BUFFER,
        0, 0, 0,
    };
    return r;
}

D3D11_DEPTH_STENCIL_DESC xd11_depthStencil_desc(void) {
    D3D11_DEPTH_STENCIL_DESC r = {
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

D3D11_DEPTH_STENCIL_VIEW_DESC
xd11_depth_stencil_view_desc(DXGI_FORMAT format, D3D11_DSV_DIMENSION dim) {
    D3D11_TEX2D_DSV t = {
        0
    };
    
    D3D11_DEPTH_STENCIL_VIEW_DESC r = {
        format, dim,
    };
    
    if (dim == D3D11_DSV_DIMENSION_TEXTURE2D) {
        r.Texture2D = t;
    } 
    
    return r;
}

D3D11_SHADER_RESOURCE_VIEW_DESC
xd11_shader_res_view_desc(DXGI_FORMAT format, D3D11_SRV_DIMENSION dim) {
    D3D11_SHADER_RESOURCE_VIEW_DESC r = {
        format,
        dim,
    };
    
    if (dim == D3D_SRV_DIMENSION_TEXTURE2D) {
        r.Texture2D.MostDetailedMip = 0;
        r.Texture2D.MipLevels = UINT_MAX; // Use all available mipmap levels
    }
    
    return r;
}

void xd11_update_subres(void *resource, void *data) {
    ID3D11DeviceContext_UpdateSubresource(xd11.deviceContext, 
                                          (ID3D11Resource *)resource, 
                                          0, 
                                          NULL, 
                                          data, 
                                          0, 
                                          0);
}

void xd11_set_render_target(ID3D11RenderTargetView *view, ID3D11DepthStencilView *dsView) {
    ID3D11RenderTargetView *views[1] = {
        view
    };
    
    ID3D11DeviceContext_OMSetRenderTargets(xd11.deviceContext,
                                           1,
                                           views,
                                           dsView);
}

void xd11_set_targets(ID3D11RenderTargetView **targetViews, ID3D11DepthStencilView *dsView,
                      u32 count) {
    ID3D11DeviceContext_OMSetRenderTargets(xd11.deviceContext,
                                           count,
                                           targetViews,
                                           dsView);
}

ID3D11Texture2D *xd11_swapchain_get_buffer(void) {
    ID3D11Texture2D * result = 0;
    if (FAILED(IDXGISwapChain_GetBuffer(xd11.swapChain, 0, &IID_ID3D11Texture2D, (void**)&result)))
        xd11_fatal("Failed to get buffer from swapchain");
    
    return result;
}

ID3D11Buffer *
xd11_buffer(D3D11_BUFFER_DESC desc, D3D11_SUBRESOURCE_DATA *data) {
    ID3D11Buffer *result = 0;
    
    if (FAILED(ID3D11Device_CreateBuffer(xd11.device, &desc, 0, &result)))
        xd11_fatal("Failed to create buffer");
    
    return result;
}

void xd11_buffer_update(ID3D11Buffer *buffer, void *data, u32 size) {
    D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;
    ID3D11DeviceContext_Map(xd11.deviceContext, 
                            (ID3D11Resource *)buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_sub_resource);
    
    memcpy(mapped_sub_resource.pData, data, size); 
    
    ID3D11DeviceContext_Unmap(xd11.deviceContext, (ID3D11Resource *)buffer, 0);
}

ID3D11Texture2D *
xd11_texture2d(D3D11_TEXTURE2D_DESC desc, D3D11_SUBRESOURCE_DATA *data) {
    ID3D11Texture2D * result = 0;
    if (FAILED(ID3D11Device_CreateTexture2D(xd11.device, &desc, data, &result)))
        xd11_fatal("Failed to create texture2d");
    return result;
}

ID3D11RenderTargetView *
xd11_target_view(ID3D11Texture2D * target_texture) {
    ID3D11RenderTargetView *result = 0;
    if (FAILED(ID3D11Device_CreateRenderTargetView(xd11.device, 
                                                   (ID3D11Resource* )target_texture, 
                                                   0, &result)))
        xd11_fatal("Failed to create render target view");
    return result;
}

ID3D11DepthStencilView *
xd11_depth_stencil_view(ID3D11Texture2D * texture, D3D11_DEPTH_STENCIL_VIEW_DESC desc) {
    ID3D11DepthStencilView * result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilView(xd11.device, 
                                                   (ID3D11Resource *)texture, 
                                                   &desc,
                                                   &result)))
        xd11_fatal("Failed to create depth stencil view");
    return result;
}

ID3D11ShaderResourceView *
xd11_shader_res_view(ID3D11Texture2D * texture, D3D11_SHADER_RESOURCE_VIEW_DESC desc) {
    ID3D11ShaderResourceView * result = 0;
    if (FAILED(ID3D11Device_CreateShaderResourceView(xd11.device, 
                                                     (ID3D11Resource*)texture, 
                                                     &desc,
                                                     &result)))
        xd11_fatal("Failed to create shader res view");
    
    return result;
}

ID3D11DepthStencilState *
xd11_depth_stencil_state(D3D11_DEPTH_STENCIL_DESC desc) {
    ID3D11DepthStencilState * result = 0;
    if (FAILED(ID3D11Device_CreateDepthStencilState(xd11.device, 
                                                    &desc, 
                                                    &result)))
        xd11_fatal("Failed to create depth stencil state");
    return result;
}

ID3D11BlendState *
xd11_blend_state(D3D11_BLEND_DESC desc) {
    ID3D11BlendState * result = 0;
    if (FAILED(ID3D11Device_CreateBlendState(xd11.device, 
                                             &desc, 
                                             &result)))
        xd11_fatal("Failed to create blend state");
    return result;
}

ID3D11RasterizerState *
xd11_rasterizer_state(D3D11_RASTERIZER_DESC desc) {
    ID3D11RasterizerState * result = 0;
    if (FAILED(ID3D11Device_CreateRasterizerState(xd11.device,
                                                  &desc,
                                                  &result)))
        xd11_fatal("Failed to create rasterizer state");
    return result;
}

ID3D11SamplerState *
xd11_sampler_state(D3D11_SAMPLER_DESC desc) {
    ID3D11SamplerState * result = 0;
    if (FAILED(ID3D11Device_CreateSamplerState(xd11.device, 
                                               &desc, 
                                               &result)))
        xd11_fatal("Failed to create sample state");
    return result;
}

#if 0
D3D11_MAPPED_SUBRESOURCE mapped_sub_resource;
ID3D11DeviceContext_Map(xd11.deviceContext, 
                        (ID3D11Resource *)texture->handle, 
                        0, 
                        D3D11_MAP_WRITE_DISCARD, 
                        0, 
                        &mapped_sub_resource);
memcpy(mapped_sub_resource.pData, 
       bytes, 
       texture->size[0]*texture->size[1]*4);
ID3D11DeviceContext_Unmap(xd11.deviceContext, (ID3D11Resource *)texture.handle, 0);
#endif

void xd11_monitor_size(v2 dest) {
    dest[0] = (f32)GetSystemMetrics(SM_CXSCREEN);
    dest[1] = (f32)GetSystemMetrics(SM_CYSCREEN);
}

ID3D11VertexShader *
xd11_compile_vshader(char *source, ID3DBlob **compiledVS) {
    ID3D11VertexShader *result = 0;
    
    // Compile the shader
    ID3DBlob *errorVertexMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "vs_main", "vs_5_0",
                          0, 0, compiledVS, &errorVertexMessages))) {
        if (errorVertexMessages) {
            char *msg = (char *)(ID3D10Blob_GetBufferPointer(errorVertexMessages));
            xd11_log(msg);
            xd11_fatal("Failed to compile shader");
        }
        else {
            xd11_fatal("Failed to compile shader");
        }
    }
    
    // Create the vertex shader if compilation went ok
    ID3D11Device_CreateVertexShader(xd11.device,
                                    ID3D10Blob_GetBufferPointer(*compiledVS),
                                    ID3D10Blob_GetBufferSize(*compiledVS),
                                    NULL, &result);
    
    return result;
}

ID3D11PixelShader *xd11_compile_pshader(char *source, ID3DBlob **compiledPS) {
    ID3D11PixelShader *result = 0;
    
    // And compilation of the pixel shader
    ID3DBlob *errorPixelMessages;
    if (FAILED(D3DCompile(source, strlen(source) + 1,
                          0, 0, 0, "ps_main", "ps_5_0",
                          0, 0, compiledPS, &errorPixelMessages))) {
        if (errorPixelMessages) {
            char *msg = (char *)
            (ID3D10Blob_GetBufferPointer(errorPixelMessages));
            xd11_log(msg);
            xd11_fatal("Failed to compile shader");
        }
        else {
            xd11_fatal("Failed to compile shader");
        }
    }
    
    // Create the pixel shader if compilation went ok
    ID3D11Device_CreatePixelShader(xd11.device,
                                   ID3D10Blob_GetBufferPointer(*compiledPS),
                                   ID3D10Blob_GetBufferSize(*compiledPS),
                                   NULL, &result);
    
    return result;
}

ID3D11InputLayout *
xd11_input_layout(ID3DBlob *vs, D3D11_INPUT_ELEMENT_DESC *array, u32 count) {    
    ID3D11InputLayout * result = 0;
    // And create the input layout
    void *vsPointer = ID3D10Blob_GetBufferPointer(vs);
    u32 vsSize = (u32)ID3D10Blob_GetBufferSize(vs);
    
    if (FAILED(ID3D11Device_CreateInputLayout(xd11.device, array, count,
                                              vsPointer, vsSize, &result)))
        exit(1);
    
    return result;
}

void xd11_clear_rtv(ID3D11RenderTargetView *render_targetView, v4 clearColor) {
    ID3D11DeviceContext_ClearRenderTargetView(xd11.deviceContext, render_targetView,  clearColor);
}

void xd11_clear_dsv(ID3D11DepthStencilView * dsView, u32 flags, UINT8 min, UINT8 max) {
    ID3D11DeviceContext_ClearDepthStencilView(xd11.deviceContext, 
                                              dsView, 
                                              flags, min, max);
}

void xd11_render_pass(XD11RenderPass *pass, u32 vertex_count, 
                      Rect2 *scissor, u32 psResourceCount, ID3D11ShaderResourceView **psResources) {
    /* Topology */
    ID3D11DeviceContext_IASetPrimitiveTopology(xd11.deviceContext, pass->topology);
    /* Input Layout */
    ID3D11DeviceContext_IASetInputLayout(xd11.deviceContext, pass->inputLayout);
    /* Vertex Buffers */
    XD11VertexBuffers *vb = &pass->vBuffers;
    if (vb->count > 0)
        ID3D11DeviceContext_IASetVertexBuffers(xd11.deviceContext, 0, vb->count, 
                                               vb->array, vb->strides, vb->offsets);
    /* Vertex Shader */
    ID3D11DeviceContext_VSSetShader(xd11.deviceContext, pass->vShader, NULL, 0);
    /* VS CBuffers */
    if (pass->vShaderCBufferCount > 0)
        ID3D11DeviceContext_VSSetConstantBuffers(xd11.deviceContext, 0, 
                                                 pass->vShaderCBufferCount, pass->vShaderCBuffers);
    /* Pixel Shader */
    ID3D11DeviceContext_PSSetShader(xd11.deviceContext, pass->pShader, NULL, 0);
    
    /* PS Resources */
    if (psResourceCount > 0)
        ID3D11DeviceContext_PSSetShaderResources(xd11.deviceContext, 0, 
                                                 psResourceCount, psResources);
    /* Sampler State */
    ID3D11DeviceContext_PSSetSamplers(xd11.deviceContext, 0, 1, &pass->samplerState);
    
    {
        s32 count;
        D3D11_VIEWPORT *array;
        Array_toarray(pass->viewports, &array, &count);
        ID3D11DeviceContext_RSSetViewports(xd11.deviceContext, 
                                           count, array);
        xfree(array);
    }
    
    D3D11_RECT scissorArray[1];
    
    if (scissor) 
        scissorArray[0] = (RECT){(LONG)scissor->min[0], (LONG)scissor->min[1], (LONG)scissor->max[0], (LONG)scissor->max[1]};
    else 
        scissorArray[0] = (RECT){0, 0, (LONG)xd11.bbDim[0], (LONG)xd11.bbDim[1]};
    
    ID3D11DeviceContext_RSSetScissorRects(xd11.deviceContext, 
                                          narray(scissorArray), scissorArray);
    
    ID3D11DeviceContext_RSSetState(xd11.deviceContext, pass->rasterizerState);
    ID3D11DeviceContext_OMSetDepthStencilState(xd11.deviceContext, 
                                               pass->depthStencil.state, 0);
    
    ID3D11DeviceContext_OMSetBlendState(xd11.deviceContext, 
                                        pass->blendState, NULL, 
                                        0xffffffff);
    ID3D11DeviceContext_Draw(xd11.deviceContext, vertex_count, 0);
}

void xd11_swapChain_resize() {
    RECT rect;
    if (!GetClientRect(xd11.wndHandle, &rect)) {
        exit(1);
    }
    
    v2 bbDim = {
        (f32)(rect.right - rect.left),
        (f32)(rect.bottom - rect.top),
    };
    
    if (((UINT)bbDim[0] != 0 && (UINT)bbDim[1] != 0) &&
        (((UINT)bbDim[0] != xd11.bbDim[0]) || 
         ((UINT)bbDim[1] != xd11.bbDim[1]))) {
        v2_copy(bbDim, xd11.bbDim);
        
        xd11_resized();
    }
}

void xd11_initialize(XD11Config config) {
    /* Open the log file */
    fopen_s(&xd11.logFile, "xd11.log", "w");
    
    xd11_log("XD11 Libraries version 0.1 Alpha.");
    
    /* Register a window class */
    WNDCLASSEXA windowClass = xwin_wndclass(config.wndproc);
    windowClass.hIcon = config.icon;
    if (RegisterClassExA(&windowClass) == 0)
        xd11_fatal("Failed to register window class.");
    else
        xd11_log("Registered window class successfully.");
    
    /* Window style based on config */
    xd11.wcStyleEx =  config.wcStyleEx;
    xd11.wcStyle = (config.wcStyle == 0) ? 
        WS_OVERLAPPEDWINDOW | WS_VISIBLE : config.wcStyle;
    
    /* Window pos and size */
    if (config.wndP[0]==0 && config.wndP[1]==0)
        v2_copy(v2_zero, xd11.wndP);
    else
        v2_copy(config.wndP, xd11.wndP);
    
    if ((config.wndDim[0]==0 && config.wndDim[1]==0))
        v2_copy((v2){800,600}, xd11.wndDim);
    else
        v2_copy(config.wndDim, xd11.wndDim);
    
    v2_copy(xd11.wndDim, xd11.bbDim);
    
    RECT size = 
    { 
        (u32)xd11.wndP[0], (u32)xd11.wndP[1],
        (u32)(xd11.wndP[0] + xd11.wndDim[0]), (u32)(xd11.wndP[1] + xd11.wndDim[1])
    };
    
    AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, FALSE);
    v2_copy((v2){(f32)(size.right - size.left), (f32)(size.bottom - size.top)}, xd11.wndDim);
    
    /* Window title */
    xstrcpy(xd11.wndTitle, (config.wndTitle == 0) ? "XLib's xd11" : config.wndTitle);
    
    /* Create the window */
    xd11.wndHandle = CreateWindowExA(xd11.wcStyleEx, "xwindow_class", xd11.wndTitle,
                                     xd11.wcStyle, (s32)xd11.wndP[0], (s32)xd11.wndP[1], 
                                     (s32)xd11.wndDim[0], (s32)xd11.wndDim[1],
                                     NULL, NULL, GetModuleHandle(0), NULL);
    if (xd11.wndHandle == NULL)
        xd11_fatal("Failed to open window.");
    else
        xd11_log("Created window successfully.");
    
    /* Direct3D 11 backbuffer size, refresh rate, scale, swap chain, feature levels...  */
    DXGI_MODE_DESC mode_desc = 
    {
        (s32)xd11.bbDim[0], (s32)xd11.bbDim[1],
        (DXGI_RATIONAL){60,1},
        DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
        DXGI_MODE_SCALING_CENTERED
    };
    
    DXGI_SWAP_CHAIN_DESC swapChain_desc =
    {
        mode_desc,
        (DXGI_SAMPLE_DESC){1, 0},
        DXGI_USAGE_RENDER_TARGET_OUTPUT,
        2,
        xd11.wndHandle,
        true, 
        DXGI_SWAP_EFFECT_FLIP_DISCARD,
        0
    };
    
    D3D_FEATURE_LEVEL feature_levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };
    
    UINT flags = 0;
#ifndef XD11_RELEASE_BUILD
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
                                               D3D_DRIVER_TYPE_HARDWARE, 
                                               NULL,
                                               flags, 
                                               feature_levels, 
                                               narray(feature_levels), 
                                               D3D11_SDK_VERSION,
                                               &swapChain_desc,
                                               &xd11.swapChain,
                                               &xd11.device,
                                               NULL,
                                               &xd11.deviceContext);
    if (FAILED(hr)) {
        char buf[512];
        snprintf(buf, 512, "Erro numero: %d", hr);
        
        // Convert the HRESULT to a human-readable error message
        // using the FormatMessage static
        TCHAR errorMsg[512];
        DWORD errorMsgLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                             NULL, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                             errorMsg, 0, NULL);
        
        xd11_fatal("Failed to create device and swapchain");
    }
    else
        xd11_log("Created device and swapchain successfully.");
    
#ifndef XD11_RELEASE_BUILD
    /* Direct3D 11 Debug interface */
    if (FAILED(ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11Debug, 
                                           (void**)&xd11.debug)))
        xd11_fatal("Failed to query interface.");
    else
        xd11_log("Queried interface successfully.");
    
    ID3D11InfoQueue* infoQueue;
    ID3D11Device_QueryInterface(xd11.device, &IID_ID3D11InfoQueue, (void**)&infoQueue);
    ID3D11InfoQueue_SetBreakOnSeverity(infoQueue, D3D11_MESSAGE_SEVERITY_WARNING, TRUE);
    ID3D11InfoQueue_Release(infoQueue);
#endif
    
    LARGE_INTEGER lastCounter;
    QueryPerformanceCounter(&lastCounter);
    
    /* Load cursors */
    char cursorFullPath[260];
    xwin_path_abs(cursorFullPath, 260, "cursors\\cur1156.ani");
    cursor_select = LoadCursorFromFileA(cursorFullPath);
    
    xwin_path_abs(cursorFullPath, 260, "cursors\\link_select.cur");
    cursor_link = LoadCursorFromFileA(cursorFullPath);
    
    xwin_path_abs(cursorFullPath, 260, "cursors\\horizontal_resize.cur");
    cursor_horizontal_resize = LoadCursorFromFileA(cursorFullPath);
    
    xwin_path_abs(cursorFullPath, 260, "cursors\\palm.cur");
    cursor_palm = LoadCursorFromFileA(cursorFullPath);
    
    xwin_path_abs(cursorFullPath, 260, "cursors\\drag.cur");
    cursor_drag = LoadCursorFromFileA(cursorFullPath);
    
    xd11.running = true;
}

void xd11_set_cursor(HCURSOR cursor) {
    xwin.lastCursorSet = cursor;
}

void xd11_update(void) {
    // Measure fps
    LARGE_INTEGER counter = xwin_time();
    
    // Calculate dt
    xd11.dt = xwin_seconds(xd11.lastCounter, counter);
    if (xd11.dt > 1000)
        xd11.dt = 0;
    
    // Save counter
    xd11.lastCounter = counter;
    
    xd11_swapChain_resize();
    
    IDXGISwapChain_Present(xd11.swapChain, 1, 0);
}

void xd11_shutdown(void) {
    xmemcheck();
    
#ifndef XD11_RELEASE_BUILD
    if (xd11.debug) 
        ID3D11Debug_Release(xd11.debug);
#endif
    
    if (xd11.swapChain) 
        IDXGISwapChain_Release(xd11.swapChain);
    
    if (xd11.deviceContext) 
        ID3D11DeviceContext_Release(xd11.deviceContext);
    
    if (xd11.device) 
        ID3D11Device_Release(xd11.device);
}

#endif