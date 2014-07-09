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
  float4 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
  float3 tangent: TANGENT;
  float3 binormal: BINORMAL;
};

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 color : COLOR, float3 tangent : TANGENT, float3 binormal : BINORMAL)
{
  VOut output;
  output.position = mul(position, WorldViewProjection);
  output.normal = normalize(mul(normal, (float3x3)InvWorld));
  output.tangent = normalize(mul(tangent, (float3x3)InvWorld));
  output.binormal = normalize(mul(binormal, (float3x3)InvWorld));
	output.texcoord = texcoord;
  output.color = color;
  return output;
}

Texture2D textures[2];
SamplerState SampleType;

float4 PS(VOut input) : SV_TARGET
{	
  float4 textureColor = textures[0].Sample(SampleType,input.texcoord);
  float4 normalMap = textures[1].Sample(SampleType,input.texcoord);
  
  normalMap = (normalMap * 2.0f) - 1.0f;

  float3 normal = (normalMap.x * input.tangent) + (normalMap.y * input.binormal) + (normalMap.z * input.normal);

  float4 color = float4(textureColor.rgb * Blend.rgb, textureColor.a);
  color.a *= Alpha;

  float3 lightDir = float3(0,0.2,1);

  return float4(color.rgb*saturate(dot(lightDir,normal)),color.a);
}