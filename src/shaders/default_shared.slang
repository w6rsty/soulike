module deafult_shared;

public cbuffer TimeCB : register(b0)
{
    public float time           : packoffset(c0.x);
    public float2 resolution    : packoffset(c0.z);
    public float4x4 projection  : packoffset(c1);
    public float4x4 view        : packoffset(c5);
};

public struct VertexInput
{
    public float3 position : POSITION;
    public float3 normal : NORMAL;
};

public struct CoarseVertex
{
    public float3 position : POSITION;
    public float3 normal   : NORMAL;
};

public struct VertexOutput
{
    public CoarseVertex coarse_vertex : CoarseVertex;
    public float4 sv_position : SV_Position;
};