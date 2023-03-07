
typedef struct XCommandHeader XCommandHeader;
typedef struct XCommandLine   XCommandLine;
typedef struct XCommandSprite XCommandSprite;
typedef struct XCommandMesh   XCommandMesh;
typedef enum   XCommandType   XCommandType;

enum XCommandType
{
    XCommandType_null,
    XCommandType_line,
    XCommandType_sprite,
    XCommandType_mesh,
};

struct XCommandHeader
{
    XCommandType type;
};

struct XCommandLine
{
    v2f a;
    v2f b;
    v4f col;
    f32 sort;
};

struct XCommandSprite
{
    v2f pos;
    v2f size;
    v2f pivot;
    f32 rot;
    f32 sort;
    v4f col;
    rect2f uv;
};

struct XCommandMesh
{ 
    f32 rot;
    f32 sort;
    v2f pos;
    v2f scale;
    v2f pivot;
    v4f col;
    Array_T vertices;
};

/* =========================================================================
   DRAWING / RENDERING UTILITIES
   ========================================================================= */

void xline   (v2f a, v2f b, v4f color, f32 sort);
void xarrow  (Array_T *batch, v2f a, v2f b, v4f col, XSprite head, v2f size, f32 sort);
void xsprite (Array_T *batch, XSprite s, v2f pos, v2f dim, v4f col, v2f pivot, f32 rot, f32 sort);
f32  xglyph  (Array_T *batch, XFont f, u32 unicode, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort);
f32  xstring (Array_T *batch, XFont f, wchar_t *string, v2f pos, v4f c, v2f pivot, f32 rot, f32 sort, bool fixedwidth);
void xmesh   (Array_T *batch, Array_T vertices, v2f pos, v2f scale, v4f color, v2f pivot, f32 rot, f32 sort);

void xlinerect   (v2f pos, v2f dim, v4f col, f32 sort);
void xlinecircle (v2f pos, f32 radius, s32 n, v4f col, f32 sort);
void xlinemesh   (Array_T vertices, v2f pos, v2f scale, v4f col, f32 sort);

v2f  xglyphsize  (XFont font, u32 unicode);
v2f  xstringsize (XFont font, wchar_t *s);

