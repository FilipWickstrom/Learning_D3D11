
cbuffer WVPMatrix : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

//Input from the input data from OBJ-files
struct VertexInput
{
    float3 Position     : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

//Output that is going to be used in the first pass pixel shader
struct VertexOutput
{
    float4 Position     : SV_POSITION;
    float3 NormalWS     : NORMALWS; 
    float2 TexCoord     : TEXCOORD;
    float4 PositionWS   : POSITIONWS;
};

VertexOutput main(const VertexInput input)
{
	VertexOutput output;
    
    //World * View * Projection
    float4x4 WVP = mul(mul(World, View), Projection);
    
    //Position of the vertex in clip space
    output.Position = mul(float4(input.Position, 1.0f), WVP);
    
    //Normal in world space - 0.0f for 
    output.NormalWS = normalize(mul(float4(input.Normal, 0.0f), World).xyz);
    
    //Texture coordinates the same
    output.TexCoord = input.TexCoord;
    
    //Vertex position in world space
    output.PositionWS = mul(float4(input.Position, 1.0f), World);
    
    return output;
}