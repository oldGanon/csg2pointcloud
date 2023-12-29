
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
    vec3 Position;
    u32 Normal;
    u32 Color;
};

static f32
GLSDF_RotationScale(quat Rotation)
{
    return 1.0f / HMax(Abs(Rotation * Vec3(1,1,1)));
}

static void
GLSDF_Undo(glsdf *GLSDF)
{
    --GLSDF->EditCount;
}

static void
GLSDF_Add(glsdf *GLSDF, glsdf_shape Edit)
{
    Edit.ShapeOp |= GLSDF_ADD;
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static void
GLSDF_Sub(glsdf *GLSDF, glsdf_shape Edit)
{
    Edit.ShapeOp |= GLSDF_SUB;
    GLSDF->Edits[GLSDF->EditCount++] = Edit;
}

static glsdf_shape
GLSDF_Sphere(vec3 Position, vec4 Color, f32 Radius, f32 Smoothing)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,Radius,Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f;
    Shape.Color = Color;
    Shape.Smoothing = Smoothing;
    Shape.ShapeOp = (GLSDF_ELLIPSOID << 2) | GLSDF_ADD;
    if (Smoothing > 0.0f) Shape.ShapeOp |= GLSDF_ADD_SMOOTH;
    Shape.Influence = 1.1f * (Smoothing + Radius);
    return Shape;
}

static glsdf_shape
GLSDF_Ellipsoid(vec3 Position, quat Rotation, vec4 Color, vec3 Radius, f32 Smoothing)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Rotation;
    Shape.RotationScale = GLSDF_RotationScale(Rotation);
    Shape.Color = Color;
    Shape.Smoothing =  Smoothing;
    Shape.ShapeOp = (GLSDF_ELLIPSOID << 2) | GLSDF_ADD;
    if (Smoothing > 0.0f) Shape.ShapeOp |= GLSDF_ADD_SMOOTH;
    Shape.Influence = 1.1f * (Smoothing + HMax(Radius) / Shape.RotationScale);
    return Shape;
}

static glsdf_shape
GLSDF_Cube(vec3 Position, quat Rotation, vec4 Color, f32 Radius, f32 Smoothing)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,Radius,Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Rotation;
    Shape.RotationScale = GLSDF_RotationScale(Rotation);
    Shape.Color = Color;
    Shape.Smoothing = Smoothing;
    Shape.ShapeOp = (GLSDF_CUBOID << 2) | GLSDF_ADD;
    if (Smoothing > 0.0f) Shape.ShapeOp |= GLSDF_ADD_SMOOTH;
    Shape.Influence = 1.1f * (Smoothing + Radius / Shape.RotationScale);
    return Shape;
}

static glsdf_shape
GLSDF_Cuboid(vec3 Position, quat Rotation, vec4 Color, vec3 Radius, f32 Smoothing)
{
    glsdf_shape Shape;
    Shape.Params = Vec4(Radius,0);
    Shape.Position = Vec4(Position,1);
    Shape.Rotation = Rotation;
    Shape.RotationScale = GLSDF_RotationScale(Rotation);
    Shape.Color = Color;
    Shape.Smoothing = Smoothing;
    Shape.ShapeOp = (GLSDF_CUBOID << 2) | GLSDF_ADD;
    if (Smoothing > 0.0f) Shape.ShapeOp |= GLSDF_ADD_SMOOTH;
    Shape.Influence = 1.1f * (Smoothing + HMax(Radius) / Shape.RotationScale);
    return Shape;
}



static f32
GLSDF_Range(glsdf_shape Edit, vec3 Position)
{
    return HMax(Abs(Position - Edit.Position.xyz)) - Edit.Influence;
}

static f32
GLSDF_Add(f32 D0, f32 D1)
{
    return ez_Min(D0, D1);
}

static f32
GLSDF_Sub(f32 D0, f32 D1)
{
    return ez_Max(D0,-D1);
}

static f32
GLSDF_SmoothAdd(f32 D0, f32 D1, f32 R)
{
    f32 E = ez_Max(R-ez_Abs(D0-D1),0);
    return ez_Min(D0,D1)-E*E*0.25f/R;
}

static f32
GLSDF_SmoothSub(f32 D0, f32 D1, f32 R)
{
    f32 E = ez_Max(R-ez_Abs(D0+D1),0);
    return ez_Max(D0,-D1)+E*E*0.25f/R;
}

static f32
GLSDF_EvalEllipsoid(glsdf_shape Shape, vec3 Position)
{
    Position = Abs(-Shape.Rotation * (Position - Shape.Position.xyz));
    vec3 R = Shape.Params.xyz;
    f32 K0 = Length(Position/R);
    f32 K1 = Length(Position/(R*R));
    return K0*(K0-1.0f)/K1;
}

static f32
GLSDF_EvalCuboid(glsdf_shape Shape, vec3 Position)
{
    Position = Abs(-Shape.Rotation * (Position - Shape.Position.xyz));
    vec3 D = Abs(Position) - Shape.Params.xyz;
    return Length(Max(D,0.0)) + ez_Min(HMax(D),0.0);
}

static f32
GLSDF_EvalEdit(vec3 Position, glsdf_shape Edit, f32 D0)
{
    f32 D1 = INFINITY;
    switch(Edit.ShapeOp / 4)
    {
        case 0: D1 = min(D1, GLSDF_EvalEllipsoid(Edit, Position)); break;
        case 1: D1 = min(D1, GLSDF_EvalCuboid(Edit, Position)); break;
        default: break;
    }
    switch(Edit.ShapeOp % 4)
    {
        case 0: D0 = GLSDF_Add(D0, D1); break;
        case 1: D0 = GLSDF_Sub(D0, D1); break;
        case 2: D0 = GLSDF_SmoothAdd(D0, D1, Edit.Smoothing); break;
        case 3: D0 = GLSDF_SmoothSub(D0, D1, Edit.Smoothing); break;
        default: break;
    }
    return D0;
}

static f32
GLSDF_Eval(const glsdf *SDF, vec3 Position)
{
    f32 D0 = INFINITY;
    for (u32 i = 0; i < SDF->EditCount; i++)
    {
        glsdf_shape Edit = SDF->Edits[i];
        D0 = GLSDF_EvalEdit(Position, Edit, D0);
    }
    return D0;
}

static f32
GLSDF_Dist(const glsdf *SDF, vec3 O, vec3 D, f32 W = 0.001)
{
    f32 t = 0.0f;
    for (int i = 0; i < 256; ++i)
    {
        f32 Dist = GLSDF_Eval(SDF, O + D * t);
        if (Dist < W) return t;
        t += Dist - W;
    }
    return t;
}
