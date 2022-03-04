
cbuffer ShadowWVP : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

float4 main(const float3 position : POSITION) : SV_Position
{
    float4x4 WVP = mul(mul(World, View), Projection);
    return mul(float4(position, 1.0f), WVP);
}