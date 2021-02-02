//Textures
Texture2D texture0 : register(t0); //first texture

//Sampler
SamplerState sampler0 : register(s0);

//Input from the vertex shader in first pass
struct PixelInput
{
	float4 Position	: SV_Position;
	float3 Normal	: NORMAL;
    float2 TexCoord : TEXCOORD; 
};

//Output to g-buffer to be used for second pass
struct PixelOutput
{
    float4 Colour : SV_Target0;
    float4 Normal : SV_Target1;
    //float4 Material : SV_Target2; (float4(Metalness, Glossiness, Shiniess, 1.0f)) or something like it.
};

PixelOutput main(PixelInput input) : SV_TARGET
{
    PixelOutput output;
    
    //Sampling the colour from the texture
    output.Colour = texture0.Sample(sampler0, input.TexCoord);
    
    //Sends forward the normal to the g-buffer
    output.Normal = float4(input.Normal, 1.0f);
    
    return output;
}