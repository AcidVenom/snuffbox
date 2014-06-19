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
  float2 Frame;
  float2 FrameOffset;
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
  float2 TexCoord = float2((input.texcoord.x+FrameOffset.x)*Frame.x,(input.texcoord.y+FrameOffset.y)*Frame.y);
  float4 textureColor = Texture.Sample(SampleType,TexCoord);
  float4 color = float4(textureColor.rgb * Blend.rgb,textureColor.a);
  color.a *= Alpha;
  clip(color.a - 0.1);
  
	return color;
}