
#define NROFLIGHTS 2

//GBuffers
Texture2D positionTexture   : register(t0);
Texture2D colourTexture     : register(t1);     //aka albedo?
Texture2D normalTexture     : register(t2);

SamplerState theSampler : register(s0);

struct Light
{
    float4 position;
    float4 colour;
    float range;
    float3 padding;
};

cbuffer Lights : register(b0)
{
    Light pointlights[NROFLIGHTS];
};

cbuffer Camera : register(b1)
{
    float3 camPos;
    float padding;
}


struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float GetAttenuation(float distance, float range)
{
    //Can be minimum 0
    return max(1.0f - (distance / range), 0.0f);
    
    //REMOVE LATER. MORE BEING LIT 
    //return 1.0f - smoothstep(range * 0.75, range, distance);
}

float GetDiffuse(float4 lightVector, float4 normal)
{
    //The amount of diffuse
    return max(dot(lightVector, normal), 0.0f);
}

float GetSpecular(float4 lightVector, float4 normal, float3 viewVector, float shininess) //materials shiniess
{
    //The vector that gets reflected 
    float3 reflectedVector = normalize(reflect(-lightVector.xyz, normal.xyz));
    
    //The percentage between the two vectors
    float reflectDotView = max(dot(reflectedVector, viewVector), 0.0f);
    
    //Times it with the shiniess to make it even brighter
    return pow(reflectDotView, shininess);
}


float4 main( PixelInput input ) : SV_TARGET
{
    //DEBUG MODE
    bool lightsOn = true;
    
    //Get the gbuffers
    float4 G_Position = positionTexture.Sample(theSampler, input.TexCoord);
    float4 G_Texture = colourTexture.Sample(theSampler, input.TexCoord);
    float4 G_Normal = normalTexture.Sample(theSampler, input.TexCoord);
    
    if (lightsOn)
    {
        //GET MATERIAL INFORMATION - DIFFUSECOLOUR, SPECULARCOLOUR, SPECULARPOWER
        float4 ambientIntensity = float4(0.1f, 0.1f, 0.1f, 0.0f); //10% ambient light
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        uint shininess = 64;
        
        //Goes through all the lights
        for (int i = 0; i < NROFLIGHTS; i++)
        {
            //Vector from the point to the light
            float4 lightVector = pointlights[i].position - G_Position;
            float distance = length(lightVector);
           
           
            lightVector = normalize(lightVector);
            
            float attenuationFactor = GetAttenuation(distance, pointlights[i].range);
    
            diffuseIntensity += GetDiffuse(lightVector, G_Normal) * attenuationFactor * pointlights[i].colour;
            
            float3 viewVector = normalize(camPos - G_Position.xyz);
            
            specularIntensity += GetSpecular(lightVector, G_Normal, viewVector, shininess) * attenuationFactor * pointlights[i].colour;       
        }
         
        return G_Texture * (ambientIntensity + diffuseIntensity + specularIntensity);
    }
    
    //Just render the colours of the models
    else
    {
        return G_Texture;   
    }
}