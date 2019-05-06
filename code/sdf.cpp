
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

struct sdf_shape_cylinder
{
    f32 HalfHeight;
    f32 Radius;
};

struct sdf_shape_cone
{
    f32 HalfHeight;
    f32 Radius;
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
        sdf_shape_torus Torus;
        sdf_shape_cylinder Cylinder;
        sdf_shape_cone Cone;
    };

    quat Rotation;
    f32 RotationScale;
    vec3 Position;
    vec3 Color;
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

static f32
SDF_RotationScale(quat Rotation)
{
    return 1.0f / HMax(Abs(Rotation * Vec3(1,1,1)));
}

static sdf_shape
SDF_Sphere(vec3 Position, vec3 Color, f32 Radius)
{
    sdf_shape_sphere Sphere = { Radius };
    sdf_shape Shape;
    Shape.Sphere = Sphere;
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = 1.0f;
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_SPHERE;
    return Shape;
}

static sdf_shape
SDF_Ellipsoid(vec3 Position, quat Rotation, vec3 Color, vec3 HalfDim)
{
    sdf_shape_ellipsoid Ellipsoid = { HalfDim };
    sdf_shape Shape;
    Shape.Ellipsoid = Ellipsoid;
    Shape.Rotation = Rotation;
    Shape.RotationScale = SDF_RotationScale(Shape.Rotation);
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_ELLIPSOID;
    return Shape;
}

static sdf_shape
SDF_Cube(vec3 Position, quat Rotation, vec3 Color, f32 HalfDim)
{
    sdf_shape_cube Cube = { HalfDim };
    sdf_shape Shape;
    Shape.Cube = Cube;
    Shape.Rotation = Quat_Id();
    Shape.RotationScale = SDF_RotationScale(Shape.Rotation);
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_CUBE;
    return Shape;
}

static sdf_shape
SDF_Cuboid(vec3 Position, quat Rotation, vec3 Color, vec3 HalfDim)
{
    sdf_shape_cuboid Cuboid = { HalfDim };
    sdf_shape Shape;
    Shape.Cuboid = Cuboid;
    Shape.Rotation = Rotation;
    Shape.RotationScale = SDF_RotationScale(Shape.Rotation);
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_CUBOID;
    return Shape;
}

static sdf_shape
SDF_Cylinder(vec3 Position, quat Rotation, vec3 Color, f32 HalfHeight, f32 Radius)
{
    sdf_shape_cylinder Cylinder = { HalfHeight, Radius };
    sdf_shape Shape;
    Shape.Cylinder = Cylinder;
    Shape.Rotation = Rotation;
    Shape.RotationScale = SDF_RotationScale(Shape.Rotation);
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_CYLINDER;
    return Shape;
}

