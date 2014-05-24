cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 WorldViewProjection;
	float4 CamPos;
}

struct VOut
{
  float4 position : SV_POSITION;
  float4 unmodified : POSITION;
  float4 color : COLOR;
  float3 normal : NORMAL;
  float2 texcoord : TEXCOORD0;
};

float4 GetVertexPos(float4 position)
{
	float4 wave = float4(position.x,position.y+sin(position.x/10+Time/50)*4+cos(position.z/10)*5+sin(Time/50)*10,position.z,position.w);

	return float4(wave.x,wave.y*(sin(wave.x*3.14/180)*wave.z/100),wave.z,wave.w);
}

VOut VS(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL, float2 texcoord : TEXCOORD0)
{
  VOut output;
  output.position = mul(GetVertexPos(position), WorldViewProjection);
  output.color = color;
	output.normal = normal;
	output.unmodified = position;
	output.texcoord = texcoord;

  return output;
}

Texture2D Texture;
SamplerState SampleType;

float4 PS(VOut input) : SV_TARGET
{	
	float4 position = input.unmodified;
	float4 A = GetVertexPos(position);
  float4 C = GetVertexPos(float4(position.x+0.5,position.y,position.z+sqrt(0.75),position.w));
  float4 B = GetVertexPos(float4(position.x+1,position.y,position.z,position.w));

  float diffuseIntensity = 0.8;
  float3 diffuseColor = float3(1.0,1.0,1.0);

  float3 diffuse = diffuseIntensity * diffuseColor;
  float3 light = normalize(float3(1.0,1.0,0.2));
  float specIntens = 4;
  float3 normal = cross(C.xyz - A.xyz, B.xyz - A.xyz);
  normalize(normal);
  float4 camView = CamPos - mul(position,World);
  float3 half = normalize(normalize(light*-1) + camView.xyz); 
	float specular = pow(saturate(dot(normal,half)),specIntens)*1.5;
	float4 specColor = float4(0.8,0.7,0.6,1);
  float4 textureColor = Texture.Sample(SampleType, input.texcoord);

	return (textureColor * input.color) * float4(diffuse,1) * saturate(dot(light,normal)) + specular * specColor;
}