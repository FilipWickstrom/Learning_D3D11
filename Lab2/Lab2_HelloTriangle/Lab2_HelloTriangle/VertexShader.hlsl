cbuffer CONSTBUFFER	//b0 per-frame
{
	row_major float4x4 world;	//Transforming to row_major
	row_major float4x4 view;
	row_major float4x4 projection;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : UV;
	//float3 normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	//float3 normal : NORMAL;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4x4 WVP = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), WVP);
	
	//output.normal = mul(float4(input.normal, 0.0f), world).xyz;
	//output.normal = normalize(output.normal);
	
	output.uv = input.uv;
	return output;
}