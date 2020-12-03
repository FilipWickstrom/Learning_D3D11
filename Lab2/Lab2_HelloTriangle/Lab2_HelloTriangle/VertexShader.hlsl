cbuffer CONSTBUFFER
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : UV;
	//float3 normal : NORMAL
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	//normal
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	matrix WVP = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), WVP);
	output.uv = input.uv;
	return output;

	//output.normal = normalize(input.normal);
}