u32 produce_vertices_from_sprite_and_mesh_groups()
{
    f32 rotRad, pvx, pvy;
    v2f p, pa, pb, pc, pd, xaxis, yaxis;
    s32 i, j, vi, spritesVertexCount, meshsVertexCount;
    XVertex3D *v;
    Array_T *batch;
    XSpriteCommand *scmd;
    XMeshCommand *mcmd;
    
    /*  Count number of vertices from all sprite groups                    */
    
    for (i=0, spritesVertexCount=0; i < xrnd.spriteBatchIndex; ++i)
        spritesVertexCount += 6*(xrnd.spriteBatchArray[i].top);
    
    /*  Count number of vertices from all mesh groups                      */
    
    for (i=0, meshsVertexCount=0; i < xrnd.meshBatchIndex; ++i)
        for (j=0; j < xrnd.meshBatchArray[i].top; ++j)
        meshsVertexCount += ((XMeshCommand*)Array_get(xrnd.meshBatchArray[i], j))->vertices.top;
    
    v = 0;
    vi = 0;
    if (spritesVertexCount+meshsVertexCount) {
        v = xnalloc(spritesVertexCount+meshsVertexCount, XVertex3D);
    }
    
    /*  Loop through all sprite groups and produces the vertices.          */
    if (spritesVertexCount>0) {
        for (i=0; i<xrnd.spriteBatchIndex; ++i)
        {
            batch = xrnd.spriteBatchArray + i;
            if (batch->top > 0)
            {
                for (j=0; j < batch->top; ++j)
                {
                    scmd = Array_get(*batch, j);
                    
                    rotRad = -radf(scmd->rot);
                    
                    pvx = scmd->pivot.x;
                    pvy = scmd->pivot.y;
                    
                    xaxis = mul2f(scmd->size.x, ini2f(cosf(rotRad), -sinf(rotRad)));
                    yaxis = mul2f(scmd->size.y, ini2f(sinf(rotRad), cosf(rotRad)));
                    
                    p = scmd->pos;
                    pa = sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(  pvy, yaxis));
                    pb = sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(  pvy, yaxis));
                    pc = sub2f(sub2f(p, mul2f(pvx-1, xaxis)), mul2f(pvy-1, yaxis));
                    pd = sub2f(sub2f(p, mul2f(  pvx, xaxis)), mul2f(pvy-1, yaxis));
                    
                    v4f c = scmd->col;
                    f32 umin = scmd->uv.min.x;
                    f32 umax = scmd->uv.max.x;
                    f32 vmin = scmd->uv.min.y;
                    f32 vmax = scmd->uv.max.y;
                    f32 sort = scmd->sort;
                    
                    XVertex3D va = { pa.x, pa.y, sort, umin, (xrnd.topDown ? vmax : vmin), c.r,c.g,c.b,c.a };
                    XVertex3D vb = { pb.x, pb.y, sort, umax, (xrnd.topDown ? vmax : vmin), c.r,c.g,c.b,c.a };
                    XVertex3D vc = { pc.x, pc.y, sort, umax, (xrnd.topDown ? vmin : vmax), c.r,c.g,c.b,c.a };
                    XVertex3D vd = { pd.x, pd.y, sort, umin, (xrnd.topDown ? vmin : vmax), c.r,c.g,c.b,c.a };
                    
                    v[vi++] = va;
                    v[vi++] = (xrnd.topDown ? vb : vc);
                    v[vi++] = (xrnd.topDown ? vc : vb);
                    v[vi++] = va;
                    v[vi++] = (xrnd.topDown ? vc : vd);
                    v[vi++] = (xrnd.topDown ? vd : vc);
                }
            }
        }
    }
    
    /*  Loop through all mesh groups and produces the vertices.          */
    
    if (meshsVertexCount>0) {
        for (i=0; i < xrnd.meshBatchIndex; ++i)
        {
            batch = xrnd.meshBatchArray + i;
            if (batch->top > 0)
            {
                for (j=0; j < batch->top; ++j)
                {
                    mcmd = Array_get(*batch, j);
                    
                    p = mcmd->pos;
                    
                    for (s32 k = 0; k < mcmd->vertices.top; ++k)
                    {
                        XVertex3D *v3d = (XVertex3D *)Array_get(mcmd->vertices, k);
                        
                        XVertex3D temp = 
                        {
                            mcmd->pos.x + v3d->pos.x*mcmd->scale.x, mcmd->pos.y + v3d->pos.y*mcmd->scale.y, v3d->pos.z,
                            v3d->uv.x, v3d->uv.y,
                            v3d->color.r*mcmd->col.r, v3d->color.g*mcmd->col.g, v3d->color.b*mcmd->col.b, v3d->color.a*mcmd->col.a,
                        };
                        
                        v[vi++] = temp;
                    }
                }
            }
        }
    }
    
    if (spritesVertexCount+meshsVertexCount > 0)
    {
        xd11_update_vertex_buffer(v, vi);
        
        xfree(v);
    }
    
    reset_sprite_groups();
    reset_mesh_groups();
    
    return vi;
}

void free_line_group()
{
    Array_free(&xrnd.lineBatch);
}

void reset_line_group()
{
    xrnd.lineBatch.top = 0;
}

