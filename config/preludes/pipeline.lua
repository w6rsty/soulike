PrimitiveType = {
    TRIANGLELIST  = 0,
    TRIANGLESTRIP = 1,
    LINELIST      = 2,
    LINESTRIP     = 3,
    POINTLIST     = 4,
}

VertexInputRate = {
    VERTEX   = 0,
    INSTANCE = 1,
}

VertexElementFormat = {
    INVALID      = 0,
    -- 32-bit Signed Integers
    INT          = 1,
    INT2         = 2,
    INT3         = 3,
    INT4         = 4,
    
    -- 32-bit Unsigned Integers
    UINT         = 5,
    UINT2        = 6,
    UINT3        = 7,
    UINT4        = 8,
    
    -- 32-bit Floats
    FLOAT        = 9,
    FLOAT2       = 10,
    FLOAT3       = 11,
    FLOAT4       = 12,
    
    -- 8-bit Signed Integers
    BYTE2        = 13,
    BYTE4        = 14,
    
    -- 8-bit Unsigned Integers
    UBYTE2       = 15,
    UBYTE4       = 16,
    
    -- 8-bit Signed Normalized
    BYTE2_NORM   = 17,
    BYTE4_NORM   = 18,
    
    -- 8-bit Unsigned Normalized
    UBYTE2_NORM  = 19,
    UBYTE4_NORM  = 20,
    
    -- 16-bit Signed Integers
    SHORT2       = 21,
    SHORT4       = 22,
    
    -- 16-bit Unsigned Integers
    USHORT2      = 23,
    USHORT4      = 24,
    
    -- 16-bit Signed Normalized
    SHORT2_NORM  = 25,
    SHORT4_NORM  = 26,
    
    -- 16-bit Unsigned Normalized
    USHORT2_NORM = 27,
    USHORT4_NORM = 28,
    
    -- 16-bit Floats
    HALF2        = 29,
    HALF4        = 30
}

FillMode = {
    FILL = 0,
    LINE = 1
}

CullMode = {
    NONE  = 0,
    FRONT = 1,
    BACK  = 2,
}

FrontFace = {
    CCW = 0,
    CW  = 1
}

SampleCount = {
    SAMPLE_COUNT_1  = 0,
    SAMPLE_COUNT_2  = 1,
    SAMPLE_COUNT_4  = 2,
    SAMPLE_COUNT_8  = 3
}

CompareOp = {
    INVALID          = 0,
    NEVER            = 1,
    LESS             = 2,
    EQUAL            = 3,
    LESS_OR_EQUAL    = 4,
    GREATER          = 5,
    NOT_EQUAL        = 6,
    GREATER_OR_EQUAL = 7,
    ALWAYS           = 8
}

StencilOp = {
    INVALID             = 0,
    KEEP                = 1,
    ZERO                = 2,
    REPLACE             = 3,
    INCREMENT_AND_CLAMP = 4,
    DECREMENT_AND_CLAMP = 5,
    INVERT              = 6,
    INCREMENT_AND_WRAP  = 7,
    DECREMENT_AND_WRAP  = 8
}

