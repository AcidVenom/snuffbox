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
	float4 AnimationCoords;
}

cbuffer Uniforms : register(b1)
{

}

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOUR;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD0;
	float shift : TEXCOORD1;
};

VOut VS(float4 position : POSITION, float3 normal : NORMAL, float2 texcoord : TEXCOORD0, float4 colour : COLOUR)
{
	VOut output;
	output.shift = position.x - floor(position.x);
	position.x = floor(position.x);
	output.position = mul(position, WorldViewProjection);
	output.normal = normalize(mul(float4(normal, 0), InvWorld).xyz);
	output.texcoord = texcoord;
	output.colour = colour;
	return output;
}

Texture2D textures[1];
SamplerState Sampler;

float4 PS(VOut input) : SV_TARGET
{
	float x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;
	float y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;

	float2 coords = float2(x, y);

	float4 base = textures[0].Sample(Sampler, coords);
	float alpha = ((base.r + base.g + base.b) / 3.0) * input.colour.a * Alpha;

	return float4(Blend * input.colour.rgb * base.rgb * alpha, alpha);
}