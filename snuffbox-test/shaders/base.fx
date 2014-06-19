cbuffer ConstantBuffer : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
  float Alpha;
  float3 Blend;
}

cbuffer Uniforms : register(b1)
{

}

struct VOut
{
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
};

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
  VOut output;
  output.position = mul(position, WorldViewProjection);
	output.normal = normal;
	output.texcoord = texcoord;
  return output;
}

Texture2D Texture;
SamplerState SampleType;

float4 PS(VOut input) : SV_TARGET
{	
  float4 textureColor = Texture.Sample(SampleType,input.texcoord);
  float4 color = float4(textureColor.rgb * Blend.rgb, textureColor.a);
  color.a *= Alpha;

  return color;
}