u32 produce_vertices_from_line_group()
{
    s32 vc, vi, i;
    XLineVertex3D *v;
    XLineCommand *cmd;
    D3D11_MAPPED_SUBRESOURCE mr;
    
    vc = 0;
    vc += 2 * xrnd.lineBatch.top;
    
    if (vc>0) {
        v = xnalloc(vc, XLineVertex3D);
        vi = 0;
        
        for (i = 0; i < xrnd.lineBatch.top; ++i) {
            cmd = Array_get(xrnd.lineBatch, i);
            
            XLineVertex3D a =
            {
                cmd->a.x, cmd->a.y, cmd->sort,
                cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
            };
            
            XLineVertex3D b =
            {
                cmd->b.x, cmd->b.y, cmd->sort,
                cmd->col.r, cmd->col.g, cmd->col.b, cmd->col.a
            };
            
            v[vi++] = a;
            v[vi++] = b;
        }
        
        ID3D11DeviceContext_Map(xrnd.d11DeviceContext, 
                                (ID3D11Resource *)xrnd.lineVertexBuffer, 
                                0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
        memcpy(mr.pData, v, vc*sizeof(XLineVertex3D));
        ID3D11DeviceContext_Unmap(xrnd.d11DeviceContext, 
                                  (ID3D11Resource *)xrnd.lineVertexBuffer, 0);
        xfree(v);
        
        reset_line_group();
    }
    
    return vc;
}


void xline(v2f a, v2f b, v4f c, f32 s)
{
    xpush_line_command(a, b, c, s);
}

void xstroke(v2f a, v2f b, v4f c, f32 w, f32 s)
{
    v2f dir, o1, o2;
    f32 angle;
    
    dir = nrm2f(sub2f(b,a));
    angle = atan2f(dir.y, dir.x);
    
    o1=ini2f(a.x+w/2*cosf(angle+PIf/2), a.y+w/2*sinf(angle+PIf/2));
    o2=ini2f(a.x+w/2*cosf(angle-PIf/2), a.y+w/2*sinf(angle-PIf/2));
    
    xline(o1, o2, ini4f(1,1,1,1), 0);
    xline(o1,  b, ini4f(1,1,1,1), 0);
    xline( b, o2, ini4f(1,1,1,1), 0);
    xline( a,  b, ini4f(1,1,1,1), 0);
}

void xarrow(Array_T *g, v2f a, v2f b, v4f c, XSprite head, v2f headsize, f32 s)
{
    v2f dir;
    
    dir=nrm2f(sub2f(b,a));
    xline(a, b, c, s);
    f32 arrowAngle = atan2f(dir.y,dir.x);
    xsprite(g, head, b, headsize, c, ini2f(1,.5f), degf(arrowAngle), s);
}

void xsprite(Array_T *g, XSprite sp, v2f p, v2f s, v4f cl, v2f pv, f32 r, f32 st)
{
    xpush_rect_command(g, p, s, sp.uv, cl, pv, r, st);
}

void xmesh(Array_T *b, Array_T v, v2f p, v2f s, v4f c, v2f pv, f32 r, f32 st)
{
    xpush_mesh_command(b, v, p, s, c, pv, r, st);
}

f32 xglyph(Array_T *g, XFont f, u32 u, v2f p, v4f c, v2f pv, f32 r, f32 s)
{
    XSprite *sp = (XSprite *)Table_get(f.glyphs, &u);
    if (sp) {
        xsprite(g, *sp, p, sp->size, c, pv, r, s);
        return sp->size.x;
    }
    return 0;
}

f32 xstring(Array_T *g, XFont f, wchar_t *s, v2f p, v4f c, v2f pv, f32 r, f32 st, bool fw)
{
    f32 startX, w;
    XSprite *sp;
    u32 u;
    
    startX = p.x;
    wchar_t *at = s;
    while (*at)
    {
        w=0;
        u=*at;
        sp = (XSprite *)Table_get(f.glyphs, &u);
        if (sp) {
            if (fw)
                p.x += .5f*(f.charwidth-sp->size.x);
            
            p.x -= sp->align.x;
            p.y -= sp->align.y;
            
            if (fw)
                w=f.charwidth;
            else
                w=sp->size.x;
            //            Draw_outline_rect(g, p, sp->size, c, st);
            xsprite(g, *sp, p, sp->size, c, pv, r, st+1);
            if (fw)
                p.x -= .5f*(f.charwidth-sp->size.x);
            p.y += sp->align.y;
        }
        
        p.x += w;
        at++;
    }
    return p.x-startX;
}

void xlinerect(v2f p, v2f wh, v4f cl, f32 st)
{
    xpush_line_command(p, ini2f(p.x + wh.x, p.y), cl, st); // bottom
    xpush_line_command(ini2f(p.x, p.y + wh.y), ini2f(p.x + wh.x, p.y + wh.y), cl, st); // top
    xpush_line_command(p, ini2f(p.x, p.y + wh.y + 1), cl, st); // left
    xpush_line_command(ini2f(p.x + wh.x, p.y), ini2f(p.x + wh.x, p.y + wh.y), cl, st); // right
}

void xlinecircle(v2f pos, f32 radius, s32 n, v4f col, f32 sort)
{
    for (s32 i = 0; i < n; ++i) {
        f32 angle1 = radf(i * 360.0f / n);
        f32 angle2 = radf((i + 1) * 360.0f / n);
        
        v2f p1 = ini2f(pos.x + sinf(angle1) * radius, pos.y + cosf(angle1) * radius);
        v2f p2 = ini2f(pos.x + sinf(angle2) * radius, pos.y + cosf(angle2) * radius);
        xpush_line_command(p1, p2, col, sort);
    }
}

void xlinemesh(Array_T vertices, v2f pos, v2f scale, v4f col, f32 sort)
{
    for (s32 i = 0; i < vertices.top-1; ++i)
    {
        XVertex3D *v3d1 = Array_get(vertices, i);
        XVertex3D* v3d2 = Array_get(vertices, i + 1);
        
        v2f p1 = add2f(pos, ini2f(scale.x*v3d1->pos.x, scale.y*v3d1->pos.y));
        v2f p2 = add2f(pos, ini2f(scale.x*v3d2->pos.x, scale.y*v3d2->pos.y));
        
        xpush_line_command(p1, p2, col, sort);
    }
}

v2f xglyphsize(XFont f, u32 u)
{
    XSprite *sp;
    
    sp=(XSprite *)Table_get(f.glyphs, &u);
    if (sp) {
        return sp->size;
    }
    return ini2f(0,0);
}

v2f xstringsize(XFont f, wchar_t *s)
{
    f32 w, mh;
    wchar_t *c;
    v2f temp;
    
    w=0;
    mh=-10000;
    temp=ini2f(0,0);
    for (c=s; *c; ++c)
    {
        temp=xglyphsize(f, *c);
        if (mh<temp.y)
            mh=temp.y;
        w+=temp.x;
    }
    
    return ini2f(w,temp.y);
}

void create_textured2d_shader(void)
{
    D3D11_INPUT_ELEMENT_DESC ied [] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };


    // First we will declare the vertex shader code
    char *vertexShaderSource = 
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

    // Then the pixel shader code   
    char *pixelShaderSource = 
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
}

