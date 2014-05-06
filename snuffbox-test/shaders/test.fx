cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VOut VS(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = float4(position.x,position.y+sin((position.x*position.x*10)+Time/20)*0.4,position.z,1);
    output.color = color;

    return output;
}


float4 PS(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return float4(-0.5+abs(sin(position.x/100))+abs(sin(Time/100)),-0.5+abs(cos(position.x/100))+abs(sin(Time/100)),-0.5+abs(sin(position.x/100))+abs(sin(Time/100)),1);
}