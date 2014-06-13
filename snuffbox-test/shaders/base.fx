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
  float3 Color;
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
  float borderWidth = 0.05;
  float4 textureColor = Texture.Sample(SampleType,input.texcoord);
  float4 color = (textureColor * input.color);
  color.a *= Alpha;
  float4 normal = mul(float4(input.normal,1), mul(World,Projection));
  float3 rgb = lerp(color.rgb*(saturate(dot(normal,float4(-0.5,-1,0.7,0)))+0.01),Color,0.8);
	
  if (input.texcoord.x < borderWidth || input.texcoord.x > 1-borderWidth || input.texcoord.y < borderWidth || input.texcoord.y > 1-borderWidth)
  {
    return float4(0,0,0,1);
  }
  else
  {
    return float4(rgb,color.a);
  }
}