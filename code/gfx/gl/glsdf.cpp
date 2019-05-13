
enum glsdf_type
{
    GLSDF_ELLIPSOID = 0,
    GLSDF_CUBOID = 1,
};

enum glsdf_op
{
    GLSDF_ADD = 0,
    GLSDF_SUB = 1,
    GLSDF_ADD_SMOOTH = 2,
    GLSDF_SUB_SMOOTH = 3,
};

struct glsdf_shape
{
    vec4 Params;
    vec4 Position;
    quat Rotation;
    vec4 Color;
    f32 Smoothing;
    u32 ShapeOp;
    f32 RotationScale;
    f32 Influence;
};

struct glsdf
{
    u32 EditCount;
    u32 Padding0;
    u32 Padding1;
    u32 Padding2;
    glsdf_shape Edits[256];
};

struct glsdf_splat
{
    vec4 Position;
    u32 Normal;
    u32 Color;
    u32 Padding[2];
};

static f32
GLSDF_RotationScale(quat Rotation)
{
    return 1.0f / HMax(Abs(Rotation * Vec3(1,1,1)));
}

static void
GLSDF_Add(glsdf *GLSDF, glsdf_shape Edit)
{
    Edit.Smoothing = 0.0f;
    Edit.ShapeOp &= ~3;
    Edit.ShapeOp |= GLSDF_ADD;
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static void
GLSDF_Sub(glsdf *GLSDF, glsdf_shape Edit)
{
    Edit.Smoothing = 0.0f;
    Edit.ShapeOp &= ~3;
    Edit.ShapeOp |= GLSDF_SUB;
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static void
GLSDF_AddSmooth(glsdf *GLSDF, glsdf_shape Edit, f32 Smoothing)
{
    if (Smoothing <= 0.0f)
    {
        Edit.Smoothing = 0.0f;
        Edit.ShapeOp &= ~3;
        Edit.ShapeOp |= GLSDF_ADD;
    }
    else
    {
        Edit.Smoothing = Smoothing;
        Edit.ShapeOp &= ~3;
        Edit.ShapeOp |= GLSDF_ADD_SMOOTH;
    }
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static void
GLSDF_SubSmooth(glsdf *GLSDF, glsdf_shape Edit, f32 Smoothing)
{
    if (Smoothing <= 0.0f)
    {
        Edit.Smoothing = 0.0f;
        Edit.ShapeOp &= ~3;
        Edit.ShapeOp |= GLSDF_SUB;
    }
    else
    {
        Edit.Smoothing = Smoothing;
        Edit.ShapeOp &= ~3;
        Edit.ShapeOp |= GLSDF_SUB_SMOOTH;
    }
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static glsdf_shape
GLSDF_Sphere(vec3 Position, vec4 Color, f32 Radius)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,Radius,Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f;
    Shape.Color = Color;
    Shape.Smoothing = 0.0f;
    Shape.ShapeOp = (GLSDF_ELLIPSOID << 2) | GLSDF_ADD;
    Shape.Influence = 1.1f * Radius;
    return Shape;
}

static glsdf_shape
GLSDF_Ellipsoid(vec3 Position, vec4 Color, vec3 Radius)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f;
    Shape.Color = Color;
    Shape.Smoothing = 0.0f;
    Shape.ShapeOp = (GLSDF_ELLIPSOID << 2) | GLSDF_ADD;
    Shape.Influence = 1.1f * HMax(Radius) / Shape.RotationScale;
    return Shape;
}

static glsdf_shape
GLSDF_Cube(vec3 Position, quat Rotation, vec4 Color, f32 Radius)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,Radius,Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Rotation;
    Shape.RotationScale = GLSDF_RotationScale(Rotation);
    Shape.Color = Color;
    Shape.Smoothing = 0.0f;
    Shape.ShapeOp = (GLSDF_CUBOID << 2) | GLSDF_ADD;
    Shape.Influence = 1.1f * Radius / Shape.RotationScale;
    return Shape;
}

static glsdf_shape
GLSDF_Cuboid(vec3 Position, quat Rotation, vec4 Color, vec3 Radius)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Rotation;
    Shape.RotationScale = GLSDF_RotationScale(Rotation);
    Shape.Color = Color;
    Shape.Smoothing = 0.0f;
    Shape.ShapeOp = (GLSDF_CUBOID << 2) | GLSDF_ADD;
    Shape.Influence = 1.1f * HMax(Radius) / Shape.RotationScale;
    return Shape;
}
