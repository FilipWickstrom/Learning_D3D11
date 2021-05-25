#define MAXWEIGHTS 32

//Read and write texture to backbuffer
RWTexture2D<unorm float4> backbuffer : register(u0);

cbuffer BilateralSettings : register(b0)
{
    uint C_Radius;
    bool C_DoVertFilter;
    float C_Sigma;
}

//Same weights as from gauss????
cbuffer BilateralWeights : register(b1)
{
    float4 C_Filter[MAXWEIGHTS / 4];
}

static const float filter[7] =
{
    0.030078323, 0.104983664, 0.222250419, 0.285375187, 0.222250419, 0.104983664, 0.030078323
};

groupshared float4 horizontalpoints[1280];
groupshared float4 verticalpoints[720];

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 dxy = float2(0.0f, 0.0f);
    if (C_DoVertFilter)
        dxy = float2(1.0f, 0.0f);
    else
        dxy = float2(0.0f, 1.0f);
    
    float4 centerColour = backbuffer[DTid.xy];
    if (C_DoVertFilter)
        verticalpoints[DTid.y] = centerColour;
    else
        horizontalpoints[DTid.x] = centerColour;
    
    GroupMemoryBarrierWithGroupSync();
    
    float4 colour = 0.0f;
    float4 totalWeight = 0.0f;
    
    int start = -3; //C_Radius * -1;
    int end = 3; //C_Radius;
    uint counter = 0;
    
    for (int i = start; i <= end; i++)
    {
        float4 currentColour; // = backbuffer[DTid.xy + (dxy * i)];
        if (C_DoVertFilter)
            currentColour = verticalpoints[DTid.y + i];
         else
            currentColour = horizontalpoints[DTid.x + i];
        
        //Find delta and use it to calculate weighting
        //Is being used to see if it was a "spike"
        float4 delta = centerColour - currentColour;
        float4 range = exp((-1.0f * delta * delta) / (2.0f * C_Sigma * C_Sigma));
        
        colour += currentColour * range * filter[counter]; //C_Filter[counter];
        totalWeight += range * filter[counter]; //C_Filter[counter];
        counter++;
    }
    
    //Normalize the colour
    backbuffer[DTid.xy] = colour / totalWeight;
}