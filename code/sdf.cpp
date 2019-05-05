
#include "f32x4.cpp"
#include "vec3x4.cpp"
#include "f32x8.cpp"
#include "vec3x8.cpp"

struct sdf_shape_sphere
{
    f32 Radius;
};

struct sdf_shape_ellipsoid
{
    vec3 HalfDim;
};

struct sdf_shape_cube
{
    f32 HalfDim;
};

struct sdf_shape_cuboid
{
    vec3 HalfDim;
};

struct sdf_shape_torus
{
    vec2 Radius;
};

enum sdf_shape_type
{
    SDF_SHAPE_SPHERE,
    SDF_SHAPE_ELLIPSOID,
    SDF_SHAPE_CUBE,
    SDF_SHAPE_CUBOID,
    SDF_SHAPE_TORUS,
    SDF_SHAPE_CYLINDER,
    SDF_SHAPE_CONE,
};

struct sdf_shape
{
    union
    {
        sdf_shape_sphere Sphere;
        sdf_shape_ellipsoid Ellipsoid;
        sdf_shape_cube Cube;
        sdf_shape_cuboid Cuboid;
    };

    quat Rotation;
    f32 RotationScale;
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
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f / HMax(Shape.Rotation * Vec3(1,1,1));
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_SPHERE;
    return Shape;
}

static sdf_shape
SDF_Ellipsoid(vec3 Pos, quat Rotation, vec3 HalfDim)
{
    sdf_shape_ellipsoid Ellipsoid = { HalfDim };
    sdf_shape Shape;
    Shape.Ellipsoid = Ellipsoid;
    Shape.Rotation = Rotation;
    Shape.RotationScale = 1.0f / HMax(Shape.Rotation * Vec3(1,1,1));
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_ELLIPSOID;
    return Shape;
}

static sdf_shape
SDF_Cube(vec3 Pos, f32 HalfDim)
{
    sdf_shape_cube Cube = { HalfDim };
    sdf_shape Shape;
    Shape.Cube = Cube;
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f / HMax(Shape.Rotation * Vec3(1,1,1));
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_CUBE;
    return Shape;
}

