cbuffer TimeCB : register(b0)
{
    float time           : packoffset(c0.x); // c0.x
    float2 resolution    : packoffset(c0.z);

    float4x4 projection  : packoffset(c1);   // 64字节
    float4x4 view        : packoffset(c5);   // 64字节
};


struct VertexInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct CoarseVertex
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
};

struct VertexOutput
{
    CoarseVertex coarse_vertex : CoarseVertex;
    float4 sv_position : SV_Position;
};

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

[shader("fragment")]
float4 fragmentMain(VertexOutput input) : SV_Target
{
    float3 color = input.coarse_vertex.normal * 0.5 + 0.5;

    return float4(color, 1);
}