cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
}

struct VOut
{
  float4 position : SV_POSITION;
  float4 unmodified : POSITION;
  float4 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
};

float4 GetVertexPosition(float4 position)
{
  return float4(position.x,position.y+sin((256-(position.z*2))*(256-(position.x*2))/2000 + Time/30)*4,position.zw);
}

float3 GetVertexNormal(float4 position)
{
  float3 A = GetVertexPosition(position).xyz;
  position.x+=1;
  float3 B = GetVertexPosition(position).xyz;
  position.z+=1;
  float3 C = GetVertexPosition(position).xyz;

  return cross(C-A,B-A);
}

VOut VS(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
  VOut output;
  output.unmodified = position;
  position = GetVertexPosition(position);
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
  float4 textureColor = Texture.Sample(SampleType, input.texcoord*10);
  float4 color = (textureColor * input.color);

  float3 normal = normalize(GetVertexNormal(input.unmodified));

	return float4(color.rgb*dot(normal,float3(1.3,1.5,1)),1);
}