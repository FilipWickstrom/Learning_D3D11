//Read and write texture to backbuffer
RWTexture2D<unorm float4> backbuffer : register(u0);

//Sampler -not with wrap
//dispatch size has a relation depending on width and height, for example, 
//640x480 got [20, 15, 1] where 15*(640/480)=20 and 1920x1080 will get??? [32, 18, 1]?

//Works great for 16:9 resolutions
[numthreads(32, 18, 1)]       //[numthreads(40, 24, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    //TESTING: INVERTING THE COLOURS
    backbuffer[DTid.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f) - backbuffer[DTid.xy];
    
    //Have to map from [0, 1280] to [0, 1] in x
    //and from [0, 720] to [0, 1] y
    
    //gaussFilter[5] = { 0.06136, 0.24477, 0.38774, 0.24477, 0.06136 };
    
    /*
    float2 steps;
    
    if (verticalBlur is on)
        steps = float2(0, 1.0f / width);
    else if(horizontalBlur is on)
        steps = float2(1.0f / height, 0);
    
    float4 finalPixel = 
    for (int i = 0; i < 5; i++)
    {
        //read from the pixels
        finalPixel += Texture.Sample(..., uv + steps) * gaussFilter[i]
    }
    */
    
}