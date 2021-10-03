
cbuffer TessellSettings : register(b0)
{
    float level;
    float depth;
    float2 padding;
};

// Output patch constant data
struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[3] : SV_TessFactor;		//4 for quad
    float insideTessFactor	: SV_InsideTessFactor;	//2 for quad
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

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT ConstantPatchFunction(InputPatch<Vertex, 3> inputPatch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
	
    [unroll]
    for (int i = 0; i < 3; i++)
    {
        output.edgeTessFactor[i] = level;
    }
    output.insideTessFactor = level;
	
    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantPatchFunction")]

Vertex PerPatchFunction(InputPatch<Vertex, 3> inputPatch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    return inputPatch[i];
}