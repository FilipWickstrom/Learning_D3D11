Texture2D colourTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState theSampler : register(s0);

struct PixelInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

float4 main( PixelInput input ) : SV_TARGET
{
    float4 colours;
    float4 normals;
    
    //DO SOME LIGHTNING
    
    
    colours = colourTexture.Sample(theSampler, input.TexCoord);
    normals = normalTexture.Sample(theSampler, input.TexCoord);
    
	return colours;
}