#define DISPLACELEVEL 0.5f;

Texture2D displacementmap : register(t0);
SamplerState anisoSampler : register(s0);

cbuffer WVPMatrix : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

struct DomainShaderOutput
{
    float4 Position     : SV_POSITION;
    float3 NormalWS     : NORMALWS;
    float2 TexCoord     : TEXCOORD;
    float4 PositionWS   : POSITIONWS;
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
    float4 Position     : SV_POSITION;
    float3 NormalWS     : NORMALWS;
    float2 TexCoord     : TEXCOORD;
    float4 PositionWS   : POSITIONWS;
};

[domain("tri")]
DomainShaderOutput main(HS_CONSTANT_DATA_OUTPUT input, float3 uvw : SV_DomainLocation, const OutputPatch<Vertex, 3> patch)
{
    DomainShaderOutput output;
    
    //Sample a texcoordinate from the 3 patches
    output.NormalWS = patch[0].NormalWS * uvw.x + patch[1].NormalWS * uvw.y + patch[2].NormalWS * uvw.z;
    output.TexCoord = patch[0].TexCoord * uvw.x + patch[1].TexCoord * uvw.y + patch[2].TexCoord * uvw.z;
    output.PositionWS = patch[0].PositionWS * uvw.x + patch[1].PositionWS * uvw.y + patch[2].PositionWS * uvw.z;
    
    //The amount of displacement from the map
    float4 displacement = displacementmap.SampleLevel(anisoSampler, output.TexCoord, 0.0) * DISPLACELEVEL;
    output.PositionWS += (float4(output.NormalWS, 0.0f) * displacement);
    
    //Convert the position back to screenspace
    float4x4 clipspace = mul(View, Projection);
    output.Position = mul(output.PositionWS, clipspace);
    
    return output;
}