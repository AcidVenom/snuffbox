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
	float shift = input.shift;
	float x = (input.texcoord.x * AnimationCoords.z) + AnimationCoords.x;
	float y = (input.texcoord.y * AnimationCoords.w) + AnimationCoords.y;

	float3 font_atlas_vector = float3(1.0 / 2048, 1.0 / 2048, 4);

	float2 coords = float2(x, y);

	float4 current = textures[0].Sample(Sampler, coords);
	float4 previous = textures[0].Sample(Sampler, coords + float2(-1, 0) * font_atlas_vector.xy);
	float4 next = textures[0].Sample(Sampler, coords + float2(1, 0) * font_atlas_vector.xy);

	float r = current.r;
	float g = current.g;
	float b = current.b;

	if (shift <= 0.333)
	{
		float z = shift / 0.333;
		r = lerp(current.r, previous.b, z);
		g = lerp(current.g, current.r, z);
		b = lerp(current.b, current.g, z);
	}
	else if (shift <= 0.666)
	{
		float z = (shift - 0.33) / 0.333;
		r = lerp(previous.b, previous.g, z);
		g = lerp(current.r, previous.b, z);
		b = lerp(current.g, current.r, z);
	}
	else if (shift < 1.0)
	{
		float z = (shift - 0.66) / 0.334;
		r = lerp(previous.g, previous.r, z);
		g = lerp(previous.b, previous.g, z);
		b = lerp(current.r, previous.b, z);
	}

	float t = max(max(r, g), b);
	float4 colour = float4(1, 1, 1, (r + g + b) / 3.0);

	float alpha = colour.a * input.colour.a * Alpha;
	return float4(colour.rgb * input.colour.rgb * Blend, alpha);
}