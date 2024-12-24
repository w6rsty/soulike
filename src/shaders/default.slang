cbuffer TimeCB : register(b0)
{
    float time        : packoffset(c0);
    float2 resolution : packoffset(c1);
};

static const float3 vertices[6] =
{
    float3(-1.0, -1.0, 0.0),
    float3(+1.0, -1.0, 0.0),
    float3(+1.0, +1.0, 0.0),
    float3(-1.0, -1.0, 0.0),
    float3(+1.0, +1.0, 0.0),
    float3(-1.0, +1.0, 0.0),
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

[shader("vertex")]
VertexOutput vertexMain(uint id : SV_VertexID)
{
    VertexOutput output;
    output.position = float4(vertices[id], 1.0);
    output.uv = vertices[id].xy * 0.5 + 0.5;
    return output;
}

[shader("fragment")]
uint4 fragmentMain(VertexOutput input) : SV_Target
{
    float t = time + 5.;
    float z = 6.;

    const int n = 200; // particle count

    float3 startColor = float3(0, 0.64, 0.2);
    float3 endColor = float3(0.06, 0.35, 0.85);

    float startRadius = 0.84;
    float endRadius = 1.6;

    float power = 0.51;
    float duration = 4.;

    float2 s = float2(800, 600);
    float2 v = z * (2. * input.position.xy - s) / s.y;

    float3 col = float3(0.);

    float2 pm = v.yx * 2.8;

    float dMax = duration;

    float evo = (sin(time * .01 + 400.) * .5 + .5) * 99. + 1.;

    float mb = 0.;
    float mbRadius = 0.;
    float sum = 0.;
    for (int i = 0; i < n; i++)
    {
        float d = fract(t * power + 48934.4238 * sin(float(i / int(evo)) * 692.7398));

        float tt = 0.;

        float a = 6.28 * float(i) / float(n);

        float x = d * cos(a) * duration;

        float y = d * sin(a) * duration;

        float distRatio = d / dMax;

        mbRadius = lerp(startRadius, endRadius, distRatio);

        float2 p = v - float2(x, y); //*vec2(1,sin(a+3.14159/2.));

        mb = mbRadius / dot(p, p);

        sum += mb;

        col = lerp(col, lerp(startColor, endColor, distRatio), mb / sum);
    }

    sum /= float(n);

    col = normalize(col) * sum;

    sum = clamp(sum, 0., .4);

    float3 tex = float3(1.);

    col *= smoothstep(tex, float3(0.), float3(sum));

    return uint4(uint3(col * 255.0), 255);
}