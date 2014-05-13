cbuffer VS_CONSTANT_BUFFER : register(b0)
{
	float Time;
	float4x4 WorldViewProjection;
	float4x4 WorldView;
	float4x4 World;
}

struct VOut
{
  float4 position : SV_POSITION;
  float4 color : COLOR;
	float3 normal : NORMAL;
};

VOut VS(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL)
{
    VOut output;
    //output.position = mul(float4(position.x,position.y+sin(position.x/2+Time/10)*1,position.z,position.w),WorldViewProjection);
    output.position = mul(position,WorldViewProjection);
    output.color = color;
	output.normal = normal;
	
    return output;
}


float4 PS(VOut input) : SV_TARGET
{
	return input.color;
}