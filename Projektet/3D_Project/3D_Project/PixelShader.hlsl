//TIPS KOLLA I BOKEN PÅ SIDA 507

//Texture2D texture : register (t0); 
//SamplerState sampler : register (so);	//anisotropisk sampler?

//cbuffer Lightning
//{
//	//lightPosition[] vec3/float3
//	//lightColor[] vec3/float3
//	//lightRange[] float
//};

//cbuffer Material
//{
//	//float3 specular albedo
//	//float specular power, shininess
//};

struct PS_INPUT
{
	float4 position : SV_Position;
    float3 colour : COLOUR;
	float3 normal : NORMAL;
	//float2 uv : UV;
	//float3 pixelInWorld???;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	//FIX PHONG LIGHTNING
	//Loop through all the lights
	
	//Sample diffuse map - diffuse and texture coordinate
	
	//Normalisera normalen
	
	//
	
	//return PS_OUTPUT ()
    return float4(input.colour, 1.0f);
}