// ShaderMain.hlsl

////////
/// Constant Buffer
////////
cbuffer ChangeEveryObject : register(b0)
{
    matrix WorldMatrix;
    float4 CustomColor;
    uint bUseVertexColor;
}

cbuffer ChangeEveryFrame : register(b1)
{
    matrix ViewMatrix;
}

cbuffer ChangeOnResizeAndFov : register(b2)
{
    matrix ProjectionMatrix;
    float NearClip;
    float FarClip;
}

cbuffer UUIDColor : register(b3){
    float4 UUIDColor;
}

cbuffer Depth : register(b4){
    int depth;
    int nearPlane;
    int farPlane;
}

////////
/// Input, Output
////////
struct VS_INPUT
{
    float3 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR;       // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR;          // Color to pass to the pixel shader
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
    float depth : SV_Depth;
};

////////
/// Function
////////
PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;
    output.position = float4(input.position.xyz, 1.0f);
    output.position = mul(output.position, WorldMatrix);
    output.position = mul(output.position, ViewMatrix);
    output.position = mul(output.position, ProjectionMatrix);

    output.color = bUseVertexColor == 1 ? input.color : CustomColor;
    return output;
}


PS_OUTPUT mainPS(PS_INPUT input):SV_TARGET{
    PS_OUTPUT output;
    output.color = UUIDColor;
    return output;
}