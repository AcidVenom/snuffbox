cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 WorldViewProjection;
	float4x4 World;
}

struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

struct DirectionalLight
{
    float4 color;
    float3 dir;
};

VOut VS(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL)
{
    VOut output;
	
	float4 newPos = float4(position.x,position.y+sin(position.x/10+Time/50)*10+cos(position.z/10+Time/50)*10,position.z,position.w);

    output.position = mul(newPos,WorldViewProjection);
    output.color = color;
	output.normal = mul(newPos,World);

    return output;
}


float4 PS(VOut input) : SV_TARGET
{
    input.normal = normalize(input.normal);
	
	float3 finalColor;

	finalColor = input.color * float4(0.1,0.2,0.4,1.0);
	finalColor += saturate(dot(float3(0.2,1.0,0.2), input.normal) * float4(0.3,0.3,0.8,1.0) * input.color);
	return float4(finalColor,input.color.a);
}