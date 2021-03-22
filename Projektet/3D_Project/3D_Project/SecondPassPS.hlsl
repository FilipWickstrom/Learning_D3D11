#define NROFLIGHTS 3

//GBuffers
Texture2D positionTexture   : register(t0);
Texture2D colourTexture     : register(t1);
Texture2D normalTexture     : register(t2);
Texture2D ambient           : register(t3);
Texture2D diffuse           : register(t4);
Texture2D specular          : register(t5);

//Shadowmap
Texture2D shadowMap         : register(t6);

//Anisotropic sampler
SamplerState anisoSampler : register(s0);
//
SamplerComparisonState shadowSampler : register(s1);    //remove later????

/*----- Constant buffers: START ------*/

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
    float s_resolution;
}

cbuffer ShadowWVP : register(b3)
{
    row_major float4x4 S_World;
    row_major float4x4 S_View;
    row_major float4x4 S_Projection;
};

/*----- Constant buffers: END ------*/

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct GBuffer
{
    float4 colourTexture;
    float3 positionWS;
    float3 normalWS;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float shininess;
};

/*----- Help functions: START ------*/

GBuffer GetGBuffer(float2 texCoord)
{
    GBuffer output;
    output.colourTexture = colourTexture.Sample(anisoSampler, texCoord);
    output.positionWS = positionTexture.Sample(anisoSampler, texCoord).xyz;
    output.normalWS = normalTexture.Sample(anisoSampler, texCoord).xyz;
    return output;
}

Material GetMaterial(float2 texCoord)
{
    Material output;
    output.ambient = float4(ambient.Sample(anisoSampler, texCoord).xyz, 1.0f);
    output.diffuse = float4(diffuse.Sample(anisoSampler, texCoord).xyz, 1.0f);
    float4 tempSpec = specular.Sample(anisoSampler, texCoord);
    output.specular = float4(tempSpec.xyz, 1.0f);
    output.shininess = tempSpec.w;
    return output;
}

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

float Shadow(float3 posWS, float3 normalWS, float3 lightPosWS)
{
    //From the shadowmap/spotlight perspective
    float4x4 lightViewProjMatrix = mul(S_View, S_Projection);

    //Get the clipspace coordinate from spotlight
    float4 lightViewPos = mul(float4(posWS, 1.0f), lightViewProjMatrix);

    //Perspective devide
    float3 shadowMapCoords = lightViewPos.xyz / lightViewPos.w;
    
    //Adjust from [-1, 1] clipspace to [0, 1] texturecoordinates
    shadowMapCoords.x = 0.5f * shadowMapCoords.x + 0.5f;
    shadowMapCoords.y = -0.5f * shadowMapCoords.y + 0.5f;
     
    //Inside of [0, 1] space, otherwise ignore not visible in the lights view
    //Only accepts values inside of the view of the light
    if (saturate(shadowMapCoords.x) == shadowMapCoords.x &&
        saturate(shadowMapCoords.y) == shadowMapCoords.y)
    {
        float3 lightDir = normalize(lightPosWS - posWS);
        normalWS = normalize(normalWS);
        float dotValue = dot(normalWS, lightDir);
        //float bias = 0.005f * tan(acos(dotValue));
        //bias = clamp(bias, 0, 0.01f);
        //float bias = 0.0001f * sqrt(1 - pow(dot(normalize(normalWS), -lightDir), 2) / dot(normalize(normalWS), -lightDir));
        
        float bias = 0.005f; //= 0.01f;   //1.0f / pow(2, 32);
        float currentDepth = shadowMapCoords.z - bias;
                
        //float shadowFactor = shadowMap.SampleCmpLevelZero(shadowSampler, shadowMapCoords.xy, currentDepth).r;
        //return shadowFactor;
        float shadowFactor = shadowMap.Sample(anisoSampler, shadowMapCoords.xy).r;
        //Is in shadow
        if (shadowFactor < currentDepth)
        {
            return 0.0f;
        }
        else
        {
            float lightIntensity = saturate(dotValue);
            return lightIntensity;
        }
    }
    return 1.0f;
}

//void CalcSpotlight(float4& diffuse, float4 specular)      //funkar inte med & calc intensity float?
//{ }

/*----- Help functions: END ------*/

float4 main( PixelInput input ) : SV_TARGET
{
    //With phong shading
    if (renderMode == 1)
    {
        //Getting what is saved in gbuffers
        GBuffer gbuffer = GetGBuffer(input.TexCoord);
        Material material = GetMaterial(input.TexCoord);
        
        //For final colour
        float4 ambientIntensity = float4(0.2f, 0.2f, 0.2f, 0.0f) * material.ambient; //%20 ambient light
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        //TESTING
        float shadow = Shadow(gbuffer.positionWS, gbuffer.normalWS, s_position);
        //If it has a shadow. STOP HERE. 
        //if (shadow == 0.0f)
        //{
        //    return gbuffer.colourTexture * ambientIntensity;
        //}
        
        
        //Else does not have a shadow
        
        
        /*------ Spotlight -------*/        //SEPERATE FUNCTION FOR THIS LATER???
        float3 lightVector = s_position - gbuffer.positionWS;
        float distance = length(lightVector);
        lightVector = normalize(lightVector);
        
            //Angle between the two vectors. Can be between 1.0f to 0.0f
        float angle = max(dot(-lightVector, normalize(s_direction)), 0.0f);
            //Smooth out and make less sharp
        float smooth = pow(angle, s_exponent);
            //Attenuation depending on distance away and the smoothness
        float totalAtt = GetAttenuation(distance, s_range) * smooth;
       
        diffuseIntensity += GetDiffuse(lightVector, gbuffer.normalWS) * float4(s_colour, 1.0f) * material.diffuse * totalAtt;
        float3 viewVector = normalize(camPos - gbuffer.positionWS);
        specularIntensity += GetSpecular(lightVector, gbuffer.normalWS, viewVector, material.shininess) * float4(s_colour, 1.0f) * material.specular * totalAtt;
        /*----- Spotlight end ----- */
        
        //Goes through all the pointlights
        for (int i = 0; i < NROFLIGHTS; i++)
        {
            //Vector from the point to the light
            float3 lightVector = pointlights[i].position.xyz - gbuffer.positionWS;
            float distance = length(lightVector);
            lightVector = normalize(lightVector);
            
            //Falloff for the light
            float attenuationFactor = GetAttenuation(distance, pointlights[i].range);
    
            diffuseIntensity += GetDiffuse(lightVector, gbuffer.normalWS) * attenuationFactor * pointlights[i].colour * material.diffuse;
            
            float3 viewVector = normalize(camPos - gbuffer.positionWS);
            
            specularIntensity += GetSpecular(lightVector, gbuffer.normalWS, viewVector, material.shininess) * 
                                 attenuationFactor * pointlights[i].colour * material.specular;
        }
        
        return gbuffer.colourTexture * (ambientIntensity + (diffuseIntensity * shadow)) + (specularIntensity * shadow);
        
    }
    
    //Show only normals
    else if (renderMode == 2)
    {
        return normalTexture.Sample(anisoSampler, input.TexCoord);
    }
    
    //Position in the world
    else if (renderMode == 3)
    {
        return positionTexture.Sample(anisoSampler, input.TexCoord);
    }
    
    // Rendermode 0, or something else
    // Just render the colours of the models 
    else
    {
        return colourTexture.Sample(anisoSampler, input.TexCoord);
    }
}