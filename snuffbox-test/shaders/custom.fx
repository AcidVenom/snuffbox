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
  float4 unmodified : POSITION;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
};

float4 GetVertexPos(float4 position)
{
  float4 pos = float4(position.x+position.z/20*position.z/20*sin(Time/400),position.y+sin(position.z/20+Time/100)*20+sin(position.x/10 + Time/100)*13+cos(position.z/5)*10,position.z + sin(position.x/3), position.w);
  return pos;
}

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
  VOut output;
  output.unmodified = position;
  output.position = mul(GetVertexPos(position), WorldViewProjection);
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

  float3 A = GetVertexPos(input.unmodified).xyz;
  float3 B = GetVertexPos(float4(input.unmodified.x+1,input.unmodified.y,input.unmodified.zw)).xyz;
  float3 C = GetVertexPos(float4(input.unmodified.x+1,input.unmodified.y,input.unmodified.z+1,input.unmodified.w)).xyz;
  
  float3 normal = normalize(cross(B-A,B-C));
  color.r *= input.texcoord.x+sin(Time/1000);

  color.g *= abs(sin(Time/200));
  return float4(color.rgb*saturate(dot(normal,float3(0.5,1,0))),color.a);
}