Texture2D aTexture : register(t0); //t: texturebuffer register 0
SamplerState aSampler : register(s0); //s: sampler register 0

cbuffer constBufLight : register(b0)
{
    float3 lightPos;
    float padding0;
    float3 lightColour;
    float lightRange;
    float3 cameraPos;
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
    float4 ambientIntensity = float4(0.1f, 0.1f, 0.1f, 1.0f);   //10% ambient light
    float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
    uint shininess = 64;
    
    //Texture
    float4 textureColour = float4(aTexture.Sample(aSampler, input.uv).xyz, 1.0f);
      
    //Vector from the positioned light to pixels location in the world. Calculates distance
    float3 lightPosToPixel = lightPos - input.pixelPosWorld;
    float distance = length(lightPosToPixel);
    
    //Only let the light shine on what is in the range of the pointlight
    if (distance < lightRange)
    {
        //Attenuation - How strong the light is based on distance from the light source
        float attenuationFactor = max(0.0f, 1.0f - (distance / lightRange)); //From Practical rendering... s.505
        
        //Diffuse - Can be minimum 0.0f. Dot part = cos(angle)
        float diffuseFactor = max(dot(normalize(input.normal), normalize(lightPosToPixel)), 0.0f);
        diffuseIntensity = float4(lightColour, 1.0f) * attenuationFactor * diffuseFactor;
    
        //Specular - Can be minimum 0.0f
        float3 reflectDirVec = normalize(reflect(lightPosToPixel, input.normal));
        float3 pixelToCamera = normalize(input.pixelPosWorld - cameraPos); 
        float specularFactor = pow(max(dot(reflectDirVec, pixelToCamera), 0.0f), shininess);
        specularIntensity = float4(lightColour, 1.0f) * attenuationFactor * specularFactor;
    }
    
    //Final colour    
    return textureColour * (ambientIntensity + diffuseIntensity) + specularIntensity;
}