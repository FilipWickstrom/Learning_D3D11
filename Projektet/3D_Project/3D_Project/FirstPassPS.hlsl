//Textures
Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);

//Sampler
SamplerState anisoSampler : register(s0);

//Cbuffer - updated for every object
cbuffer Material : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

cbuffer Settings : register(b1)
{
    uint useNormalMap;
}

//Input from the vertex shader in first pass
struct PixelInput
{
    float4 PositionCS   : SV_POSITION;
    float4 PositionWS   : POSITIONWS;
    float2 TexCoord     : TEXCOORD;
    float3 NormalWS     : NORMALWS;
    float3 TangentWS    : TANGENTWS;
    float3 BiTangentWS  : BITANGENTWS;
};

//Output to g-buffer to be used for second pass
struct PixelOutput
{
    float4 PositionWS   : SV_TARGET0;
    float4 Colour       : SV_Target1;
    float4 NormalWS     : SV_Target2;
    float4 Ambient      : SV_Target3;
    float4 Diffuse      : SV_Target4;
    float4 Specular     : SV_Target5;
};

PixelOutput main(PixelInput input) : SV_TARGET
{
    PixelOutput output;
    
    output.PositionWS = input.PositionWS;
    
    //Sampling the colour from the texture
    output.Colour = diffuseTexture.Sample(anisoSampler, input.TexCoord);
    
    //Use the normalmap
    if (useNormalMap == 1)
    {
        //Make TBN, the rotation matrix of this vertex - Normalize, could have been changed with interpolation
        float3x3 TBNmatrix = float3x3(normalize(input.TangentWS), 
                                      normalize(input.BiTangentWS), 
                                      normalize(input.NormalWS));
        
        float3 normalMap = normalTexture.Sample(anisoSampler, input.TexCoord).xyz;
        //Change range for normalmap, [0.0f, 1.0f] to [-1.0f, 1.0f]
        normalMap = (normalMap * 2.0f) - 1.0f;
        
        output.NormalWS = float4(mul(normalMap, TBNmatrix), 1.0f);
    }
    else
    {
        //Sends forward the normal to the g-buffer
        output.NormalWS = float4(input.NormalWS, 1.0f);
    }
    
    output.Ambient = ambient;
    output.Diffuse = diffuse;
    output.Specular = specular;
    
    return output;
}