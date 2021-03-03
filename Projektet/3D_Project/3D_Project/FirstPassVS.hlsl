
cbuffer WVPMatrix : register(b0)
{
    row_major float4x4 World;   //Model matrix
    row_major float4x4 View;
    row_major float4x4 Projection;
};

cbuffer Settings : register(b1)
{
    uint useNormalMap;
}

//Input from the input data from OBJ-files
struct VertexInput
{
    float3 Position     : POSITION;
    float2 TexCoord     : TEXCOORD;
    float3 Normal       : NORMAL;
    float3 Tangent      : TANGENT;      //CHANGE TO FLOAT4???
};

//Output that is going to be used in the first pass pixel shader
struct VertexOutput
{
    float4 PositionCS   : SV_POSITION;  //clipspace
    float4 PositionWS   : POSITIONWS;
    float2 TexCoord     : TEXCOORD;
    float3 NormalWS     : NORMALWS;
    float3 TangentWS    : TANGENTWS;
    float3 BiTangentWS  : BITANGENTWS;
};

VertexOutput main(const VertexInput input)
{
	VertexOutput output;
    
    //World * View * Projection
    float4x4 WVP = mul(mul(World, View), Projection);
    
    //Position of the vertex in clip space
    output.PositionCS = mul(float4(input.Position, 1.0f), WVP);
    
    //Normal in world space
    output.NormalWS = normalize(mul(input.Normal, (float3x3)World));
    
    if (useNormalMap == 1)
    {
        // Normal is in worldspace and so should tangent and bitangent also be
        output.TangentWS = normalize(mul(input.Tangent, (float3x3) World));
        
        //if (dot(output.NormalWS, output.TangentWS) == 0.0f)
        //{
        //    output.NormalWS = float3(0, 0, 0);
        //}
        
        //ÄR INTE ORTOGONAL MED VARANDRA HÄR EFTER...
        
        output.BiTangentWS = normalize(cross(output.NormalWS, output.TangentWS)); // * tangent.w? up or down? -1 or 1   
    }
    else
    {
        // Avoid getting unset values warning
        output.TangentWS = float3(0, 0, 0);
        output.BiTangentWS = float3(0, 0, 0);
    }
    
    //Texture coordinates the same
    output.TexCoord = input.TexCoord;
    
    //Vertex position in world space
    output.PositionWS = mul(float4(input.Position, 1.0f), World);
    
    return output;
}