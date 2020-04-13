#pragma pack_matrix( row_major )

cbuffer perObjectBuffer  : register(b0)
{
	float4x4 wvpMatrix;
	float4x4 worldMatrix;
};

struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal : NORMAL;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(float4(input.position, 1.0f), wvpMatrix);
	output.texCoord = input.texCoord;
	output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix));
	return output;
};