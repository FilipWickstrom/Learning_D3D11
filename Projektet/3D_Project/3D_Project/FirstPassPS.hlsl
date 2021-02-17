//Main texture
Texture2D texture0 : register(t0);

//Sampler
SamplerState sampler0 : register(s0);

//Input from the vertex shader in first pass
struct PixelInput
{
    float4 Position     : SV_POSITION;
	float3 NormalWS	    : NORMALWS;
    float2 TexCoord     : TEXCOORD; 
    float4 PositionWS   : POSITIONWS;
};

//Output to g-buffer to be used for second pass
struct PixelOutput
{
    float4 PositionWS   : SV_TARGET0;
    float4 Colour       : SV_Target1;
    float4 NormalWS     : SV_Target2;
    //float4 Material : SV_Target2; (float4(Metalness, Glossiness, Shiniess, 1.0f)) or something like it.
};

PixelOutput main(PixelInput input) : SV_TARGET
{
    PixelOutput output;
    
    //
    output.PositionWS = input.PositionWS;
    
    //Sampling the colour from the texture
    output.Colour = texture0.Sample(sampler0, input.TexCoord);
    
    //Sends forward the normal to the g-buffer
    output.NormalWS = float4(input.NormalWS, 1.0f);
    
    return output;
}