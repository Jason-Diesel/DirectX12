struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    return tex.Sample(samp, input.uv);
}