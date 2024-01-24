struct VertexInput
{
    float3 position : Position;
    float3 color : Color;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 color : Color;
};

cbuffer MVP : register(b0)
{
    row_major matrix view;
    row_major matrix projection;
};

cbuffer Transform : register(b1)
{
    row_major matrix transform;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    float4x4 mvp = mul(mul(transform, view), projection);
    output.color = input.color;
    output.position = mul(float4(input.position, 1.0), mvp);

    return output;
}
