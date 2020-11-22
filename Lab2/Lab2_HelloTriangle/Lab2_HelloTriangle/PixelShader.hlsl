struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 colour : COLOUR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.colour, 1.0f);
}