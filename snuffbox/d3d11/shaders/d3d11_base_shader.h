#pragma once

#define SNUFF_BASE_SHADER "cbuffer ConstantBuffer : register(b0)\n\
{\n\
\tfloat Time;\n\
\tfloat4x4 World;\n\
\tfloat4x4 View;\n\
\tfloat4x4 Projection;\n\
\tfloat4x4 WorldViewProjection;\n\
\tfloat Alpha;\n\
\tfloat3 Blend;\n\
\tfloat4x4 InvWorld;\n\
\tfloat4 AnimationCoords;\n\
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
\tfloat3 normal : NORMAL;\n\
\tfloat2 texcoord : TEXCOORD0;\n\
};\n\
\n\
VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 colour : COLOUR)\n\
{\n\
\tVOut output;\n\
\toutput.position = mul(position, WorldViewProjection);\n\
\toutput.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);\n\
\toutput.texcoord = texcoord;\n\
\toutput.colour = colour;\n\
\treturn output;\n\
}\n\
\n\
Texture2D textures[1];\n\
SamplerState SampleType;\n\
\n\
float4 PS(VOut input) : SV_TARGET\n\
{\n\
\tfloat x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;\
\tfloat y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;\
\tfloat4 textureColour = textures[0].Sample(SampleType, float2(x, y));\n\
\tfloat4 colour = float4(textureColour.rgb * Blend * input.colour.rgb, textureColour.a);\n\
\tcolour.a *= Alpha;\n\
\treturn colour;\n\
}"