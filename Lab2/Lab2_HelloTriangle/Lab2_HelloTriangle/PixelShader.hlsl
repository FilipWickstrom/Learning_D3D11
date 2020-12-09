Texture2D aTexture: register(t0);
SamplerState aSampler;

cbuffer LIGHTBUFFER
{
	//float4 ambientColour;
	//float4 diffuseColour;
	//float3 lightDirection;
	
	
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	//float3 normal : normal	
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	//float3 ambientIntensity = (0.1f, 0.1f, 0.1f);
	
	//textureColour = float4(aTexture.Sample(aSampler, input.uv).xyz, 1.0f);
	//float4 ambient = textureColour * ambientColour;
	//go through all the lights
		//diffuse
		//specular
	
	//Final
	//return float4(ambient + diffuse + specular);
	
	return float4(aTexture.Sample(aSampler, input.uv).xyz, 1.0f);
}