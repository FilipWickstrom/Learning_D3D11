cbuffer ConstantBuffer
{
	float4x4 W;
	//float4x4 V;
	//float4x4 P;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : UV;

	//float3 normal : NORMAL
	//float4x4 worldMatrix : WORLDMATRIX
	//float4x4 viewMatrix : VIEWMATRIX
	//float4x4 projectMatrix : PROJMATRIX
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
	//row_major float4x4 WVP = mul(mul(W, V), P);
	//output.position = mul(float4(input.position, 1.0f), WVP);
	//output.position = mul(float4(input.position, 1.0f), W);
	output.position = float4(input.position, 1.0f);
	output.uv = input.uv;
	//output.colour = input.colour;
	return output;

	//output.normal = normalize(input.normal);
}