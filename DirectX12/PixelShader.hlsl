struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 color : Color;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}