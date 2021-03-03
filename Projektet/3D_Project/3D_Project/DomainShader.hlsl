Texture2D displacementmap : register(t0);
SamplerState anisoSampler : register(s0);

cbuffer WVPMatrix : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer TessellSettings : register(b1)
{
    float level;
    float depth;
    float2 padding;
};

struct DomainShaderOutput
{ 
    float4 PositionCS   : SV_POSITION;
    float4 PositionWS   : POSITIONWS;
    float2 TexCoord     : TEXCOORD;
    float3 NormalWS     : NORMALWS;
    float3 TangentWS    : TANGENTWS;
    float3 BiTangentWS  : BITANGENTWS;
};

// Output patch constant data
struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[3] : SV_TessFactor;
    float insideTessFactor  : SV_InsideTessFactor;
};

// Input
struct Vertex
{
    float4 PositionCS   : SV_POSITION;
    float4 PositionWS   : POSITIONWS;
    float2 TexCoord     : TEXCOORD;
    float3 NormalWS     : NORMALWS;
    float3 TangentWS    : TANGENTWS;
    float3 BiTangentWS  : BITANGENTWS;
};

[domain("tri")]
DomainShaderOutput main(HS_CONSTANT_DATA_OUTPUT input, float3 uvw : SV_DomainLocation, const OutputPatch<Vertex, 3> patch)
{
    DomainShaderOutput output;
    
    //Sample a texcoordinate from the 3 patches
    output.NormalWS = patch[0].NormalWS * uvw.x + patch[1].NormalWS * uvw.y + patch[2].NormalWS * uvw.z;
    output.TexCoord = patch[0].TexCoord * uvw.x + patch[1].TexCoord * uvw.y + patch[2].TexCoord * uvw.z;
    output.PositionWS = patch[0].PositionWS * uvw.x + patch[1].PositionWS * uvw.y + patch[2].PositionWS * uvw.z;
    output.TangentWS = patch[0].TangentWS * uvw.x + patch[1].TangentWS * uvw.y + patch[2].TangentWS * uvw.z;
    output.BiTangentWS = patch[0].BiTangentWS * uvw.x + patch[1].BiTangentWS * uvw.y + patch[2].BiTangentWS * uvw.z;
    
    //The amount of displacement from the map
    float4 displacement = displacementmap.SampleLevel(anisoSampler, output.TexCoord, 0.0) * depth;
    output.PositionWS += (float4(output.NormalWS, 0.0f) * displacement);
    
    //Convert the position back to screenspace
    float4x4 clipspace = mul(View, Projection);
    output.PositionCS = mul(output.PositionWS, clipspace);
       
    return output;
}