cbuffer ConstantBuffer : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
  float Alpha;
  float3 Blend;
  float4x4 InvWorld;
}

cbuffer Uniforms : register(b1)
{

}

struct VOut
{
  float4 position : SV_POSITION;
  float4 COLOUR : COLOUR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
  float3 tangent: TANGENT;
  float3 binormal: BINORMAL;
};

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 COLOUR : COLOUR, float3 tangent : TANGENT, float3 binormal : BINORMAL)
{
  VOut output;
  output.position = mul(position, WorldViewProjection);
  output.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);
  output.tangent = normalize(mul(float4(normal, 0), InvWorld).xyz);
  output.binormal = normalize(mul(float4(normal, 0), InvWorld).xyz);
  output.texcoord = texcoord;
  output.COLOUR = COLOUR;
  return output;
}

Texture2D textures[2];
SamplerState SampleType;

float4 PS(VOut input) : SV_TARGET
{	
  float4 textureCOLOUR = textures[0].Sample(SampleType,input.texcoord);
  float4 COLOUR = float4(textureCOLOUR.rgb * Blend * input.COLOUR.rgb, textureCOLOUR.a);
  COLOUR.a *= Alpha;

  return COLOUR;
}