Texture2D aTexture : register(t0);
SamplerState aSampler;

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(aTexture.Sample(aSampler, input.uv).xyz, 1.0f);
}