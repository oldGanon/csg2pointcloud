
struct vec3x4
{
    f32x4 x,y,z;

#ifdef __cplusplus

    vec3x4(f32x4 F)  : x(F),y(F),z(F) { };
    vec3x4(f32x4 X, f32x4 Y, f32x4 Z)  : x(X),y(Y),z(Z) { };
    vec3x4(f32 F)  : x(F),y(F),z(F) { };
    vec3x4(vec3 V)  : x(V.x),y(V.y),z(V.z) { };

#endif
};

// CONSTRUCTORS
inline vec3x4 Vec3x4_Zero(void)                 { vec3x4 Result = { F32x4_Zero(), F32x4_Zero(), F32x4_Zero() }; return Result; }
inline vec3x4 Vec3x4_Set1(f32x4 f)              { vec3x4 Result = { f, f, f }; return Result; }
inline vec3x4 Vec3x4(f32x4 x, f32x4 y, f32x4 z) { vec3x4 Result = { x, y, z }; return Result; }

// ARITHMETIC
inline vec3x4 Vec3x4_Add(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Add(A.x, B.x), F32x4_Add(A.y, B.y), F32x4_Add(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Sub(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Sub(A.x, B.x), F32x4_Sub(A.y, B.y), F32x4_Sub(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Mul(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Mul(A.x, B.x), F32x4_Mul(A.y, B.y), F32x4_Mul(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Div(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Div(A.x, B.x), F32x4_Div(A.y, B.y), F32x4_Div(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Negate(vec3x4 A)        { vec3x4 Result = { F32x4_Negate(A.x), F32x4_Negate(A.y), F32x4_Negate(A.z) }; return Result; }

// HELPER FUNCTIONS
inline vec3x4 Vec3x4_Round(vec3x4 A)         { vec3x4 Result = { F32x4_Round(A.x),    F32x4_Round(A.y),    F32x4_Round(A.z) };    return Result; }
inline vec3x4 Vec3x4_Truncate(vec3x4 A)      { vec3x4 Result = { F32x4_Truncate(A.x), F32x4_Truncate(A.y), F32x4_Truncate(A.z) }; return Result; }
inline vec3x4 Vec3x4_Ceil(vec3x4 A)          { vec3x4 Result = { F32x4_Ceil(A.x),     F32x4_Ceil(A.y),     F32x4_Ceil(A.z) };     return Result; }
inline vec3x4 Vec3x4_Floor(vec3x4 A)         { vec3x4 Result = { F32x4_Floor(A.x),    F32x4_Floor(A.y),    F32x4_Floor(A.z) };    return Result; }
inline vec3x4 Vec3x4_Fract(vec3x4 A)         { vec3x4 Result = { F32x4_Fract(A.x),    F32x4_Fract(A.y),    F32x4_Fract(A.z) };    return Result; }
inline vec3x4 Vec3x4_Mod(vec3x4 A, vec3x4 D) { vec3x4 Result = { F32x4_Mod(A.x, D.x), F32x4_Mod(A.y, D.y), F32x4_Mod(A.z, D.z) }; return Result; }
inline vec3x4 Vec3x4_Saturate(vec3x4 A)      { vec3x4 Result = { F32x4_Saturate(A.x), F32x4_Saturate(A.y), F32x4_Saturate(A.z) }; return Result; }
inline vec3x4 Vec3x4_Sign(vec3x4 A)			 { vec3x4 Result = { F32x4_Sign(A.x),     F32x4_Sign(A.y),     F32x4_Sign(A.z) };     return Result; }
inline vec3x4 Vec3x4_Positive(vec3x4 A)      { vec3x4 Result = { F32x4_Positive(A.x), F32x4_Positive(A.y), F32x4_Positive(A.z) }; return Result; }
inline vec3x4 Vec3x4_Negative(vec3x4 A)      { vec3x4 Result = { F32x4_Negative(A.x), F32x4_Negative(A.y), F32x4_Negative(A.z) }; return Result; }
inline vec3x4 Vec3x4_Abs(vec3x4 A)           { vec3x4 Result = { F32x4_Abs(A.x),      F32x4_Abs(A.y),      F32x4_Abs(A.z) };      return Result; }
inline vec3x4 Vec3x4_Sqrt(vec3x4 A)          { vec3x4 Result = { F32x4_Sqrt(A.x),     F32x4_Sqrt(A.y),     F32x4_Sqrt(A.z) };     return Result; }
inline vec3x4 Vec3x4_Min(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Min(A.x, B.x), F32x4_Min(A.y, B.y), F32x4_Min(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Max(vec3x4 A, vec3x4 B) { vec3x4 Result = { F32x4_Max(A.x, B.x), F32x4_Max(A.y, B.y), F32x4_Max(A.z, B.z) }; return Result; }
inline vec3x4 Vec3x4_Lerp(vec3x4 A, vec3x4 B, f32x4 T) 		 { vec3x4 Result = { F32x4_Lerp(A.x, B.x, T), F32x4_Lerp(A.y, B.y, T), F32x4_Lerp(A.z, B.z, T) }; return Result; }
inline vec3x4 Vec3x4_Clamp(vec3x4 A, vec3x4 Min, vec3x4 Max) { vec3x4 Result = { F32x4_Clamp(A.x, Min.x, Max.x), F32x4_Clamp(A.y, Min.y, Max.y), F32x4_Clamp(A.z, Min.z, Max.z) }; return Result; }
inline f32x4 Vec3x4_HMax(vec3x4 A) { return F32x4_Max(F32x4_Max(A.x, A.y), A.z); }
inline f32x4 Vec3x4_HMin(vec3x4 A) { return F32x4_Min(F32x4_Min(A.x, A.y), A.z); }
inline f32x4 Vec3x4_Sum(vec3x4 A)  { return F32x4_Add(F32x4_Add(A.x, A.y), A.z); }

// TRIGONOMETRY
inline vec3x4 Vec3x4_Sin(vec3x4 X);
inline vec3x4 Vec3x4_Cos(vec3x4 X);
inline vec3x4 Vec3x4_Tan(vec3x4 X);
inline vec3x4 Vec3x4_Arcsin(vec3x4 X);
inline vec3x4 Vec3x4_Arccos(vec3x4 X);
inline vec3x4 Vec3x4_Arctan(vec3x4 X);
inline vec3x4 Vec3x4_Arctan2(vec3x4 X, vec3x4 Y);

// LINEAR ALGEBRA
inline f32x4 Vec3x4_LengthSq(vec3x4 A)        { return Vec3x4_Sum(Vec3x4_Mul(A,A)); }
inline f32x4 Vec3x4_Length(vec3x4 A)          { return F32x4_Sqrt(Vec3x4_LengthSq(A)); }
inline f32x4 Vec3x4_Angle(vec3x4 A, vec3x4 B);
inline f32x4 Vec3x4_Dot(vec3x4 A, vec3x4 B)   { return Vec3x4_Sum(Vec3x4_Mul(A, B)); }
inline vec3x4 Vec3x4_Log(vec3x4 X);
inline vec3x4 Vec3x4_Exp(vec3x4 X);
inline vec3x4 Vec3x4_Pow(vec3x4 X, vec3x4 Y);
inline vec3x4 Vec3x4_Normalize(vec3x4 A);

// UNIQUE FUNCTIONS
inline vec3x4
Vec3x4_Cross(vec3x4 A, vec3x4 B)
{
    f32x4 X = F32x4_Sub(F32x4_Mul(A.y, B.z), F32x4_Mul(A.z, B.y));
    f32x4 Y = F32x4_Sub(F32x4_Mul(A.z, B.x), F32x4_Mul(A.x, B.z));
    f32x4 Z = F32x4_Sub(F32x4_Mul(A.x, B.y), F32x4_Mul(A.y, B.x));
    return Vec3x4(X,Y,Z);
}

#ifdef __cplusplus

// OPERATORS
inline vec3x4 operator+ (vec3x4 A, vec3x4 B) { return Vec3x4_Add(A, B); }
inline vec3x4 operator- (vec3x4 A, vec3x4 B) { return Vec3x4_Sub(A, B); }
inline vec3x4 operator* (vec3x4 A, vec3x4 B) { return Vec3x4_Mul(A, B); }
inline vec3x4 operator/ (vec3x4 A, vec3x4 B) { return Vec3x4_Div(A, B); }
inline vec3x4 operator- (vec3x4 A)           { return Vec3x4_Negate(A); }

inline void operator+= (vec3x4 &A, vec3x4 B) { A = Vec3x4_Add(A, B); }
inline void operator-= (vec3x4 &A, vec3x4 B) { A = Vec3x4_Sub(A, B); }
inline void operator*= (vec3x4 &A, vec3x4 B) { A = Vec3x4_Mul(A, B); }
inline void operator/= (vec3x4 &A, vec3x4 B) { A = Vec3x4_Div(A, B); }

// inline vec3x4 operator== (vec3x4 A, vec3x4 B) { return Vec3x4_Equal(A, B); }
// inline vec3x4 operator!= (vec3x4 A, vec3x4 B) { return Vec3x4_NotEqual(A, B); }
// inline vec3x4 operator>= (vec3x4 A, vec3x4 B) { return Vec3x4_GreaterEqual(A, B); }
// inline vec3x4 operator>  (vec3x4 A, vec3x4 B) { return Vec3x4_GreaterThan(A, B); }
// inline vec3x4 operator<= (vec3x4 A, vec3x4 B) { return Vec3x4_LessEqual(A, B); }
// inline vec3x4 operator<  (vec3x4 A, vec3x4 B) { return Vec3x4_LessThan(A, B); }

// ARITHMETIC
inline vec3x4 Add(vec3x4 A, vec3x4 B)              { return Vec3x4_Add(A, B); }
inline vec3x4 Sub(vec3x4 A, vec3x4 B)              { return Vec3x4_Sub(A, B); }
inline vec3x4 Mul(vec3x4 A, vec3x4 B)              { return Vec3x4_Mul(A, B); }
inline vec3x4 Div(vec3x4 A, vec3x4 B)              { return Vec3x4_Div(A, B); }
inline vec3x4 Negate(vec3x4 A)                     { return Vec3x4_Negate(A); }
// inline vec3x4 MulAdd(vec3x4 A, vec3x4 B, vec3x4 C) { return Vec3x4_MulAdd(A, B, C); }

// HELPER FUNCTIONS
inline vec3x4 Round(vec3x4 A)                         { return Vec3x4_Round(A); }
inline vec3x4 Truncate(vec3x4 A)                      { return Vec3x4_Truncate(A); }
inline vec3x4 Ceil(vec3x4 A)                          { return Vec3x4_Ceil(A); }
inline vec3x4 Floor(vec3x4 A)                         { return Vec3x4_Floor(A); }
inline vec3x4 Fract(vec3x4 A)                         { return Vec3x4_Fract(A); }
inline vec3x4 Mod(vec3x4 A, vec3x4 D)                 { return Vec3x4_Mod(A, D); }
inline vec3x4 Saturate(vec3x4 A)                      { return Vec3x4_Saturate(A); }
inline vec3x4 Sign(vec3x4 A)                          { return Vec3x4_Sign(A); }
inline vec3x4 Positive(vec3x4 A)                      { return Vec3x4_Positive(A); }
inline vec3x4 Negative(vec3x4 A)                      { return Vec3x4_Negative(A); }
inline vec3x4 Abs(vec3x4 A)                           { return Vec3x4_Abs(A); }
inline vec3x4 Sqrt(vec3x4 A)                          { return Vec3x4_Sqrt(A); }
inline vec3x4 Min(vec3x4 A, vec3x4 B)                 { return Vec3x4_Min(A, B); }
inline vec3x4 Max(vec3x4 A, vec3x4 B)                 { return Vec3x4_Max(A, B); }
inline vec3x4 Lerp(vec3x4 A, vec3x4 B, f32x4 T)       { return Vec3x4_Lerp(A, B, T); }
inline vec3x4 Clamp(vec3x4 A, vec3x4 Min, vec3x4 Max) { return Vec3x4_Clamp(A, Min, Max); }
inline f32x4 HMin(vec3x4 A)                           { return Vec3x4_HMin(A); }
inline f32x4 HMax(vec3x4 A)                           { return Vec3x4_HMax(A); }
inline f32x4 Sum(vec3x4 A)                            { return Vec3x4_Sum(A); }

// TRIGONOMETRY
inline vec3x4 Sin(vec3x4 X)               { return Vec3x4_Sin(X); }
inline vec3x4 Cos(vec3x4 X)               { return Vec3x4_Cos(X); }
inline vec3x4 Tan(vec3x4 X)               { return Vec3x4_Tan(X); }
inline vec3x4 Arcsin(vec3x4 X)            { return Vec3x4_Arcsin(X); }
inline vec3x4 Arccos(vec3x4 X)            { return Vec3x4_Arccos(X); }
inline vec3x4 Arctan(vec3x4 X)            { return Vec3x4_Arctan(X); }
inline vec3x4 Arctan2(vec3x4 X, vec3x4 Y) { return Vec3x4_Arctan2(X, Y); }

// LINEAR ALGEBRA
inline f32x4 LengthSq(vec3x4 A)        { return Vec3x4_LengthSq(A); }
inline f32x4 Length(vec3x4 A)          { return Vec3x4_Length(A); }
inline f32x4 Angle(vec3x4 A, vec3x4 B) { return Vec3x4_Angle(A, B); }
inline f32x4 Dot(vec3x4 A, vec3x4 B)   { return Vec3x4_Dot(A, B); }
inline vec3x4 Log(vec3x4 X)            { return Vec3x4_Log(X); }
inline vec3x4 Exp(vec3x4 X)            { return Vec3x4_Exp(X); }
inline vec3x4 Pow(vec3x4 X, vec3x4 Y)  { return Vec3x4_Pow(X, Y); }
inline vec3x4 Normalize(vec3x4 A)      { return Vec3x4_Normalize(A); }

// UNIQUE FUNCTIONS
inline vec3x4 Cross(vec3x4 A, vec3x4 B) { return Vec3x4_Cross(A, B); }

#endif

inline vec3x4
Quat_MulVec3x4(quat A, vec3x4 B)
{
    vec3x4 T = Vec3x4_Mul(Vec3x4_Cross(A.Imag, B), Vec3x4_Set1(2.0f));
    return Vec3x4_Add(B, Vec3x4_Add(Vec3x4_Mul(T, Vec3x4_Set1(A.Real)), Vec3x4_Cross(A.Imag, T)));
}
inline vec3x4 operator*(const quat A, const vec3x4 B) { return Quat_MulVec3x4(A, B); }
