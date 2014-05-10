cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 WorldViewProjection;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VS(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = mul(position,WorldViewProjection);
    output.color = color;

    return output;
}


float4 PS(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}