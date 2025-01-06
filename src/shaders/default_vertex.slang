import default_shared;

[shader("vertex")]
VertexOutput vertexMain(VertexInput input)
{
    VertexOutput output;

    float4 position = mul(view, float4(input.position, 1));

    output.coarse_vertex.position = position.xyz;
    output.coarse_vertex.normal = normalize(mul(float4(input.normal, 0), view).xyz);

    output.sv_position = mul(projection, position);
    return output;
}
