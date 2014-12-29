#pragma once
#define SNUFF_POST_PROCESSING_SHADER "cbuffer ConstantBuffer : register(b0)\n\
{\n\
\tfloat Time;\n\
}\n\
\n\
cbuffer Uniforms : register(b1)\n\
{\n\
\n\
}\n\
\n\
struct VOut\n\
{\n\
\tfloat4 position : SV_POSITION;\n\
\tfloat4 colour : COLOUR;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 colour : COLOUR)\n\
{\n\
\tVOut output;\n\
\toutput.position = position;\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
Texture2D tex2D;\n\
SamplerState SampleType;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat4 colour = tex2D.Sample(SampleType, input.texcoord);\n\
\treturn colour;\n\
}"