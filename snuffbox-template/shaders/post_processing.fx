cbuffer ConstantBuffer : register(b0)
{
	float Time;
}

cbuffer Uniforms : register(b1)
{

}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float2 texcoord : TEXCOORD0;
};

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 colour : COLOUR)
{
	VOut output;
	output.position = position;
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D tex2D;
SamplerState SampleType;

float4 PS(VOut input) : SV_TARGET
{
	float4 colour = tex2D.Sample(SampleType, input.texcoord);
	return colour;
}