void create_lines_shader(void)
{
    // First we will declare the vertex shader code
    char *vertexShaderSource = 
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

    // Then the pixel shader code
    char *pixelShaderSource = 
        "struct ps_in"
        "{"
        "  float4 position_clip : SV_POSITION;"
        "  float4 color         : COLOR;"
        "};"
        
        "float4 ps_main(ps_in input) : SV_TARGET"
        "{"
        "  return input.color;"
        "}";


    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        
        { "COL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
}


ID3D11Buffer *xrenderer2d_vertexBuffer_create(void)
{
    D3D11_BUFFER_DESC vertexBufferDesc =
    {
        size, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER,
        D3D11_CPU_ACCESS_WRITE, 0, sizeof(XVertex3D)
    };
}

void xrenderer2d_shutdown(void)
{

    if (xrnd.textureAtlasSRV) 
        ID3D11ShaderResourceView_Release(xrnd.textureAtlasSRV);
    
    if (xrnd.vertexCBuffer) 
        ID3D11Buffer_Release(xrnd.vertexCBuffer);
    
    if (xrnd.lineVertexBuffer) 
        ID3D11Buffer_Release(xrnd.lineVertexBuffer);
    
    if (xrnd.texturedVertexBuffer) 
        ID3D11Buffer_Release(xrnd.texturedVertexBuffer);
    
    if (xrnd.lineInputLayout) 
        ID3D11InputLayout_Release(xrnd.lineInputLayout);
    
    if (xrnd.texturedInputLayout) 
        ID3D11InputLayout_Release(xrnd.texturedInputLayout);
    
    if (xrnd.linePixelShader)
        ID3D11PixelShader_Release(xrnd.linePixelShader);
    
    if (xrnd.texturedPixelShader) 
        ID3D11PixelShader_Release(xrnd.texturedPixelShader);
    
    if (xrnd.lineVertexShader) 
        ID3D11VertexShader_Release(xrnd.lineVertexShader);
    
    if (xrnd.texturedVertexShader)
        ID3D11VertexShader_Release(xrnd.texturedVertexShader);
    
    if (xrnd.samplerState)
        ID3D11SamplerState_Release(xrnd.samplerState);
    
    if (xrnd.rasterizerState) 
        ID3D11RasterizerState_Release(xrnd.rasterizerState);
    
    if (xrnd.blendState) 
        ID3D11BlendState_Release(xrnd.blendState);
    
    if (xrnd.depthStencilState)
        ID3D11DepthStencilState_Release(xrnd.depthStencilState);
    
    if (xrnd.depthStencilView) 
        ID3D11DepthStencilView_Release(xrnd.depthStencilView);
    
    if (xrnd.renderTargetView) 
        ID3D11RenderTargetView_Release(xrnd.renderTargetView);
    
    if (xrnd.textureAtlasTexture2D) 
        ID3D11Texture2D_Release(xrnd.textureAtlasTexture2D);
    
    if (xrnd.depthStencilBufferTexture2D) 
        ID3D11Texture2D_Release(xrnd.depthStencilBufferTexture2D);
    
    if (xrnd.backBufferTexture2D) 
        ID3D11Texture2D_Release(xrnd.backBufferTexture2D);

}

{
    u32 svc = produce_vertices_from_sprite_and_mesh_groups();
    u32 lvc = produce_vertices_from_line_group();
    
    f32 scaleX = 2.0f / xrnd.backBufferSize.x;
    f32 scaleY = (xrnd.topDown ? -2.0f : 2.0f) / xrnd.backBufferSize.y;
    mat4f wvpMatrix =
    {
        scaleX, 0, 0, -1,
        0, scaleY, 0, (xrnd.topDown ? 1.f : -1.f),
        0, 0, .001f, 0,
        0, 0, 0, 1,
    };
    
    xrnd.vertexCBufferData.WVP = wvpMatrix;
    ID3D11DeviceContext_UpdateSubresource(xrnd.d11DeviceContext, 
                                          (ID3D11Resource*)xrnd.vertexCBuffer, 0, NULL, 
                                          &xrnd.vertexCBufferData, 0, 0);
    
    ID3D11RenderTargetView* views[1] = { xrnd.renderTargetView };
    ID3D11DeviceContext_OMSetRenderTargets(xrnd.d11DeviceContext, 1, views, xrnd.depthStencilView);


}

void xrender_resized(void)
{
    ID3D11RenderTargetView_Release(xrnd.renderTargetView);
    ID3D11DepthStencilView_Release(xrnd.depthStencilView);
    ID3D11Texture2D_Release(xrnd.backBufferTexture2D);
    ID3D11Texture2D_Release(xrnd.depthStencilBufferTexture2D);
    
    IDXGISwapChain_ResizeBuffers(xrnd.d11SwapChain, 2, 
                                 (UINT)backBufferSize.x,(UINT)backBufferSize.y, 
                                 DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    
    if (FAILED(IDXGISwapChain_GetBuffer(xrnd.d11SwapChain, 0, &IID_ID3D11Texture2D, 
                                        (void **)&xrnd.backBufferTexture2D)))
        exit(1);
    
    if (FAILED(ID3D11Device_CreateRenderTargetView(xrnd.d11Device, 
                                                   (ID3D11Resource *)xrnd.backBufferTexture2D, 
                                                   0, &xrnd.renderTargetView)))
        exit(1);
    
    xrnd.depthStencilBufferTexDesc.Width = (UINT)xrnd.backBufferSize.x;
    xrnd.depthStencilBufferTexDesc.Height = (UINT)xrnd.backBufferSize.y;
    
    if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &xrnd.depthStencilBufferTexDesc, 0, 
                                            &xrnd.depthStencilBufferTexture2D)))
        exit(1);
    
    if (FAILED(ID3D11Device_CreateDepthStencilView(xrnd.d11Device, 
                                                   (ID3D11Resource *)xrnd.depthStencilBufferTexture2D, 
                                                   &xrnd.depthStencilViewDesc,
                                                   &xrnd.depthStencilView)))
        exit(1);
}

