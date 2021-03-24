
cbuffer Camera : register(b0)
{
    float3 C_CameraPos;
    uint C_RenderMode;
}

struct GSInput
{
    float4 PositionCS : SV_POSITION;
    float4 PositionWS : POSITIONWS;
    float2 TexCoord : TEXCOORD;
    float3 NormalWS : NORMALWS;
    float3 TangentWS : TANGENTWS;
    float3 BiTangentWS : BITANGENTWS;
};


struct GSOutput
{
    float4 PositionCS   : SV_POSITION;
    float4 PositionWS   : POSITIONWS;
    float2 TexCoord     : TEXCOORD;
    float3 NormalWS     : NORMALWS;
    float3 TangentWS    : TANGENTWS;
    float3 BiTangentWS  : BITANGENTWS;
};

[maxvertexcount(3)]
void main(triangle GSInput input[3] : SV_POSITION, inout TriangleStream< GSOutput > output)
{
    GSOutput element;
    
    //All 3 vertices for this triangle
    float3 v0 = input[0].PositionWS.xyz;
    float3 v1 = input[1].PositionWS.xyz;
    float3 v2 = input[2].PositionWS.xyz;
    
    //Make 2 vectors of this three
    float3 vec1 = v1 - v0;
    float3 vec2 = v2 - v0;
    
    //Make a normal of this face/triangle
    float3 triNormal = normalize(cross(vec1, vec2));
    
    //Direction that points from first point to the camera
    float3 camDir = normalize(C_CameraPos - v0);                    //change to view direction???
    
    //Normal of the face have to be between 0.0f to 1.0f
    //Will be -1 if backward face is showing
    if (dot(triNormal, camDir) >= 0.0f)
    {
        for (uint i = 0; i < 3; i++)
        {
            element.PositionCS = input[i].PositionCS;
            element.PositionWS = input[i].PositionWS;
            element.TexCoord = input[i].TexCoord;
            element.NormalWS = input[i].NormalWS;
            element.TangentWS = input[i].TangentWS;
            element.BiTangentWS = input[i].BiTangentWS;
            output.Append(element);
        }
    }
}