pipeline = {
    vertex_shader   = "default.vertex",
    fragment_shader = "default.fragment",
    primitive_type = PrimitiveType.TRIANGLELIST,
    rasterizer_state = {
        fill_mode = FillMode.FILL,
        cull_mode = CullMode.NONE,
        front_face = FrontFace.CW,
    },
    multisample_state = {
        sample_count = SampleCount.SAMPLE_COUNT_1,
        enable_mask = false,
    },
    depth_stencil_state = { 
        enable_depth_test = false,
        enable_depth_write = false,
        enable_stencil_test = false,
    },
    target_info = { 
        color_target_descriptions = {
            {
                format = TextureFormat.R8G8B8A8_UINT,
                blend_state = {
                    enable_blend = false,
                },
            },
        },
        has_depth_stencil_target  = false,
    },
}