cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
  float Alpha;
  float dummy;
  float4 test4;
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
  float4 textureColor = Texture.Sample(SampleType, input.texcoord);
  float4 color = (textureColor * input.color);
  color.a *= Alpha;
  
	return test4;
}