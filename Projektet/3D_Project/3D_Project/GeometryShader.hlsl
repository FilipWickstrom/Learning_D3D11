
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
    float3 v0 = input[0].PositionCS.xyz;
    float3 v1 = input[1].PositionCS.xyz;
    float3 v2 = input[2].PositionCS.xyz;
    
    float3 vec1 = v1 - v0; //v1 head, v0 tail
    float3 vec2 = v2 - v0; //v2 head, v0 tail
    
    //Normal of the surface
    float3 faceNormal = normalize(cross(vec1, vec2));
    //Camera pos ( (0,0,0) in screenspace) is head, v0 is tail
    float3 camera = normalize(float3(0, 0, 0) - v0);
    
    //Normal of the face have to be between 0.0f to 1.0f
    //Will be -1 if back face is showing
    if (dot(camera, faceNormal) >= 0)
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