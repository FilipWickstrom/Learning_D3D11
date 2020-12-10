Texture2D aTexture : register(t0); //t: texturebuffer register 0
SamplerState aSampler : register(s0); //s: sampler register 0

cbuffer constBufLight : register(b0)
{
    float3 lightPos; //In world space
    float padding0;
    float3 lightColor;
    float lightRange;
    float3 cameraPos; //converted to worldspace    
    float padding1;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
    float3 normal : NORMAL;
    float3 pixelPosWorld : PIXELPOSWORLD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{	
    //Default
    float4 ambientIntensity = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
	//Ambient
    //float ambientConst = 1.0f;	//ka
    //float3 ambientColour = float3(1.0f, 1.0f, 1.0f);
    //ambientIntensity = ambientConst * float4(ambientColour, 1.0f);
	
    //Diffuse
    float3 diffuseColour = float3(1.0f, 1.0f, 1.0f);
    float3 lightVector = normalize(lightPos - input.pixelPosWorld);
    diffuseIntensity = float4(max(dot(input.normal, lightVector), 0) * diffuseColour, 1.0f);
     
    //Specular 
    float3 lightToPosDirVec = normalize(lightPos - input.pixelPosWorld);
    float3 reflectDirVec = normalize(reflect(lightToPosDirVec, input.normal));
    float3 posToViewDirVec = normalize(input.pixelPosWorld - cameraPos);
    float specularConstant = pow(max(dot(posToViewDirVec, reflectDirVec), 0), 64);
    specularIntensity = float4(1.0f, 1.0f, 1.0f, 1.0f) * specularConstant;
    
    //Attenuation
    
    //Texture
	float4 textureColour = float4(aTexture.Sample(aSampler, input.uv).xyz, 1.0f);
	
    return textureColour * (diffuseIntensity + ambientIntensity) + specularIntensity;
     
    //Kolla i boken med frank luna sida 254 och 271
    //LÄGGA TILL LJUSET FRÅN LAMPAN
}