struct VertexInput {
	float3 position : Position;
	float3 color : Color;
};

struct VertexOutput
{
	float4 position : SV_POSITION;
	float3 color : Color;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;

    output.color = input.color;
    output.position = float4(input.position, 1.0);

	return output;
}