struct VSInput {
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
};

struct PSInput {
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer constants : register(b0)
{
    row_major float4x4 M;
    row_major float4x4 VP;
    float Flag;
}

PSInput main(VSInput input) {


    PSInput output;
    output.position = mul(float4(input.position, 1.0f), mul(M, VP));
    
    output.texCoord = input.texCoord;
    
    return output;
}
