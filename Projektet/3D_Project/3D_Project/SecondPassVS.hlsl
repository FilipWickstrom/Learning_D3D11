
struct VertexInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VertexOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};


VertexOutput main( VertexInput input )
{
    VertexOutput output;
     
    output.TexCoord = input.TexCoord;
    
    //Convertion from 0 to 1 coordinates to -1 to +1
    //Needed as the screen quad has this kinds of coordinates
    output.Position = float4(output.TexCoord.x * 2 - 1, (1 - output.TexCoord.y) * 2 - 1, 0.0f, 1.0f);
    
	return output;
}