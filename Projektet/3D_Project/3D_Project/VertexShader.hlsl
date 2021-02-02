cbuffer constantBufferWVP : register(b0)
{
    row_major float4x4 cb_world;
    row_major float4x4 cb_view;
    row_major float4x4 cb_projection;
};


struct VS_INPUT
{
    float3 Position     : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position     : SV_POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
    //float3 PositionWS   : POSITIONWS;
};

VS_OUTPUT main(const VS_INPUT input)
{
    VS_OUTPUT output;
    
    //Convert to world space (position och normal)
    //Clip space?
    //Send forward the texture coordinate
    
    float4x4 WVP = mul(mul(cb_world, cb_view), cb_projection);
    
    //
    output.Position = mul(float4(input.Position, 1.0f), WVP);
     
    //Positioning of the normal relative to the world
    output.Normal = normalize(mul(float4(input.Normal, 1.0f), cb_world).xyz);
    
    output.TexCoord = input.TexCoord;
    
    //Position in the world
    //output.PositionWS = mul(float4(input.Position, 1.0f), cb_world);
    
	return output;
}