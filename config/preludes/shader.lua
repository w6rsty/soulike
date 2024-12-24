ShaderStage = {
    Vertex    = 0,
    Fragment = 1
}

ShaderFormat = {
    INVALID  = 0,
    PRIVATE  = 1 << 0,
    SPIRV    = 1 << 1,
    DXBC     = 1 << 2,
    DXIL     = 1 << 3,
    MSL      = 1 << 4,
    METALLIB = 1 << 5,
}