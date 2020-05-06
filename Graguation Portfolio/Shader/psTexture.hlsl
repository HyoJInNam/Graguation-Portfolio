struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
	float4 sampleColor = objTexture.Sample(objSamplerState, input.texCoord);
	return sampleColor;
}