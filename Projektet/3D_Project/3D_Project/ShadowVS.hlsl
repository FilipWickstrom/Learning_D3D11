
cbuffer ShadowWVP : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
};

struct Input
{
    float3 Position : POSITION;
};

struct Output
{
	float4 Position	  : SV_Position; 
    float4 PositionWS : POSITIONWS;
};

Output main(const Input input)
{
    Output output;
	
    float4x4 WVP = mul(mul(World, View), Projection);
    output.Position = mul(float4(input.Position, 1.0f), WVP);
    output.PositionWS = mul(float4(input.Position, 1.0f), World);
	
	return output;
}