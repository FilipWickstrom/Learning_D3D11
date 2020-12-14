cbuffer constBufWVP : register(b0)  //Constant buffer register 0
{
	//Transforming to row_major
	row_major float4x4 world;
	row_major float4x4 view;
	row_major float4x4 projection;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 uv : UV;
	float3 normal : NORMAL;
    float3 pixelPosWorld : PIXELPOSWORLD;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4x4 WVP = mul(mul(world, view), projection);
    
	//Positioning of vertex relative to the WVP
	output.position = mul(float4(input.position, 1.0f), WVP);
	
	//Normalize the normal for the vertex in worldspace
	output.normal = normalize(mul(float4(input.normal, 0.0f), world).xyz);
	
	//The pixels position in the worldspace
    output.pixelPosWorld = mul(float4(input.position, 1.0f), world).xyz;
	
	//Not going to change
	output.uv = input.uv;
	
	return output;
}