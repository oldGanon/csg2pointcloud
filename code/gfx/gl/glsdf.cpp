struct glsdf_shape
{
    vec4 Params;
    vec4 Position;
    f32 Smoothing;
    u32 ShapeOp;
    u32 Padding1;
    u32 Padding2;
};

struct glsdf
{
    u32 EditCount;
    u32 Padding0;
    u32 Padding1;
    u32 Padding2;
    glsdf_shape Edits[16];
};
