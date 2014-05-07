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

    output.position = mul(float4(position.x,position.y+sin(position.x*position.x*10+Time/10)/2,position.z,position.w), WorldViewProjection);
    output.color = color;

    return output;
}


float4 PS(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return float4(-0.5+abs(sin(position.x/100))+abs(sin(Time/100)),-0.5+abs(cos(position.x/100))+abs(sin(Time/100)),-0.5+abs(sin(position.x/100))+abs(sin(Time/100)),1);
}