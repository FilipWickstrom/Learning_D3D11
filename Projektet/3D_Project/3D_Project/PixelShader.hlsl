//TIPS KOLLA I BOKEN PÅ SIDA 507

//Texture2D gBuffColour;
//Texture2D gBuffNormal;
//Texture2D gBuffPosition;

Texture2D theTexture : register (t0); 
SamplerState AnisoSampler : register (s0);

//cbuffer Lightning		//Once per fragment
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
	float4 Position		: SV_POSITION;
    float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
    //float3 PositionWS	: POSITIONWS;
};

//float3 DoLight(position, normal, color, specular, lightPos, lightColour, lightRange)
//	*Follow phong lightning
//	return shading;


float4 main(PS_INPUT input) : SV_TARGET
{
	//FOR EACH FRAGMENT TOUCHED - READ G-BUFFERS - COMPUTE SHADING
	//	read g-buffer
	//	for each light that is touching the pizel
	//		compute shading
	//		accumulate
	//	write frame buffer
	
	
	//FIX PHONG LIGHTNING
	//Loop through all the lights
	
	//Sample diffuse map - diffuse and texture coordinate
	
	//Normalisera normalen
	
	//
    float4 output = float4(theTexture.Sample(AnisoSampler, input.TexCoord).xyz, 1.0f);

    return output;
}