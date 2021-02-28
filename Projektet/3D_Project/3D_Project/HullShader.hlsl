
// Output patch constant data
struct HS_CONSTANT_DATA_OUTPUT
{
    float edgeTessFactor[3] : SV_TessFactor;		//4 for quad
    float insideTessFactor	: SV_InsideTessFactor;	//2 for quad
};

// Input
struct Vertex
{
    float4 Position		: SV_POSITION;
    float3 NormalWS		: NORMALWS;
    float2 TexCoord		: TEXCOORD;
    float4 PositionWS	: POSITIONWS;
};

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT ConstantPatchFunction(InputPatch<Vertex, 3> inputPatch, uint patchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT output;
	
    for (int i = 0; i < 3; i++)
    {
        output.edgeTessFactor[i] = 16.0f;	//Hardcoded for now. Could be changed depending on distance
    }
    output.insideTessFactor = 16.0f;
	
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