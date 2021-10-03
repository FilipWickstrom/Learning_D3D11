#define MAXWEIGHTS 32

Texture2D input                  : register(t0);
RWTexture2D<unorm float4> output : register(u0);

cbuffer BilateralSettings : register(b0)
{
    uint C_Radius;
    bool C_DoVertFilter;
    float C_Sigma;
}

//Weights that has been calculated like a gaussfilter
cbuffer BilateralWeights : register(b1)
{
    float4 C_Filter[MAXWEIGHTS / 4];
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    //Go vertical or horizontal
    float2 dxy = float2(0.0f, 0.0f);
    if (C_DoVertFilter)
        dxy = float2(1.0f, 0.0f);
    else
        dxy = float2(0.0f, 1.0f);
    
    float4 centerColour = input[DTid.xy];
    float4 totalColour = 0.0f;
    float4 totalWeight = 0.0f;
    
    int start = C_Radius * -1;
    int end = C_Radius;
    uint pos = 0;
    
    for (int i = start; i <= end; i++)
    {
        //Unpackage the right gauss filter for it
        float gaussWeight = (C_Filter[pos / 4])[pos % 4];
        pos++;
        
        float4 currentColour = input[DTid.xy + (dxy * i)];
        
        //Difference in colour
        float4 delta = centerColour - currentColour;
        
        //Gaussian function - flatting out the colour depending on sigma value
        //https://en.wikipedia.org/wiki/Gaussian_function
        //exp = e^x where x = ((-1.0f*delta...
        float4 G_range = exp((-1.0f * delta * delta) / (2.0f * C_Sigma * C_Sigma));
        
        //Like the picture on page 7
        //https://people.csail.mit.edu/sparis/bf_course/slides/03_definition_bf.pdf
        totalColour += currentColour * G_range * gaussWeight;
        totalWeight += G_range * gaussWeight;
    }
    
    //Normalization of the final colour
    output[DTid.xy] = totalColour / totalWeight;
    
}