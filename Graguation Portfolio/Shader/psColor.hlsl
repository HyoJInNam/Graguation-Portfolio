cbuffer GradientBuffer
{
	float4 apexColor;
	float4 centerColor;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 domePosition : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	float height = input.domePosition.y;

	if (height < 0.0)
	{
		height = 0.0f;
	}
	return lerp(centerColor, apexColor, height);
}