TextureFormat = {
    INVALID               = 0,

    -- Unsigned Normalized Float Color Formats
    A8_UNORM              = 1,
    R8_UNORM              = 2,
    R8G8_UNORM            = 3,
    R8G8B8A8_UNORM        = 4,
    R16_UNORM             = 5,
    R16G16_UNORM          = 6,
    R16G16B16A16_UNORM    = 7,
    R10G10B10A2_UNORM     = 8,
    B5G6R5_UNORM          = 9,
    B5G5R5A1_UNORM        = 10,
    B4G4R4A4_UNORM        = 11,
    B8G8R8A8_UNORM        = 12,

    -- Compressed Unsigned Normalized Float Color Formats
    BC1_RGBA_UNORM        = 13,
    BC2_RGBA_UNORM        = 14,
    BC3_RGBA_UNORM        = 15,
    BC4_R_UNORM           = 16,
    BC5_RG_UNORM          = 17,
    BC7_RGBA_UNORM        = 18,

    -- Compressed Signed Float Color Formats
    BC6H_RGB_FLOAT        = 19,

    -- Compressed Unsigned Float Color Formats
    BC6H_RGB_UFLOAT       = 20,

    -- Signed Normalized Float Color Formats
    R8_SNORM              = 21,
    R8G8_SNORM            = 22,
    R8G8B8A8_SNORM        = 23,
    R16_SNORM             = 24,
    R16G16_SNORM          = 25,
    R16G16B16A16_SNORM    = 26,

    -- Signed Float Color Formats
    R16_FLOAT             = 27,
    R16G16_FLOAT          = 28,
    R16G16B16A16_FLOAT    = 29,
    R32_FLOAT             = 30,
    R32G32_FLOAT          = 31,
    R32G32B32A32_FLOAT    = 32,

    -- Unsigned Float Color Formats
    R11G11B10_UFLOAT      = 33,

    -- Unsigned Integer Color Formats
    R8_UINT               = 34,
    R8G8_UINT             = 35,
    R8G8B8A8_UINT         = 36,
    R16_UINT              = 37,
    R16G16_UINT           = 38,
    R16G16B16A16_UINT     = 39,
    R32_UINT              = 40,
    R32G32_UINT           = 41,
    R32G32B32A32_UINT     = 42,

    -- Signed Integer Color Formats
    R8_INT                = 43,
    R8G8_INT              = 44,
    R8G8B8A8_INT          = 45,
    R16_INT               = 46,
    R16G16_INT            = 47,
    R16G16B16A16_INT      = 48,
    R32_INT               = 49,
    R32G32_INT            = 50,
    R32G32B32A32_INT      = 51,

    -- SRGB Unsigned Normalized Color Formats
    R8G8B8A8_UNORM_SRGB   = 52,
    B8G8R8A8_UNORM_SRGB   = 53,

    -- Compressed SRGB Unsigned Normalized Color Formats
    BC1_RGBA_UNORM_SRGB   = 54,
    BC2_RGBA_UNORM_SRGB   = 55,
    BC3_RGBA_UNORM_SRGB   = 56,
    BC7_RGBA_UNORM_SRGB   = 57,

    -- Depth Formats
    D16_UNORM             = 58,
    D24_UNORM             = 59,
    D32_FLOAT             = 60,
    D24_UNORM_S8_UINT     = 61,
    D32_FLOAT_S8_UINT     = 62,

    -- Compressed ASTC Normalized Float Color Formats
    ASTC_4x4_UNORM        = 63,
    ASTC_5x4_UNORM        = 64,
    ASTC_5x5_UNORM        = 65,
    ASTC_6x5_UNORM        = 66,
    ASTC_6x6_UNORM        = 67,
    ASTC_8x5_UNORM        = 68,
    ASTC_8x6_UNORM        = 69,
    ASTC_8x8_UNORM        = 70,
    ASTC_10x5_UNORM       = 71,
    ASTC_10x6_UNORM       = 72,
    ASTC_10x8_UNORM       = 73,
    ASTC_10x10_UNORM      = 74,
    ASTC_12x10_UNORM      = 75,
    ASTC_12x12_UNORM      = 76,

    -- Compressed SRGB ASTC Normalized Float Color Formats
    ASTC_4x4_UNORM_SRGB   = 77,
    ASTC_5x4_UNORM_SRGB   = 78,
    ASTC_5x5_UNORM_SRGB   = 79,
    ASTC_6x5_UNORM_SRGB   = 80,
    ASTC_6x6_UNORM_SRGB   = 81,
    ASTC_8x5_UNORM_SRGB   = 82,
    ASTC_8x6_UNORM_SRGB   = 83,
    ASTC_8x8_UNORM_SRGB   = 84,
    ASTC_10x5_UNORM_SRGB  = 85,
    ASTC_10x6_UNORM_SRGB  = 86,
    ASTC_10x8_UNORM_SRGB  = 87,
    ASTC_10x10_UNORM_SRGB = 88,
    ASTC_12x10_UNORM_SRGB = 89,
    ASTC_12x12_UNORM_SRGB = 90,

    -- Compressed ASTC Signed Float Color Formats
    ASTC_4x4_FLOAT        = 91,
    ASTC_5x4_FLOAT        = 92,
    ASTC_5x5_FLOAT        = 93,
    ASTC_6x5_FLOAT        = 94,
    ASTC_6x6_FLOAT        = 95,
    ASTC_8x5_FLOAT        = 96,
    ASTC_8x6_FLOAT        = 97,
    ASTC_8x8_FLOAT        = 98,
    ASTC_10x5_FLOAT       = 99,
    ASTC_10x6_FLOAT       = 100,
    ASTC_10x8_FLOAT       = 101,
    ASTC_10x10_FLOAT      = 102,
    ASTC_12x10_FLOAT      = 103,
    ASTC_12x12_FLOAT      = 104,
}

BlendFactor = {
    INVALID                  = 0,
    ZERO                     = 1,
    ONE                      = 2,
    SRC_COLOR                = 3,
    ONE_MINUS_SRC_COLOR      = 4,
    DST_COLOR                = 5,
    ONE_MINUS_DST_COLOR      = 6,
    SRC_ALPHA                = 7,
    ONE_MINUS_SRC_ALPHA      = 8,
    DST_ALPHA                = 9,
    ONE_MINUS_DST_ALPHA      = 10,
    CONSTANT_COLOR           = 11,
    ONE_MINUS_CONSTANT_COLOR = 12,
    SRC_ALPHA_SATURATE       = 13
}

BlendOp = {
    INVALID          = 0,
    ADD              = 1,
    SUBTRACT         = 2,
    REVERSE_SUBTRACT = 3,
    MIN              = 4,
    MAX              = 5
}