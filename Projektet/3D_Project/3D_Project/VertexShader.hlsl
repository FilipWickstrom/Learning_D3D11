/*cbuffer Transforms : register(b0)
{
    //float4x4 world
    //float4x4 view
    //float4x4 projection
};*/



//struct VS_INPUT
//{
//    float4 Position     : POSITION;		//4 eller 3?
//    float3 Normal       : NORMAL;
//	float2 TexCoord     : TEXCOORD;     //UV
//};

//struct VS_OUTPUT
//{
//    float4 Position     : SV_Position;
//    float3 Normal       : NORMAL_WS;     //normal in world space
//    float TexCoord      : TEXCOORD;
//    float3 PositionWS   : POSITION_WS;  //Pixels position in the world
//};

//FOR TESTING***
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Colour   : COLOUR;
    float3 Normal   : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float3 Colour   : COLOUR;
    float3 Normal   : NORMAL;
};

VS_OUTPUT main(const VS_INPUT input)
{
    VS_OUTPUT output;
    
    //Convert to world space (position och normal)
    
    //Clip space?
	
    //Send forward the texture coordinate
    
    //TEST
    output.Position = float4(input.Position, 1.0f);
    output.Colour = input.Colour;
    output.Normal = input.Normal;
	return output;
}