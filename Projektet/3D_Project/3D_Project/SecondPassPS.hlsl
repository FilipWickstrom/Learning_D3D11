#define NROFLIGHTS 5

//GBuffers
Texture2D positionTexture   : register(t0);
Texture2D colourTexture     : register(t1);     //aka albedo?
Texture2D normalTexture     : register(t2);
Texture2D ambient           : register(t3);
Texture2D diffuse           : register(t4);
Texture2D specular          : register(t5);

//Anisotropic sampler
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
    //Smoothstep(min, max, interpolated value)
    //if distance is less than min: return 0                Total return: 1.0f
    //if distance is greateer than max: return 1            Total return: 0.0f;
    //if distance is in range of min and max: return 0-1    Total return: 0.0f - 1.0f;
    return 1.0f - smoothstep(range * 0.65, range, distance);   
}

float GetDiffuse(float3 lightVector, float3 normal)
{
    //The amount of diffuse depends on the "angle" between the two vectors
    return max(dot(lightVector, normal), 0.0f);
}

float GetSpecular(float3 lightVector, float3 normal, float3 viewVector, float shininess) //materials shiniess later!***
{
    //The vector that gets reflected 
    float3 reflectedVector = normalize(reflect(-lightVector, normal));
    
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
    
    //FUNKAR NOG INTE HELT MED .XYZ. SÅ FÅR SPARA FÖRST OCH SAMPLA SEN?***
    //float3 G_Position = positionTexture.xyz.Sample(theSampler, input.TexCoord);
    //float3 G_Normal = normalTexture.xyz.Sample(theSampler, input.TexCoord);
    //float3 G_TextureColour = colourTexture.xyz.Sample(theSampler, input.TexCoord);
    
    //Material - move inside of lights on
    //float3 G_Diffuse = diffuseTexture.xyz.Sample(theSampler, input.TexCoord);
    //float3 G_Specular = specularTexture.xyz.Sample(theSampler, input.TexCoord);
    //float G_Shininess = specularTexutre.w.Sample(theSampler, input.TexCoord);
    float G_Shiniess = specular.Sample(theSampler,input.TexCoord).w;
    
    //FIXA DENNA DELEN IMORGON!
    
    if (lightsOn)
    {
        //GET MATERIAL INFORMATION - DIFFUSECOLOUR, SPECULARCOLOUR, SPECULARPOWER
        float4 ambientIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f); //0% ambient light for now. had 10%
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        uint shininess = G_Shiniess; //16;
        
        //Goes through all the lights
        for (int i = 0; i < NROFLIGHTS; i++)
        {
            //Vector from the point to the light
            float3 lightVector = pointlights[i].position.xyz - G_Position.xyz;
            float distance = length(lightVector);
            lightVector = normalize(lightVector);
            
            //Fallof of the light
            float attenuationFactor = GetAttenuation(distance, pointlights[i].range);
    
            diffuseIntensity += GetDiffuse(lightVector, G_Normal.xyz) * attenuationFactor * pointlights[i].colour;
            
            float3 viewVector = normalize(camPos - G_Position.xyz);
            
            specularIntensity += GetSpecular(lightVector, G_Normal.xyz, viewVector, shininess) * attenuationFactor * pointlights[i].colour;
            
        }
        
        return G_Texture * (ambientIntensity + diffuseIntensity + specularIntensity);
    }
    
    //Just render the colours of the models
    else
    {
        return G_Texture;   
    }
}