
#include "f32x8.cpp"

struct sdf_shape_sphere
{
    f32 Radius;
};

struct sdf_shape_cube
{
    f32 Radius;
};

enum sdf_shape_type
{
    SDF_SHAPE_SPHERE,
    SDF_SHAPE_CUBE,
};

struct sdf_shape
{
    union
    {
        sdf_shape_sphere Sphere;
        sdf_shape_cube Cube;
    };

    vec3 Position;
    u32 Type;
};

enum sdf_op_type
{
    SDF_OP_ADD, // UNION
    SDF_OP_SUB, // SUBTRACT
    SDF_OP_ADD_SMOOTH,
    SDF_OP_SUB_SMOOTH,
};

struct sdf_op
{
    u32 Type;
    f32 Smoothing;
};

struct sdf_edit
{
    sdf_shape Shape;
    sdf_op Op;
};

struct sdf
{
    u32 EditCount;
    sdf_edit Edits[64];
};



static sdf_shape
SDF_Sphere(vec3 Pos, f32 Radius)
{
    sdf_shape_sphere Sphere = { Radius };
    sdf_shape Shape;
    Shape.Sphere = Sphere;
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_SPHERE;
    return Shape;
}

static sdf_shape
SDF_Cube(vec3 Pos, f32 Radius)
{
    sdf_shape_cube Cube = { Radius };
    sdf_shape Shape;
    Shape.Cube = Cube;
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_CUBE;
    return Shape;
}

static void
SDF_AddEdit(sdf *SDF, sdf_shape Shape, sdf_op Op)
{
    u32 Index = SDF->EditCount++;
    SDF->Edits[Index].Shape = Shape;
    SDF->Edits[Index].Op = Op; 
}

static void
SDF_Add(sdf *SDF, sdf_shape Shape)
{
    SDF_AddEdit(SDF, Shape, { SDF_OP_ADD });
}

static void
SDF_Sub(sdf *SDF, sdf_shape Shape)
{
    SDF_AddEdit(SDF, Shape, { SDF_OP_SUB });
}

static void
SDF_AddSmooth(sdf *SDF, sdf_shape Shape, f32 Smoothing)
{
    SDF_AddEdit(SDF, Shape, { SDF_OP_ADD_SMOOTH, Smoothing });
}

static void
SDF_SubSmooth(sdf *SDF, sdf_shape Shape, f32 Smoothing)
{
    SDF_AddEdit(SDF, Shape, { SDF_OP_SUB_SMOOTH, Smoothing });
}



template <typename FLOAT>
static FLOAT
SDF_Add(FLOAT D1, FLOAT D2)
{
    return Min(D1, D2);
}

template <typename FLOAT>
static FLOAT
SDF_Sub(FLOAT D1, FLOAT D2)
{
    return Max(D1,-D2);
}

template <typename FLOAT>
static FLOAT
SDF_AddSmooth(FLOAT D1, FLOAT D2, FLOAT K)
{
    FLOAT H = Clamp(0.5f + 0.5f * (D2 - D1) / K, 0.0f, 1.0f);
    return Lerp(D2, D1, H) - K * H * (1.0f - H);
}

template <typename FLOAT>
static FLOAT
SDF_SubSmooth(FLOAT D1, FLOAT D2, FLOAT K)
{
    FLOAT H = Clamp(0.5f - 0.5f * (D2 + D1) / K, 0.0f, 1.0f);
    return Lerp(D2, -D1, H) + K * H * (1.0f - H);
}

template <typename FLOAT>
static FLOAT
SDF_Combine(const sdf_op Op, FLOAT D1, FLOAT D2)
{
    switch (Op.Type)
    {
        case SDF_OP_ADD: return SDF_Add(D1, D2); break;
        case SDF_OP_SUB: return SDF_Sub(D1, D2); break;
        case SDF_OP_ADD_SMOOTH: return SDF_AddSmooth(D1, D2, (FLOAT)Op.Smoothing); break;
        case SDF_OP_SUB_SMOOTH: return SDF_SubSmooth(D1, D2, (FLOAT)Op.Smoothing); break;
        default: return INFINITY; break;
    }
}



template <typename FLOAT>
static FLOAT
SDF_SphereMax(const sdf_shape_sphere Sphere, FLOAT X, FLOAT Y, FLOAT Z)
{
    X = Abs(X);
    Y = Abs(Y);
    Z = Abs(Z);
    FLOAT R = Sphere.Radius;

    FLOAT XYZ = X+Y+Z;
    FLOAT M = 3.0f*R*R-2.0f*(X*X-X*(Y+Z)+Y*Y-Y*Z+Z*Z);
    FLOAT S = Sqrt(M);
    FLOAT V0 = 0.33333333333f*( S+XYZ);
    FLOAT V1 = 0.33333333333f*(-S+XYZ);
    FLOAT V = Blend(V0,V1,Abs(V0)>Abs(V1));
    V = Blend(V, INFINITY, M < 0);

    FLOAT A = Min(Min(X,Y),Z);
    FLOAT B = Max(Max(Min(X,Y),Min(X,Z)),Min(Y,Z));
    FLOAT C = Max(Max(X,Y),Z);

    FLOAT BPC = B+C;
    FLOAT BMC = B-C;
    FLOAT N = 2.0f*R*R-BMC*BMC;
    FLOAT K = Sqrt(N);
    FLOAT E0 = 0.5f*( K+BPC);
    FLOAT E1 = 0.5f*(-K+BPC);
    FLOAT E = Blend(E0,E1,Abs(E0)>Abs(E1));
    E = Max(Blend(E, INFINITY, N < 0), A);

    FLOAT F = Abs(Max(C-R,B));

    return Min(Min(V,F),E);
}

