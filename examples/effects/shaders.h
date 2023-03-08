#ifndef SHADERS_H
#define SHADERS_H

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


#endif //SHADERS_H
