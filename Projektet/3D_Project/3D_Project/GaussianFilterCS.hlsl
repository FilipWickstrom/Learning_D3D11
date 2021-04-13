#define MAXGAUSSWEIGHTS 32

//Read and write texture to backbuffer
RWTexture2D<unorm float4> backbuffer : register(u0);

cbuffer GaussSettings : register(b0)
{ 
    uint C_Radius;
    float C_WinWidth;
    float C_WinHeight;
    bool C_DoVertBlur;
    float4 C_Weights[MAXGAUSSWEIGHTS/4];
}

//Each group works with 8x8 = 64 threads, Maximum is 1024 threads
[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID)        //uint groupIndex : SV_GroupIndex 
{
    //Do vertical or horizontal blur
    float2 dxy = float2(0.0f, 0.0f);
    if (C_DoVertBlur)
        dxy = float2(1.0f/C_WinWidth, 0.0f);
    else
        dxy = float2(0.0f, 1.0f/C_WinHeight);
    
    float4 finalColour;
    uint counter = 0;
    int start = C_Radius * -1;
    int end = C_Radius;
    
    //Goes through all the values from -Radius to Radius
    for (int i = start; i <= end; i++)
    {
        //Get the packed value on the right position
        float weight = (C_Weights[counter / 4])[counter % 4];
        
        finalColour += backbuffer[DTid.xy + (dxy * i)] * weight;
        counter++;
    }

    backbuffer[DTid.xy] = finalColour;    
}