static sdf_shape
SDF_Cone(vec3 Position, quat Rotation, vec3 Color, f32 HalfHeight, f32 Radius)
{
    sdf_shape_cone Cone = { HalfHeight, Radius };
    sdf_shape Shape;
    Shape.Cone = Cone;
    Shape.Rotation = Rotation;
    Shape.RotationScale = SDF_RotationScale(Shape.Rotation);
    Shape.Position = Position;
    Shape.Color = Color;
    Shape.Type = SDF_SHAPE_CONE;
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
    if (SDF->EditCount == 0) return;
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
    if (SDF->EditCount == 0) return;
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

template <typename FLOAT>
static FLOAT
SDF_Cubic(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
{
    FLOAT A = b/a;
    FLOAT B = c/a;
    FLOAT C = d/a;
    FLOAT AA = A*A;
    FLOAT P = 1.0f/3.0f*(-1.0f/3.0f*AA + B);
    FLOAT Q = 1.0f/2.0f*(2.0f/27.0f*A*AA - 1.0f/3.0f*A*B + C);

    FLOAT PPP = P*P*P;
    FLOAT D = Q*Q + PPP;
    FLOAT R = INFINITY;

    {
        // INCOMPLETE
    }

    R -= 1.0f/3.0f*A;
    return R;
}

template <typename FLOAT>
static FLOAT
SDF_Quartic(FLOAT a, FLOAT b, FLOAT c, FLOAT d, FLOAT e)
{
    FLOAT A = b/a;
    FLOAT B = c/a;
    FLOAT C = d/a;
    FLOAT D = e/a;
    FLOAT AA = A*A;
    FLOAT P = -3.0f/8.0f*AA + C;
    FLOAT Q = 1.0f/8.0f*AA*A - 1.0f/2.0f*A*B + C;
    FLOAT R = 3.0f/256*AA*AA + 1.0f/16.0f*AA*B - 1.0f/4.0f*A*C + D;

    // INCOMPLETE
}



template <typename FLOAT, typename VEC3>
static FLOAT
SDF_SphereMax(const sdf_shape_sphere Sphere, VEC3 P)
{
    P = Abs(P);
    FLOAT PS = Sum(P);
    VEC3 P2 = P*P;
    FLOAT P2S = Sum(P2);
    FLOAT R = Sphere.Radius;
    FLOAT R2 = R*R;

    FLOAT M = 3.0f*R2-2.0f*(P2S-P.x*P.y-P.x*P.z-P.y*P.z);
    FLOAT S = Sqrt(M);
    FLOAT V0 = 0.33333333333f*(PS+S);
    FLOAT V1 = 0.33333333333f*(PS-S);
    FLOAT V = Min(V0,V1);
    V = Blend(V, INFINITY, M < 0);

    FLOAT A = HMin(P);
    FLOAT B = Max(Max(Min(P.x,P.y),Min(P.x,P.z)),Min(P.y,P.z));
    FLOAT C = HMax(P);

    FLOAT BPC = B+C;
    FLOAT BMC = B-C;
    FLOAT N = 2.0f*R2-BMC*BMC;
    FLOAT K = Sqrt(N);
    FLOAT E0 = 0.5f*(BPC+K);
    FLOAT E1 = 0.5f*(BPC-K);
    FLOAT E = Min(E0,E1);
    E = Max(Blend(E, INFINITY, N < 0), A);

    FLOAT F = Abs(Max(C-R,B));

    return Min(Min(V,F),E);
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
SDF_TorusdMax(const sdf_shape_torus Torus, VEC3 P)
{
    P = Abs(P);
    VEC3 PP= P*P;
    FLOAT PS = Sum(P);
    FLOAT PPS = Sum(PP);
    FLOAT R = Torus.Radius.x;
    FLOAT r = Torus.Radius.y;
    FLOAT RR = R*R;
    FLOAT rr = r*r;
    FLOAT PPSrrRR = (PPS-rr-RR);

    FLOAT QA = 9.0f;
    FLOAT QB = 12.0f*PS;
    FLOAT QC = 6.0f*PPSrrRR+4.0f*Ps*Ps+4.0f*RR*PP.y;
    FLOAT QD = 4.0f*PPSrrRR*PS+8.0f*RR*P.y;
    FLOAT QE = PPSrrRR*PPSrrRR-4.0f*RR*(rr-PP.y);

    // INCOMPLETE
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_CylinderMax(const sdf_shape_cylinder Cylinder, VEC3 P)
{
    VEC3 C = Abs(P);
    VEC3 C2 = C*C;
    FLOAT R = Cylinder.Radius;
    FLOAT R2 = R*R;
    FLOAT H = Cylinder.HalfHeight;
    FLOAT D = INFINITY;

    FLOAT QAS = 2.0f;
    FLOAT QBS = -2.0f*(C.x+C.z);
    FLOAT QCS = (C2.x+C2.z) - R2;

    FLOAT A = C.y-H;
    FLOAT B = Sign(QCS) * SDF_Quadratic(QAS, QBS, QCS);
    D = Max(A, B);
    if (All(D <= 0)) return D;

    D = Min(D, Max(Max(C.x, C.y), Abs(C.z - R)));
    D = Min(D, Max(Max(C.y, C.z), Abs(C.x - R)));

    return D;
}

template <typename FLOAT, typename VEC3>
static FLOAT
SDF_EvalMax(const sdf_shape Shape, VEC3 P)
{
    P -= Shape.Position;
    P = -Shape.Rotation * P;
    FLOAT Dist = INFINITY;
    switch (Shape.Type)
    {
        case SDF_SHAPE_SPHERE:    Dist = SDF_SphereMax<FLOAT>(Shape.Sphere, P); break;
        case SDF_SHAPE_ELLIPSOID: Dist = SDF_EllipsoidMax<FLOAT>(Shape.Ellipsoid, P); break;
        case SDF_SHAPE_CUBE:      Dist = SDF_CubeMax<FLOAT>(Shape.Cube, P); break;
        case SDF_SHAPE_CUBOID:    Dist = SDF_CuboidMax<FLOAT>(Shape.Cuboid, P); break;
        case SDF_SHAPE_CYLINDER:  Dist = SDF_CylinderMax<FLOAT>(Shape.Cylinder, P); break;
        default:                  break;
    }
    return Shape.RotationScale * Dist;
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
SDF_Cylinder(const sdf_shape_cylinder Cylinder, VEC3 P)
{
    FLOAT X = Sqrt(P.x*P.x + P.z*P.z) - Cylinder.Radius;
    FLOAT Y = Abs(P.y) - Cylinder.HalfHeight;
    return Max(X,Y);
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
        case SDF_SHAPE_CYLINDER:  return SDF_Cylinder<FLOAT>(Shape.Cylinder, P);
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

static vec3x8
SDF_Color(const sdf *SDF, vec3x8 Pos)
{
    sdf_shape Shape = SDF->Edits[0].Shape;
    vec3x8 Color = Shape.Color;
    f32x8 D0 = SDF_EvalShape<f32x8>(Shape, Pos);
    for (u32 i = 1; i < SDF->EditCount; ++i)
    {
        Shape = SDF->Edits[i].Shape;
        f32x8 D1 = SDF_EvalShape<f32x8>(Shape, Pos);
        D1 = SDF_Combine(SDF->Edits[i].Op, D0, D1);
        f32x8 S = SDF->Edits[i].Op.Smoothing;
        f32x8 C = Saturate((D0-D1)/S);
        Color = Lerp(Color, Shape.Color, C);
        D0 = D1;
    }
    return Color;
}



struct splat
{
    vec3 Position;
    vec3 Normal;
    vec3 Color;
};

splat SPLATS[10000000];
atomic SPLATCOUNT;

static void
SDF_PlaceSplat(vec3 Position, vec3 Normal, vec3 Color)
{
    u64 Index = Atomic_Inc(&SPLATCOUNT);
    SPLATS[Index].Position = Position;
    SPLATS[Index].Normal = Normal;
    SPLATS[Index].Color = Color;
}

static void
SDF_Gen8(const sdf *SDF, vec3 Center, f32 Dim, u32 Depth)
{
    Dim /= 2.0f;
    f32x8 vDim = F32x8_Set1(Dim);
 
    f32x8 X = F32x8_Set1(Center.x) + vDim * F32x8(-1,1,-1,1,-1,1,-1,1);
    f32x8 Y = F32x8_Set1(Center.y) + vDim * F32x8(-1,-1,1,1,-1,-1,1,1);
    f32x8 Z = F32x8_Set1(Center.z) + vDim * F32x8(-1,-1,-1,-1,1,1,1,1);

    vec3x8 Pos = Vec3x8(X,Y,Z);
    f32x8 Dst = SDF_EvalMax<f32x8>(SDF, Pos);
    u32 InRange = F32x8_Mask(F32x8_Abs(Dst) <= vDim);
    if (!InRange) return;

    if (Depth)
    {
        for (u8 x = 0; x < 8; ++x)
        {
            if (InRange & (1 << x))
            {
                vec3 P = Vec3x8_First(Pos);
                SDF_Gen8(SDF, P, Dim, Depth - 1);
            }
            Pos = Vec3x8_Roll(Pos);
        }
    }
    else
    {
        vec3x8 Nrm = SDF_Normal(SDF, Pos);
        vec3x8 Col = SDF_Color(SDF, Pos);
        for (u8 x = 0; x < 8; ++x)
        {
            if (InRange & (1 << x))
            {
                vec3 P = Vec3x8_First(Pos);
                vec3 N = Vec3x8_First(Nrm);
                vec3 C = Vec3x8_First(Col);
                f32 D = F32x8_First(Dst);
                P -= N * Vec3_Set1(D);
                SDF_PlaceSplat(P, N, C);
            }
            Pos = Vec3x8_Roll(Pos);
            Nrm = Vec3x8_Roll(Nrm);
            Col = Vec3x8_Roll(Col);
            Dst = F32x8_Roll(Dst);
        }
    }
}

struct sdf_thread_data
{
    const sdf *SDF;
    vec3 Center;
    f32 Dim;
    u32 Depth;
};

static void
SDF_GenThread(void *Data)
{
    sdf_thread_data *SDFData = (sdf_thread_data *)Data;
    SDF_Gen8(SDFData->SDF, SDFData->Center, SDFData->Dim, SDFData->Depth);
}

static void
SDF_Gen(const sdf *SDF)
{
    SPLATCOUNT = 0;
    u32 MaxDepth = 8; // (8 ^ (MaxDepth) splats upper bound
    // SDF_Gen8(SDF, Vec3_Zero(), 1.0f, MaxDepth+1);

    sdf_thread_data ThreadData[8] = {
     { SDF, Vec3( 0.5f, 0.5f, 0.5f), 0.5f, MaxDepth },
     { SDF, Vec3( 0.5f, 0.5f,-0.5f), 0.5f, MaxDepth },
     { SDF, Vec3( 0.5f,-0.5f, 0.5f), 0.5f, MaxDepth },
     { SDF, Vec3( 0.5f,-0.5f,-0.5f), 0.5f, MaxDepth },
     { SDF, Vec3(-0.5f, 0.5f, 0.5f), 0.5f, MaxDepth },
     { SDF, Vec3(-0.5f, 0.5f,-0.5f), 0.5f, MaxDepth },
     { SDF, Vec3(-0.5f,-0.5f, 0.5f), 0.5f, MaxDepth },
     { SDF, Vec3(-0.5f,-0.5f,-0.5f), 0.5f, MaxDepth } };
    for (u32 i = 0; i < 8; ++i)
        Async_AddWork(SDF_GenThread, ThreadData + i);
    Async_FinishAllWork();
}
