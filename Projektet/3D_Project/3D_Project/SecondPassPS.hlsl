//GBuffers
Texture2D positionTexture   : register(t0);
Texture2D colourTexture     : register(t1);     //aka albedo?
Texture2D normalTexture     : register(t2);

SamplerState theSampler : register(s0);

cbuffer Lights : register(b0)
{
    float3 lightPos;
    float padding;
    float4 lightColour;
    float3 camPos;
    float lightrange;
};


struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

//DO Diffuse
/*float DoDiffuse(float4 lightColour, float4 lightPos, float4 normal)
{
    normal = normalize(normal);
    float diffuseFactor = max(dot(normal, lightPos

}*/

//Do Specular


float4 main( PixelInput input ) : SV_TARGET
{
    //DEBUG MODE
    bool lightsOn = false;
    
    //Get the gbuffers
    float4 G_Position = positionTexture.Sample(theSampler, input.TexCoord);
    float4 G_Texture = colourTexture.Sample(theSampler, input.TexCoord);
    float4 G_Normal = normalTexture.Sample(theSampler, input.TexCoord);
    
    
    if (lightsOn)
    {
        //Default - SHOULD LATER BE DEFINED BY THE INPUTFILE
        float4 ambientIntensity = float4(0.1f, 0.1f, 0.1f, 1.0f); //10% ambient light
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 1.0f);
        uint shininess = 64;
    
        float3 pixelToLight = lightPos - G_Position.xyz;
        float distance = length(pixelToLight);
        if (distance < lightrange)
        {
            float attenuationFactor = max(0.0f, 1.0f - (distance / lightrange));
    
            //Diffuse - Can be minimum 0.0f. Dot part = cos(angle)
            float diffuseFactor = max(dot(G_Normal.xyz, normalize(pixelToLight)), 0.0f);
            diffuseIntensity = lightColour * attenuationFactor * diffuseFactor;
    
            //Specular - Can be minimum 0.0f
            float3 reflectedLight = normalize(reflect(-pixelToLight, G_Normal.xyz));
            float3 pixelToCamera = normalize(camPos - G_Position.xyz);
            float specularFactor = pow(max(dot(reflectedLight, pixelToCamera), 0.0f), shininess);
            specularIntensity = lightColour * attenuationFactor * specularFactor;
        }
     
        return G_Texture * (ambientIntensity + diffuseIntensity) + specularIntensity;
    }
    
    //Just render the colours of the models
    else
    {
        return G_Texture;   
    }
}