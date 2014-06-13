cbuffer ConstantBuffer : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
  float Alpha;
}

cbuffer Uniforms : register(b1)
{
  float2 Frame;
  float2 FrameOffset;
}

struct VOut
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
};

VOut VS(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
  VOut output;
  output.position = mul(position, WorldViewProjection);
  output.color = color;
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
  float4 color = (textureColor * input.color);
  color.a *= Alpha;
  
	return color;
}