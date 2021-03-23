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
    Light C_Pointlights[NROFLIGHTS];
};

cbuffer Camera : register(b1)
{
    float3 C_CamPos;
    uint C_RenderMode;
}

cbuffer SpotLight : register(b2)
{
    float3 C_SpotPos;
    float C_SpotRange;
    float3 C_SpotDir;
    float C_SpotOutAngle;
    float3 C_SpotColour;
    float C_SpotInAngle;
}

cbuffer ShadowWVP : register(b3)
{
    row_major float4x4 C_SpotWorld;
    row_major float4x4 C_SpotView;
    row_major float4x4 C_SpotProjection;
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

float CalcAttenuation(float distance, float range)
{
    //Smoothstep(min, max, interpolated value)
    //if distance is less than min: return 0                Total return: 1.0f
    //if distance is greateer than max: return 1            Total return: 0.0f;
    //if distance is in range of min and max: return 0-1    Total return: 0.0f - 1.0f;
    return 1.0f - smoothstep(range * 0.65, range, distance);   
}

float CalcDiffuse(float3 lightVector, float3 normal)
{
    //The amount of diffuse depends on the "angle" between the two vectors
    return max(dot(lightVector, normal), 0.0f);
}

float CalcSpecular(float3 lightVector, float3 normal, float3 viewVector, float shininess)
{
    //The vector that gets reflected 
    float3 reflectedVector = normalize(reflect(-lightVector, normal));
    
    //The percentage between the two vectors
    float reflectDotView = max(dot(reflectedVector, viewVector), 0.0f);
    
    //Times it with the shiniess to make it even brighter
    return pow(reflectDotView, shininess);
}

float CalcShadow(float3 posWS)
{
    float shadowValue = 1.0f;
    
    //From the shadowmap/spotlight perspective
    float4x4 lightViewProjMatrix = mul(C_SpotView, C_SpotProjection);

    //Get the clipspace coordinate from spotlight
    float4 lightViewPos = mul(float4(posWS, 1.0f), lightViewProjMatrix);

    //Perspective divide:
    //"The further away something is, the more it will be pulled towards the center of the screen"
    //https://www.learnopengles.com/tag/perspective-divide/
    float3 shadowMapCoords = lightViewPos.xyz / lightViewPos.w;
    
    //Adjust from [-1, 1] clipspace to [0, 1] texturecoordinates
    shadowMapCoords.x = 0.5f * shadowMapCoords.x + 0.5f;
    shadowMapCoords.y = -0.5f * shadowMapCoords.y + 0.5f;
     
    //Inside of [0, 1] space, otherwise ignore not visible in the lights view
    //Only accepts values inside the shadowmap
    if (saturate(shadowMapCoords.x) == shadowMapCoords.x &&
        saturate(shadowMapCoords.y) == shadowMapCoords.y)
    {
        //Fixed bias that works in this scene
        float bias = 0.01f;
        
        //The depth after transforming to lights space
        float currentDepth = shadowMapCoords.z - bias;

        //Depth that is in the shadowmap
        float shadowMapDepth = shadowMap.Sample(anisoSampler, shadowMapCoords.xy).r;
        
        //Something else is infront - This pixel is in shadow
        if (shadowMapDepth < currentDepth)
        {
            shadowValue = 0.0f;
        }
    }
    return shadowValue;
}

float CalcSpotAttenuation(float3 posWS)
{ 
    float attenuation = 0.0f;
    
    //Vector from spotlights position to the focuspoint
    float3 lightDir = normalize(C_SpotDir);
    
    //Vector from spotlights position to current pixels position
    float3 spotToPosWS = posWS - C_SpotPos;
    float distance = length(spotToPosWS);
    spotToPosWS = normalize(spotToPosWS);
    float currentAngle = dot(lightDir, spotToPosWS);
   
    //Inside of the big cone
    if (currentAngle > C_SpotOutAngle)
    {
        //Can be between 0.0f and 1.0f
        float intensity = clamp((currentAngle - C_SpotOutAngle) / (C_SpotOutAngle - C_SpotInAngle), 0.0f, 1.0f);
        
        //Falloff depending on how far away from the light
        float distanceFalloff = CalcAttenuation(distance, C_SpotRange);
        
        //Total attenuation
        attenuation = intensity * distanceFalloff;
    }
    return attenuation;
}

/*----- Help functions: END ------*/

float4 main( PixelInput input ) : SV_TARGET
{
    //With phong shading
    if (C_RenderMode == 1)
    {
        //Getting what is saved in gbuffers
        GBuffer gbuffer = GetGBuffer(input.TexCoord);
        Material material = GetMaterial(input.TexCoord);
        
        //Total colour values of intensity
        //20% ambient light
        float4 ambientIntensity = float4(0.2f, 0.2f, 0.2f, 0.0f) * material.ambient;
        float4 diffuseIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 specularIntensity = float4(0.0f, 0.0f, 0.0f, 0.0f);
        
        //Calculates shadow. Has shadow: 0.0f, Has not: 1.0f, Between 0.0f - 1.0f for some shadow
        float shadow = CalcShadow(gbuffer.positionWS);
        
        /* - - - - - - - Spotlight - - - - - - - */
        float spotAtt = CalcSpotAttenuation(gbuffer.positionWS);
       
        //Shadow only affects on the spotlight
        float3 lightVector = normalize(C_SpotPos - gbuffer.positionWS);
        
        diffuseIntensity += CalcDiffuse(lightVector, gbuffer.normalWS) * 
                            material.diffuse *
                            float4(C_SpotColour, 1.0f) * 
                            spotAtt * 
                            shadow;
        
        float3 viewVector = normalize(C_CamPos - gbuffer.positionWS);
        specularIntensity += CalcSpecular(lightVector, gbuffer.normalWS, viewVector, material.shininess) * 
                             material.specular *
                             float4(C_SpotColour, 1.0f) *
                             spotAtt * 
                             shadow;
        
        /* - - - - - - - Point lights - - - - - - - */
        for (int i = 0; i < NROFLIGHTS; i++)
        {
            //Vector from the point to the light
            float3 pointlightVector = C_Pointlights[i].position.xyz - gbuffer.positionWS;
            float distance = length(pointlightVector);
            pointlightVector = normalize(pointlightVector);
            
            //Falloff from the light
            float attenuationFactor = CalcAttenuation(distance, C_Pointlights[i].range);
    
            diffuseIntensity += CalcDiffuse(pointlightVector, gbuffer.normalWS) *
                                C_Pointlights[i].colour * 
                                attenuationFactor *
                                material.diffuse;
            
            float3 viewVector = normalize(C_CamPos - gbuffer.positionWS);
            
            specularIntensity += CalcSpecular(pointlightVector, gbuffer.normalWS, viewVector, material.shininess) *
                                 C_Pointlights[i].colour * 
                                 attenuationFactor * 
                                 material.specular;
        }
        
        //Final colour calculation
        return gbuffer.colourTexture * (ambientIntensity + diffuseIntensity) + specularIntensity;
    }
    
    //Show only normals
    else if (C_RenderMode == 2)
    {
        return normalTexture.Sample(anisoSampler, input.TexCoord);
    }
    
    //Position in the world
    else if (C_RenderMode == 3)
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