{
    /* Get back buffer texture from swap chain */    
    if (FAILED(IDXGISwapChain_GetBuffer(xrnd.d11SwapChain, 0, &IID_ID3D11Texture2D, 
                                        (void**)&xrnd.backBufferTexture2D)))
        exit(1);

    /* Create depth stencil buffer texture */    
    DXGI_FORMAT depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D11_TEXTURE2D_DESC depthStencilBufferTexDesc =
    {
        (s32)xrnd.backBufferSize.x, (s32)xrnd.backBufferSize.y,
        0, 1, depthStencilBufferFormat, backBufferSize, 
        D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL, 0, 0
    };
    
    xrnd.depthStencilBufferTexDesc = depthStencilBufferTexDesc;
    
    if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &depthStencilBufferTexDesc, 0, 
                                            &xrnd.depthStencilBufferTexture2D)))
        exit(1);

    /* Create texture atlas texture */        
    xrnd.textureAtlas.size = (config.textureAtlasSize == 0) ? 
        4096 : config.textureAtlasSize;
    
    xrnd.textureAtlas.at.x = xrnd.textureAtlas.at.y = xrnd.textureAtlas.bottom = 0;
    
    atf = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC atd = { xrnd.textureAtlas.size, xrnd.textureAtlas.size, 1, 1, atf, {1,0},
        D3D11_USAGE_DYNAMIC, D3D11_BIND_SHADER_RESOURCE, D3D11_CPU_ACCESS_WRITE, 0, };
    
    u32 ams = xrnd.textureAtlas.size * xrnd.textureAtlas.size * 4;
    xrnd.textureAtlas.bytes = (u8*)xalloc(ams);
    
    D3D11_SUBRESOURCE_DATA ad = { xrnd.textureAtlas.bytes, (UINT)(xrnd.textureAtlas.size * 4), 0 };
    if (FAILED(ID3D11Device_CreateTexture2D(xrnd.d11Device, &atd, &ad, 
                                            &xrnd.textureAtlasTexture2D)))
        exit(1);

    /* Create render target view */        
    if (FAILED(ID3D11Device_CreateRenderTargetView(xrnd.d11Device, 
                                                   (ID3D11Resource*)xrnd.backBufferTexture2D, 
                                                   0, &xrnd.renderTargetView)))
        exit(1);
    

    /* Create depth stencil view */        
    xrnd.depthStencilViewDesc = xdsviewdesc(depthStencilBufferFormat, 
                                            D3D11_DSV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateDepthStencilView(xrnd.d11Device, 
                                                   (ID3D11Resource*)xrnd.depthStencilBufferTexture2D, 
                                                   &xrnd.depthStencilViewDesc,
                                                   &xrnd.depthStencilView)))
        exit(1);

    /* Create shader resource view */            
    D3D11_SHADER_RESOURCE_VIEW_DESC asrvd = xshadresview(atf, D3D_SRV_DIMENSION_TEXTURE2D);
    if (FAILED(ID3D11Device_CreateShaderResourceView(xrnd.d11Device, 
                                                     (ID3D11Resource*)xrnd.textureAtlasTexture2D, 
                                                     &asrvd, &xrnd.textureAtlasSRV)))
        exit(1);
    

    /* Create depth stencil state */            
    D3D11_DEPTH_STENCIL_DESC dsd = xdepthstencildesc();
    if (FAILED(ID3D11Device_CreateDepthStencilState(xrnd.d11Device, &dsd, &xrnd.depthStencilState)))
        exit(1);
    
    /* Create blend state */            
    D3D11_BLEND_DESC bd = xblenddesc();
    if (FAILED(ID3D11Device_CreateBlendState(xrnd.d11Device, &bd, &xrnd.blendState)))
        exit(1);
    
    /* Create rasterizer state */            
    D3D11_RASTERIZER_DESC rasterizerDesc = xrasterstate();
    if (FAILED(ID3D11Device_CreateRasterizerState(xrnd.d11Device, &rasterizerDesc, 
                                                  &xrnd.rasterizerState)))
        exit(1);

    /* Create sampler state */            
    D3D11_SAMPLER_DESC sd = xsamplerdesc();
    if (FAILED(ID3D11Device_CreateSamplerState(xrnd.d11Device, &sd, &xrnd.samplerState)))
        exit(1);
    
    /* Create shaders */            
    create_sprites_shaders();
    create_lines_shaders();
    
    /* Create vertex buffers */            
    create_sprites_vertex_buffer();
    create_lines_vertex_buffer();
    
    /* Create constant buffers */            
    D3D11_BUFFER_DESC vcbd = xcbufferdesc(sizeof(XVertexCBuffer));
    if (FAILED(ID3D11Device_CreateBuffer(xrnd.d11Device, &vcbd, 0, &xrnd.vertexCBuffer)))
        exit(1);

}