template <typename FLOAT>
static FLOAT
SDF_CubeMax(const sdf_shape_cube Cube, FLOAT X, FLOAT Y, FLOAT Z)
{
    FLOAT R = Cube.Radius;
    X = Abs(X) - R;
    Y = Abs(Y) - R;
    Z = Abs(Z) - R;
    return Max(Max(X, Y), Z);
}

template <typename FLOAT>
static FLOAT
SDF_EvalMax(const sdf_shape Shape, FLOAT X, FLOAT Y, FLOAT Z)
{
    X -= Shape.Position.x;
    Y -= Shape.Position.y;
    Z -= Shape.Position.z;
    switch (Shape.Type)
    {
        case SDF_SHAPE_SPHERE: return SDF_SphereMax(Shape.Sphere, X, Y, Z);
        case SDF_SHAPE_CUBE:   return SDF_CubeMax(Shape.Cube, X, Y, Z);
        default:               return INFINITY;
    }
}

template <typename FLOAT>
static FLOAT
SDF_EvalMax(const sdf *SDF, FLOAT X, FLOAT Y, FLOAT Z)
{
    FLOAT Dist = INFINITY;
    for (u32 i = 0; i < SDF->EditCount; ++i)
    {
        FLOAT Dist2 = SDF_EvalMax(SDF->Edits[i].Shape, X, Y, Z);
        Dist = SDF_Combine(SDF->Edits[i].Op, Dist, Dist2);
    }
    return Dist;
}



static f32
SDF_Sphere(const sdf_shape_sphere Sphere, vec P)
{
    f32 D = Vec_Length(P) - Sphere.Radius;
    return D;
}

static f32
SDF_Cube(const sdf_shape_cube Cube, vec P)
{
    vec Dist = Vec_Abs(P) - Vec_Set1(Cube.Radius);
    return Vec_Length(Vec_Max(Dist,0.0)) + Min(Vec_HMax(Dist),0.0);
}

static f32
SDF_EvalShape(const sdf_shape Shape, vec P)
{
    P -= Shape.Position;
    switch (Shape.Type)
    {
        case SDF_SHAPE_SPHERE: return SDF_Sphere(Shape.Sphere, P);
        case SDF_SHAPE_CUBE:   return SDF_Cube(Shape.Cube, P);
        default: return INFINITY;
    }
}

static f32
SDF_Eval(const sdf *SDF, vec3 Pos)
{
    vec P = Pos;
    f32 Dist = INFINITY;
    for (u32 i = 0; i < SDF->EditCount; ++i)
    {
        f32 Dist2 = SDF_EvalShape(SDF->Edits[i].Shape, P);
        Dist = SDF_Combine(SDF->Edits[i].Op, Dist, Dist2);
    }
    return Dist;
}

static vec3
SDF_Normal(const sdf *SDF, vec3 Position)
{
#define H 0.0001f
    vec P = Position;
    return Vec_Normalize(Vec( 1,-1,-1, 0) * SDF_Eval(SDF, P + Vec( H,-H,-H, 0)) + 
                         Vec(-1,-1, 1, 0) * SDF_Eval(SDF, P + Vec(-H,-H, H, 0)) + 
                         Vec(-1, 1,-1, 0) * SDF_Eval(SDF, P + Vec(-H, H,-H, 0)) + 
                         Vec( 1, 1, 1, 0) * SDF_Eval(SDF, P + Vec( H, H, H, 0)));
}

struct splat
{
    vec3 Position;
    vec3 Normal;
};

splat SPLATS[10000000];
u64 SPLATCOUNT;

static void
SDF_Gen8(const sdf *SDF, vec3 P, f32 Dim, u32 Depth)
{
    Dim /= 2.0f;
    f32x8 vDim = F32x8_Set1(Dim);
 
    f32x8 X = F32x8_Set1(P.x) + vDim * F32x8(-1,1,-1,1,-1,1,-1,1);
    f32x8 Y = F32x8_Set1(P.y) + vDim * F32x8(-1,-1,1,1,-1,-1,1,1);
    f32x8 Z = F32x8_Set1(P.z) + vDim * F32x8(-1,-1,-1,-1,1,1,1,1);

    f32x8 Dist = SDF_EvalMax(SDF, X, Y, Z);
    u32 InRange = F32x8_Mask(F32x8_Abs(Dist) <= vDim);
    if (!InRange) return;

    f32 XX[8];
    f32 YY[8];
    f32 ZZ[8];
    F32x8_Store8(X, XX);
    F32x8_Store8(Y, YY);
    F32x8_Store8(Z, ZZ);

    for (u8 x = 0; x < 8; ++x)
    if (InRange & (1 << x))
    {
        vec3 Q = Vec3(XX[x], YY[x], ZZ[x]);
        if (Depth)
            SDF_Gen8(SDF, Q, Dim, Depth - 1);
        else
        {
            f32 D = SDF_Eval(SDF, Q);
            vec3 Normal = SDF_Normal(SDF, Q);
            // Q -= Normal * D;
            SPLATS[SPLATCOUNT].Position = Q;
            SPLATS[SPLATCOUNT].Normal = Normal;
            ++SPLATCOUNT;
        }
    }
}

static void
SDF_Gen(const sdf *SDF)
{
    SPLATCOUNT = 0;
    u32 MaxDepth = 9; // (8 ^ (MaxDepth+1) splats upper bound
    SDF_Gen8(SDF, Vec3_Zero(), 1.0f, MaxDepth);
}
