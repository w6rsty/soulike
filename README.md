pipeline template
```lua
pipeline = {
    -- Shaders
    vertex_shader   = "shader.vertex",   -- Register vertex shader name
    fragment_shader = "shader.fragment", -- Register fragment shader name

    -- SDL_GPUVertexInputState
    vertex_input_state = {
        vertex_buffer_descriptions = { -- LIST
            {
                slot               = 0,                      -- Binding slot for the vertex buffer. Determines which buffer corresponds to this slot (u32).
                pitch              = 0,                      -- Distance in bytes between consecutive vertices in the buffer (u32).
                input_rate         = VertexInputRate.VERTEX, -- Specifies whether data is per-vertex or per-instance (SDL_GPUVertexInputRate).
                instance_step_rate = 0,                      -- Step rate for instanced rendering; 0 means no instancing (u32).
            },
        },
        -- SDL_GPUVertexAttribute
        vertex_attributes = { -- LIST
            {
                location    = 0,                          -- Attribute location in the shader (u32). Links this attribute to a variable in the shader.
                buffer_slot = 0,                          -- Index of the vertex buffer supplying this attribute (u32).
                format      = VertexElementFormat.FLOAT3, -- Data format of the attribute, e.g., 3-component float (SDL_GPUVertexElementFormat).
                offset      = 0,                          -- Byte offset within the vertex buffer where this attribute begins (u32).
            },
        },
    },

    -- Primitive type configuration
    primitive_type = PrimitiveType.TRIANGLELIST, -- Specifies the type of primitives to render (e.g., triangle list) (SDL_GPUPrimitiveType).

    -- SDL_GPURasterizerState
    rasterizer_state = {
        fill_mode                  = FillMode.FILL, -- How polygons are filled: FILL for solid or LINE for wireframe (SDL_GPUFillMode).
        cull_mode                  = CullMode.NONE, -- Determines if back-face culling is applied (SDL_GPUCullMode).
        front_face                 = FrontFace.CCW, -- Specifies the winding order for front-facing polygons (SDL_GPUFrontFace).
        depth_bias_constant_factor = 0.0,           -- Constant factor applied to depth values (f32).
        depth_bias_clamp           = 0.0,           -- Maximum allowable depth bias (f32).
        depth_bias_slope_factor    = 0.0,           -- Factor applied based on the polygon slope (f32).
        enable_depth_bias          = false,         -- Enables adding a depth bias to polygons (bool).
        enable_depth_clip          = false,         -- Enables clipping polygons outside the depth range (bool).
    },

    -- SDL_GPUMultisampleState
    multisample_state = {
        sample_count = SampleCount.SAMPLE_COUNT_1, -- Number of samples per pixel for multisampling (SDL_GPUSampleCount).
        sample_mask  = 0,                          -- Bitmask indicating which samples are active (u32).
        enable_mask  = false,                      -- Enables the sample mask for selective rendering (bool).
    },

    -- SDL_GPUDepthStencilState
    depth_stencil_state = { 
        compare_op = CompareOp.LESS, -- Operation used for depth comparisons (SDL_GPUCompareOp).
        -- SDL_GPUStencilOpState
        back_stencil_state = {
            fail_op       = StencilOp.KEEP,   -- Action when stencil test fails (SDL_GPUStencilOp).
            pass_op       = StencilOp.KEEP,   -- Action when stencil test passes (SDL_GPUStencilOp).
            depth_fail_op = StencilOp.KEEP,   -- Action when depth test fails (SDL_GPUStencilOp).
            compare_op    = CompareOp.ALWAYS, -- Stencil comparison operation (SDL_GPUCompareOp).
        },
        -- SDL_GPUStencilOpState
        front_stencil_state = {
            fail_op       = StencilOp.KEEP,   -- Action when stencil test fails (SDL_GPUStencilOp).
            pass_op       = StencilOp.KEEP,   -- Action when stencil test passes (SDL_GPUStencilOp).
            depth_fail_op = StencilOp.KEEP,   -- Action when depth test fails (SDL_GPUStencilOp).
            compare_op    = CompareOp.ALWAYS, -- Stencil comparison operation (SDL_GPUCompareOp).
        },
        compare_mask        = 0xFF,  -- Mask applied to stencil values before comparison (u8).
        write_mask          = 0xFF,  -- Mask determining which stencil bits are writable (u8).
        enable_depth_test   = false, -- Enables testing against depth buffer (bool).
        enable_depth_write  = false, -- Enables writing to the depth buffer (bool).
        enable_stencil_test = false, -- Enables stencil testing (bool).
    },

    -- SDL_GPUGraphicsPipelineTargetInfo
    target_info = { 
        -- SDL_GPUColorTargetDescription
        color_target_descriptions = { -- LIST
            {
                format = TextureFormat.INVALID, -- Format of the color target texture (SDL_GPUTextureFormat).
                -- SDL_GPUColorTargetBlendState
                blend_state = {
                    src_color_blendfactor   = BlendFactor.ONE,  -- Source color blend factor (SDL_GPUBlendFactor).
                    dst_color_blendfactor   = BlendFactor.ZERO, -- Destination color blend factor (SDL_GPUBlendFactor).
                    color_blend_op          = BlendOp.ADD,      -- Blend operation for color components (SDL_GPUBlendOp).
                    src_alpha_blendfactor   = BlendFactor.ONE,  -- Source alpha blend factor (SDL_GPUBlendFactor).
                    dst_alpha_blendfactor   = BlendFactor.ZERO, -- Destination alpha blend factor (SDL_GPUBlendFactor).
                    alpha_blend_op          = BlendOp.ADD,      -- Blend operation for alpha components (SDL_GPUBlendOp).
                    color_write_mask        = 0xF,              -- Bitmask specifying which color channels to write (SDL_GPUColorComponentFlags).
                    enable_blend            = false,            -- Enables blending for this target (bool).
                    enable_color_write_mask = false,            -- Enables the color write mask (bool).
                },
            },
        },
        depth_stencil_format      = TextureFormat.INVALID, -- Format of the depth-stencil target texture (SDL_GPUTextureFormat).
        has_depth_stencil_target  = false,                 -- Indicates if a depth-stencil target is used (bool).
    },
}
```


```lua
shader = {
    is_byte_code = false,
    source_path = "/Users/w6rsty/dev/Cpp/soulike/src/shaders/test.slang", -- source code / target byte code
    stage = ShaderStage.Vertex, -- SDL_GPUShaderStage
    format = ShaderFormat.MSL, -- SDL_GPUShaderFormat
    entry_point = "vertexMain",
    num_samplers = 0,
    num_storage_buffers = 0,
    num_uniform_buffres = 0
}
```