static sdf_shape
SDF_Cuboid(vec3 Pos, quat Rotation, vec3 HalfDim)
{
    sdf_shape_cuboid Cuboid = { HalfDim };
    sdf_shape Shape;
    Shape.Cuboid = Cuboid;
    Shape.Rotation = Rotation;
    Shape.RotationScale = 1.0f / HMax(Shape.Rotation * Vec3(1,1,1));
    Shape.Position = Pos;
    Shape.Type = SDF_SHAPE_CUBOID;
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
SDF_AddSmooth(FLOAT A, FLOAT B, FLOAT R)
{
    FLOAT E = Max(R-Abs(A-B),0);
    return Min(A,B)-E*E*0.25f/R;
}

template <typename FLOAT>
static FLOAT
SDF_SubSmooth(FLOAT A, FLOAT B, FLOAT R)
{
    FLOAT E = Max(R-Abs(A+B),0);
    return Max(A,-B)+E*E*0.25f/R;
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



template <typename FLOAT, typename VEC3>
static FLOAT
SDF_SphereMax(const sdf_shape_sphere Sphere, VEC3 P)
{
    FLOAT X = Abs(P.x);
    FLOAT Y = Abs(P.y);
    FLOAT Z = Abs(P.z);
    FLOAT R = Sphere.Radius;

    FLOAT XYZ = X+Y+Z;
    FLOAT M = 3.0f*R*R-2.0f*(X*X-X*(Y+Z)+Y*Y-Y*Z+Z*Z);
    FLOAT S = Sqrt(M);
    FLOAT V0 = 0.33333333333f*( S+XYZ);
    FLOAT V1 = 0.33333333333f*(-S+XYZ);
    FLOAT V = Min(V0,V1);
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
    FLOAT E = Min(E0,E1);
    E = Max(Blend(E, INFINITY, N < 0), A);

    FLOAT F = Abs(Max(C-R,B));

    return Min(Min(V,F),E);
}

template <typename FLOAT>
static FLOAT
SDF_Quadratic(FLOAT A, FLOAT B, FLOAT C)
{
    FLOAT M = B*B-4.0f*A*C;
    FLOAT R0 = -B/(2.0f*A);
    FLOAT R1 = Sqrt(M)/(2.0f*A);
    FLOAT R = Min(Abs(R0-R1),Abs(R0+R1));
    return Blend(R, INFINITY, M < 0.0f);
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_EllipsoidMax(const sdf_shape_ellipsoid Ellipsoid, VEC3 P)
{
    VEC3 C = Abs(P);
    VEC3 C2 = C*C;
    VEC3 R = Ellipsoid.HalfDim;
    VEC3 R2 = R*R;
    FLOAT D = INFINITY;

    VEC3 QA = 1.0f/R2;
    VEC3 QB = -2.0f*C/R2;
    VEC3 QC = C2/R2;

    FLOAT QCS = Sum(QC) - 1.0f;
    D = Min(D, SDF_Quadratic(Sum(QA), Sum(QB), QCS));
    D = Blend(D, -D, QCS <= 0.0f);
    if (All(QCS <= 0.0f)) return D;

    D = Min(D, Max(SDF_Quadratic(QA.y + QA.z, QB.y + QB.z, QC.y + QC.z - 1.0f), C.x));
    D = Min(D, Max(SDF_Quadratic(QA.x + QA.z, QB.x + QB.z, QC.x + QC.z - 1.0f), C.y));
    D = Min(D, Max(SDF_Quadratic(QA.x + QA.y, QB.x + QB.y, QC.x + QC.y - 1.0f), C.z));

    D = Min(D, Max(Max(C.x, C.y), Abs(C.z - R.z)));
    D = Min(D, Max(Max(C.x, Abs(C.y - R.y)), C.z));
    D = Min(D, Max(Max(Abs(C.x - R.x), C.y), C.z));

    return D;
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_CubeMax(const sdf_shape_cube Cube, VEC3 P)
{
    P = Abs(P) - Cube.HalfDim;
    return Max(Max(P.x, P.y), P.z);
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_CuboidMax(const sdf_shape_cuboid Cuboid, VEC3 P)
{
    P = Abs(P) - Cuboid.HalfDim;
    return Max(Max(P.x, P.y), P.z);
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_EvalMax(const sdf_shape Shape, VEC3 P)
{
    P -= Shape.Position;
    P = -Shape.Rotation * P;
    switch (Shape.Type)
    {
        case SDF_SHAPE_SPHERE:    return Shape.RotationScale * SDF_SphereMax<FLOAT>(Shape.Sphere, P);
        case SDF_SHAPE_ELLIPSOID: return Shape.RotationScale * SDF_EllipsoidMax<FLOAT>(Shape.Ellipsoid, P);
        case SDF_SHAPE_CUBE:      return Shape.RotationScale * SDF_CubeMax<FLOAT>(Shape.Cube, P);
        case SDF_SHAPE_CUBOID:    return Shape.RotationScale * SDF_CuboidMax<FLOAT>(Shape.Cuboid, P);
        default:                  return INFINITY;
    }
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_EvalMax(const sdf *SDF, VEC3 P)
{
    FLOAT Dist = INFINITY;
    for (u32 i = 0; i < SDF->EditCount; ++i)
    {
        FLOAT Dist2 = SDF_EvalMax<FLOAT>(SDF->Edits[i].Shape, P);
        Dist = SDF_Combine(SDF->Edits[i].Op, Dist, Dist2);
    }
    return Dist;
}



template <typename FLOAT, typename VEC3>
static FLOAT
SDF_Sphere(const sdf_shape_sphere Sphere, VEC3 P)
{
    FLOAT D = Length(P) - Sphere.Radius;
    return D;
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_Ellipsoid(const sdf_shape_ellipsoid Ellipsoid, VEC3 P)
{
    VEC3 R = Ellipsoid.HalfDim;
    FLOAT K0 = Length(P/R);
    FLOAT K1 = Length(P/(R*R));
    return K0*(K0-1.0f)/K1;
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_Cube(const sdf_shape_cube Cube, VEC3 P)
{
    VEC3 D = Abs(P) - Cube.HalfDim;
    return Length(Max(D,0.0)) + Min(HMax(D),0.0);
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_Cuboid(const sdf_shape_cuboid Cuboid, VEC3 P)
{
    VEC3 D = Abs(P) - Cuboid.HalfDim;
    return Length(Max(D,0.0)) + Min(HMax(D),0.0);
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_EvalShape(const sdf_shape Shape, VEC3 P)
{
    P -= Shape.Position;
    P = -Shape.Rotation * P;
    switch (Shape.Type)
    {
        case SDF_SHAPE_SPHERE:    return SDF_Sphere<FLOAT>(Shape.Sphere, P);
        case SDF_SHAPE_ELLIPSOID: return SDF_Ellipsoid<FLOAT>(Shape.Ellipsoid, P);
        case SDF_SHAPE_CUBE:      return SDF_Cube<FLOAT>(Shape.Cube, P);
        case SDF_SHAPE_CUBOID:    return SDF_Cuboid<FLOAT>(Shape.Cuboid, P);
        default: return INFINITY;
    }
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_Eval(const sdf *SDF, VEC3 Pos)
{
    FLOAT Dist = INFINITY;
    for (u32 i = 0; i < SDF->EditCount; ++i)
    {
        FLOAT Dist2 = SDF_EvalShape<FLOAT>(SDF->Edits[i].Shape, Pos);
        Dist = SDF_Combine(SDF->Edits[i].Op, Dist, Dist2);
    }
    return Dist;
}

static vec3x8
SDF_Normal(const sdf *SDF, vec3x8 Position)
{
#define H 0.001f
    return Vec3x8_Normalize(Vec3x8( 1,-1,-1) * SDF_Eval<f32x8>(SDF, Position + Vec3x8( H,-H,-H)) + 
                            Vec3x8(-1,-1, 1) * SDF_Eval<f32x8>(SDF, Position + Vec3x8(-H,-H, H)) + 
                            Vec3x8(-1, 1,-1) * SDF_Eval<f32x8>(SDF, Position + Vec3x8(-H, H,-H)) + 
                            Vec3x8( 1, 1, 1) * SDF_Eval<f32x8>(SDF, Position + Vec3x8( H, H, H)));
#undef H
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

    vec3x8 Pos = Vec3x8(X,Y,Z);
    f32x8 Dist = SDF_EvalMax<f32x8>(SDF, Pos);
    u32 InRange = F32x8_Mask(F32x8_Abs(Dist) <= vDim);
    if (!InRange) return;

    f32 XX[8];
    f32 YY[8];
    f32 ZZ[8];
    F32x8_Store8(Pos.x, XX);
    F32x8_Store8(Pos.y, YY);
    F32x8_Store8(Pos.z, ZZ);

    vec3x8 Nrm = SDF_Normal(SDF, Pos);

    f32 NX[8];
    f32 NY[8];
    f32 NZ[8];
    F32x8_Store8(Nrm.x, NX);
    F32x8_Store8(Nrm.y, NY);
    F32x8_Store8(Nrm.z, NZ);

    for (u8 x = 0; x < 8; ++x)
    if (InRange & (1 << x))
    {
        vec3 Q = Vec3(XX[x], YY[x], ZZ[x]);
        if (Depth)
            SDF_Gen8(SDF, Q, Dim, Depth - 1);
        else
        {
            // f32 D = SDF_Eval(SDF, Q);
            // vec3 Normal = SDF_Normal(SDF, Q);
            vec3 Normal = Vec3(NX[x], NY[x], NZ[x]);
            // Q -= Normal * Vec3_Set1(D);
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
