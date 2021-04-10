//Read and write texture to backbuffer
RWTexture2D<unorm float4> backbuffer : register(u0);

cbuffer GaussSettings : register(b0)
{ 
    float C_Sigma;
    float C_Radius;
    float C_WinWidth;
    float C_WinHeight;
    bool C_DoVertBlur;
    float3 padding;
}

//Each group works with 8x8 = 64 threads, Maximum is 1024 threads
[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID)        //uint groupIndex : SV_GroupIndex 
{
    //TESTING: INVERTING THE COLOURS for every pixel
    //backbuffer[DTid.xy] = float4(1.0f, 1.0f, 1.0f, 1.0f) - backbuffer[DTid.xy];
    
    //BLUR BUT NOT WITH GAUSS... 
    /*
    float4 finalColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int w = -5; w <= 5; w++)
    {
        for (int h = -5; h <= 5; h++)
        {
            finalColour += backbuffer[DTid.xy + float2(w, h)];
        }
    }
    backbuffer[DTid.xy] = finalColour / (11 * 11);
    */
    
    float2 dxy = float2(0.0f, 0.0f);
    if (C_DoVertBlur)
        dxy = float2(1.0f/C_WinWidth, 0.0f);
    else
        dxy = float2(0.0f, 1.0f/C_WinHeight);
        
    //FIX LATER
    //float gaussFilter[5] = { 0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f};           //sigma 1 and radius 2
    //float gaussFilter[5] = { 0.192077f, 0.203914f, 0.208019f, 0.203914f, 0.192077f};      //sigma 5 and radius 2
    //float gaussFilter[7] = { 0.129001f, 0.142521f, 0.151303f, 0.15435f, 0.151303f, 0.142521f, 0.129001f }; //sigma 5 and radius 3
    float gaussFilter[11] = { 0.066414, 0.079465, 0.091364, 0.100939, 0.107159, 0.109317,
                              0.107159, 0.100939, 0.091364, 0.079465, 0.066414 }; //sigma 5 and radius 5
    
    float4 finalColour;
    int counter = 0;
    for (int i = -5; i <= 5; i++)
    {
        //Horizontal blur
        finalColour += backbuffer[DTid.xy + (dxy * i) ] * gaussFilter[counter++];
    }
    
    backbuffer[DTid.xy] = finalColour;   
    
}