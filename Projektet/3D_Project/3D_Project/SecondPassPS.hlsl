#define NROFLIGHTS 3

//GBuffers
Texture2D positionTexture   : register(t0);
Texture2D colourTexture     : register(t1);
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
    uint renderMode;
}

cbuffer SpotLight : register(b2)
{
    float3 s_position;
    float s_range;
    float3 s_direction; 
    float s_exponent;
    float3 s_colour;
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

float GetSpecular(float3 lightVector, float3 normal, float3 viewVector, float shininess)
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
    //With phong shading
    if (renderMode == 1)
    {
        //Get the gbuffers
        float4 G_Texture = colourTexture.Sample(theSampler, input.TexCoord);
        float3 G_Position = positionTexture.Sample(theSampler, input.TexCoord).xyz;
        float3 G_Normal = normalTexture.Sample(theSampler, input.TexCoord).xyz;
        
        //Material
        float4 M_Ambient = float4(ambient.Sample(theSampler, input.TexCoord).xyz, 1.0f);
        float4 M_Diffuse = float4(diffuse.Sample(theSampler, input.TexCoord).xyz, 1.0f);
        float4 G_Specular = specular.Sample(theSampler, input.TexCoord);
        float4 M_Specular = float4(G_Specular.xyz, 1.0f);
        float M_Shininess = G_Specular.w;
        
        //For final colour
        float4 ambientIntensity = float4(0.2f, 0.2f, 0.2f, 0.0f) * M_Ambient; //%20 ambient light
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        /*------ Spotlight -------*/
        float3 lightVector = s_position - G_Position;
        float distance = length(lightVector);
        lightVector = normalize(lightVector);
        
        //Angle between the two vectors. Can be between 1.0f to 0.0f
        float angle = max(dot(-lightVector, normalize(s_direction)), 0.0f);
        //Smooth out and make less sharp
        float smooth = pow(angle, s_exponent);
        //Attenuation depending on distance away and the smoothness
        float totalAtt = GetAttenuation(distance, s_range) * smooth;
       
        diffuseIntensity += GetDiffuse(lightVector, G_Normal) * float4(s_colour, 1.0f) * M_Diffuse * totalAtt;
        float3 viewVector = normalize(camPos - G_Position);
        specularIntensity += GetSpecular(lightVector, G_Normal, viewVector, M_Shininess) * float4(s_colour, 1.0f) * M_Specular * totalAtt;
        /*----- Spotlight end ----- */
        
        //Goes through all the pointlights
        for (int i = 0; i < NROFLIGHTS; i++)
        {
            //Vector from the point to the light
            float3 lightVector = pointlights[i].position.xyz - G_Position;
            float distance = length(lightVector);
            lightVector = normalize(lightVector);
            
            //Falloff for the light
            float attenuationFactor = GetAttenuation(distance, pointlights[i].range);
    
            diffuseIntensity += GetDiffuse(lightVector, G_Normal) * attenuationFactor * pointlights[i].colour * M_Diffuse;
            
            float3 viewVector = normalize(camPos - G_Position);
            
            specularIntensity += GetSpecular(lightVector, G_Normal, viewVector, M_Shininess) * attenuationFactor * pointlights[i].colour * M_Specular;
        }
        
        return G_Texture * (ambientIntensity + diffuseIntensity) + specularIntensity;
    }
    
    //Show only normals
    else if (renderMode == 2)
    {
        return normalTexture.Sample(theSampler, input.TexCoord);
    }
    
    //Position in the world
    else if (renderMode == 3)
    {
        return positionTexture.Sample(theSampler, input.TexCoord);
    }
    
    // Rendermode 0, or something else
    // Just render the colours of the models 
    else 
    {
        return colourTexture.Sample(theSampler, input.TexCoord);
    }
}