pipeline = {
    vertex_shader   = "default.vertex",
    fragment_shader = "default.fragment",
    vertex_input_state = {
        vertex_buffer_descriptions = {
            {
                slot               = 0,
                pitch              = 4 * 3, -- position
                input_rate         = VertexInputRate.VERTEX,
                instance_step_rate = 0,
            },
            {
                slot               = 1,
                pitch              = 4 * 3, -- normal
                input_rate         = VertexInputRate.VERTEX,
                instance_step_rate = 0,
            }
        },
        vertex_attributes = {
            {
                location    = 0,
                buffer_slot = 0,
                format      = VertexElementFormat.FLOAT3,
                offset      = 0,
            },
            {
                location    = 1,
                buffer_slot = 1,
                format      = VertexElementFormat.FLOAT3,
                offset      = 0,
            }
        },
    },
    primitive_type = PrimitiveType.TRIANGLELIST,
    rasterizer_state = {
        fill_mode = FillMode.FILL,
        cull_mode = CullMode.BACK,
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
                format = 12,
                blend_state = {
                    enable_blend = false,
                },
            },
        },
        has_depth_stencil_target  = false,
    },
}