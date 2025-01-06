import default_shared;

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float3 color = input.coarse_vertex.normal * 0.5 + 0.5;

    return float4(color, 1);
}