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
    
    output.Position = patch[0].Position * uvw.x + patch[1].Position * uvw.y + patch[2].Position * uvw.z;
    output.NormalWS = patch[0].NormalWS * uvw.x + patch[1].NormalWS * uvw.y + patch[2].NormalWS * uvw.z;
    output.TexCoord = patch[0].TexCoord * uvw.x + patch[1].TexCoord * uvw.y + patch[2].TexCoord * uvw.z;
    output.PositionWS = patch[0].PositionWS * uvw.x + patch[1].PositionWS * uvw.y + patch[2].PositionWS * uvw.z;
    
    //TRANSFORMERA MED DISPLACEMENT MAP
    
    return output;
}