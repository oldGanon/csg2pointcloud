
struct vec3x8
{
    f32x8 x,y,z;

#ifdef __cplusplus

    vec3x8(f32x8 F)  : x(F),y(F),z(F) { };
    vec3x8(f32x8 X, f32x8 Y, f32x8 Z)  : x(X),y(Y),z(Z) { };
    vec3x8(f32 F)  : x(F),y(F),z(F) { };
    vec3x8(vec3 V)  : x(V.x),y(V.y),z(V.z) { };

#endif
};

// CONSTRUCTORS
inline vec3x8 Vec3x8_Zero(void)                 { vec3x8 Result = { F32x8_Zero(), F32x8_Zero(), F32x8_Zero() }; return Result; }
inline vec3x8 Vec3x8_Set1(f32x8 f)              { vec3x8 Result = { f, f, f }; return Result; }
inline vec3x8 Vec3x8(f32x8 x, f32x8 y, f32x8 z) { vec3x8 Result = { x, y, z }; return Result; }

// ARITHMETIC
inline vec3x8 Vec3x8_Add(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Add(A.x, B.x), F32x8_Add(A.y, B.y), F32x8_Add(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Sub(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Sub(A.x, B.x), F32x8_Sub(A.y, B.y), F32x8_Sub(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Mul(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Mul(A.x, B.x), F32x8_Mul(A.y, B.y), F32x8_Mul(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Div(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Div(A.x, B.x), F32x8_Div(A.y, B.y), F32x8_Div(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Negate(vec3x8 A)        { vec3x8 Result = { F32x8_Negate(A.x), F32x8_Negate(A.y), F32x8_Negate(A.z) }; return Result; }

// HELPER FUNCTIONS
inline vec3x8 Vec3x8_Round(vec3x8 A)         { vec3x8 Result = { F32x8_Round(A.x),    F32x8_Round(A.y),    F32x8_Round(A.z) };    return Result; }
inline vec3x8 Vec3x8_Truncate(vec3x8 A)      { vec3x8 Result = { F32x8_Truncate(A.x), F32x8_Truncate(A.y), F32x8_Truncate(A.z) }; return Result; }
inline vec3x8 Vec3x8_Ceil(vec3x8 A)          { vec3x8 Result = { F32x8_Ceil(A.x),     F32x8_Ceil(A.y),     F32x8_Ceil(A.z) };     return Result; }
inline vec3x8 Vec3x8_Floor(vec3x8 A)         { vec3x8 Result = { F32x8_Floor(A.x),    F32x8_Floor(A.y),    F32x8_Floor(A.z) };    return Result; }
inline vec3x8 Vec3x8_Fract(vec3x8 A)         { vec3x8 Result = { F32x8_Fract(A.x),    F32x8_Fract(A.y),    F32x8_Fract(A.z) };    return Result; }
inline vec3x8 Vec3x8_Mod(vec3x8 A, vec3x8 D) { vec3x8 Result = { F32x8_Mod(A.x, D.x), F32x8_Mod(A.y, D.y), F32x8_Mod(A.z, D.z) }; return Result; }
inline vec3x8 Vec3x8_Saturate(vec3x8 A)      { vec3x8 Result = { F32x8_Saturate(A.x), F32x8_Saturate(A.y), F32x8_Saturate(A.z) }; return Result; }
inline vec3x8 Vec3x8_Sign(vec3x8 A)			 { vec3x8 Result = { F32x8_Sign(A.x),     F32x8_Sign(A.y),     F32x8_Sign(A.z) };     return Result; }
inline vec3x8 Vec3x8_Positive(vec3x8 A)      { vec3x8 Result = { F32x8_Positive(A.x), F32x8_Positive(A.y), F32x8_Positive(A.z) }; return Result; }
inline vec3x8 Vec3x8_Negative(vec3x8 A)      { vec3x8 Result = { F32x8_Negative(A.x), F32x8_Negative(A.y), F32x8_Negative(A.z) }; return Result; }
inline vec3x8 Vec3x8_Abs(vec3x8 A)           { vec3x8 Result = { F32x8_Abs(A.x),      F32x8_Abs(A.y),      F32x8_Abs(A.z) };      return Result; }
inline vec3x8 Vec3x8_Sqrt(vec3x8 A)          { vec3x8 Result = { F32x8_Sqrt(A.x),     F32x8_Sqrt(A.y),     F32x8_Sqrt(A.z) };     return Result; }
inline vec3x8 Vec3x8_Min(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Min(A.x, B.x), F32x8_Min(A.y, B.y), F32x8_Min(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Max(vec3x8 A, vec3x8 B) { vec3x8 Result = { F32x8_Max(A.x, B.x), F32x8_Max(A.y, B.y), F32x8_Max(A.z, B.z) }; return Result; }
inline vec3x8 Vec3x8_Lerp(vec3x8 A, vec3x8 B, f32x8 T) 		 { vec3x8 Result = { F32x8_Lerp(A.x, B.x, T), F32x8_Lerp(A.y, B.y, T), F32x8_Lerp(A.z, B.z, T) }; return Result; }
inline vec3x8 Vec3x8_Clamp(vec3x8 A, vec3x8 Min, vec3x8 Max) { vec3x8 Result = { F32x8_Clamp(A.x, Min.x, Max.x), F32x8_Clamp(A.y, Min.y, Max.y), F32x8_Clamp(A.z, Min.z, Max.z) }; return Result; }
inline f32x8 Vec3x8_HMax(vec3x8 A) { return F32x8_Max(F32x8_Max(A.x, A.y), A.z); }
inline f32x8 Vec3x8_HMin(vec3x8 A) { return F32x8_Min(F32x8_Min(A.x, A.y), A.z); }
inline f32x8 Vec3x8_Sum(vec3x8 A)  { return F32x8_Add(F32x8_Add(A.x, A.y), A.z); }

// TRIGONOMETRY
inline vec3x8 Vec3x8_Sin(vec3x8 X);
inline vec3x8 Vec3x8_Cos(vec3x8 X);
inline vec3x8 Vec3x8_Tan(vec3x8 X);
inline vec3x8 Vec3x8_Arcsin(vec3x8 X);
inline vec3x8 Vec3x8_Arccos(vec3x8 X);
inline vec3x8 Vec3x8_Arctan(vec3x8 X);
inline vec3x8 Vec3x8_Arctan2(vec3x8 X, vec3x8 Y);

// LINEAR ALGEBRA
inline f32x8 Vec3x8_LengthSq(vec3x8 A)        { return Vec3x8_Sum(Vec3x8_Mul(A,A)); }
inline f32x8 Vec3x8_Length(vec3x8 A)          { return F32x8_Sqrt(Vec3x8_LengthSq(A)); }
inline f32x8 Vec3x8_Angle(vec3x8 A, vec3x8 B);
inline f32x8 Vec3x8_Dot(vec3x8 A, vec3x8 B)   { return Vec3x8_Sum(Vec3x8_Mul(A, B)); }
inline vec3x8 Vec3x8_Log(vec3x8 X);
inline vec3x8 Vec3x8_Exp(vec3x8 X);
inline vec3x8 Vec3x8_Pow(vec3x8 X, vec3x8 Y);
inline vec3x8 Vec3x8_Normalize(vec3x8 A);

// UNIQUE FUNCTIONS
inline vec3x8
Vec3x8_Cross(vec3x8 A, vec3x8 B)
{
    f32x8 X = F32x8_Sub(F32x8_Mul(A.y, B.z), F32x8_Mul(A.z, B.y));
    f32x8 Y = F32x8_Sub(F32x8_Mul(A.z, B.x), F32x8_Mul(A.x, B.z));
    f32x8 Z = F32x8_Sub(F32x8_Mul(A.x, B.y), F32x8_Mul(A.y, B.x));
    return Vec3x8(X,Y,Z);
}

#ifdef __cplusplus

// OPERATORS
inline vec3x8 operator+ (vec3x8 A, vec3x8 B) { return Vec3x8_Add(A, B); }
inline vec3x8 operator- (vec3x8 A, vec3x8 B) { return Vec3x8_Sub(A, B); }
inline vec3x8 operator* (vec3x8 A, vec3x8 B) { return Vec3x8_Mul(A, B); }
inline vec3x8 operator/ (vec3x8 A, vec3x8 B) { return Vec3x8_Div(A, B); }
inline vec3x8 operator- (vec3x8 A)           { return Vec3x8_Negate(A); }

inline void operator+= (vec3x8 &A, vec3x8 B) { A = Vec3x8_Add(A, B); }
inline void operator-= (vec3x8 &A, vec3x8 B) { A = Vec3x8_Sub(A, B); }
inline void operator*= (vec3x8 &A, vec3x8 B) { A = Vec3x8_Mul(A, B); }
inline void operator/= (vec3x8 &A, vec3x8 B) { A = Vec3x8_Div(A, B); }

// inline vec3x8 operator== (vec3x8 A, vec3x8 B) { return Vec3x8_Equal(A, B); }
// inline vec3x8 operator!= (vec3x8 A, vec3x8 B) { return Vec3x8_NotEqual(A, B); }
// inline vec3x8 operator>= (vec3x8 A, vec3x8 B) { return Vec3x8_GreaterEqual(A, B); }
// inline vec3x8 operator>  (vec3x8 A, vec3x8 B) { return Vec3x8_GreaterThan(A, B); }
// inline vec3x8 operator<= (vec3x8 A, vec3x8 B) { return Vec3x8_LessEqual(A, B); }
// inline vec3x8 operator<  (vec3x8 A, vec3x8 B) { return Vec3x8_LessThan(A, B); }

// ARITHMETIC
inline vec3x8 Add(vec3x8 A, vec3x8 B)              { return Vec3x8_Add(A, B); }
inline vec3x8 Sub(vec3x8 A, vec3x8 B)              { return Vec3x8_Sub(A, B); }
inline vec3x8 Mul(vec3x8 A, vec3x8 B)              { return Vec3x8_Mul(A, B); }
inline vec3x8 Div(vec3x8 A, vec3x8 B)              { return Vec3x8_Div(A, B); }
inline vec3x8 Negate(vec3x8 A)                     { return Vec3x8_Negate(A); }
// inline vec3x8 MulAdd(vec3x8 A, vec3x8 B, vec3x8 C) { return Vec3x8_MulAdd(A, B, C); }

// HELPER FUNCTIONS
inline vec3x8 Round(vec3x8 A)                         { return Vec3x8_Round(A); }
inline vec3x8 Truncate(vec3x8 A)                      { return Vec3x8_Truncate(A); }
inline vec3x8 Ceil(vec3x8 A)                          { return Vec3x8_Ceil(A); }
inline vec3x8 Floor(vec3x8 A)                         { return Vec3x8_Floor(A); }
inline vec3x8 Fract(vec3x8 A)                         { return Vec3x8_Fract(A); }
inline vec3x8 Mod(vec3x8 A, vec3x8 D)                 { return Vec3x8_Mod(A, D); }
inline vec3x8 Saturate(vec3x8 A)                      { return Vec3x8_Saturate(A); }
inline vec3x8 Sign(vec3x8 A)                          { return Vec3x8_Sign(A); }
inline vec3x8 Positive(vec3x8 A)                      { return Vec3x8_Positive(A); }
inline vec3x8 Negative(vec3x8 A)                      { return Vec3x8_Negative(A); }
inline vec3x8 Abs(vec3x8 A)                           { return Vec3x8_Abs(A); }
inline vec3x8 Sqrt(vec3x8 A)                          { return Vec3x8_Sqrt(A); }
inline vec3x8 Min(vec3x8 A, vec3x8 B)                 { return Vec3x8_Min(A, B); }
inline vec3x8 Max(vec3x8 A, vec3x8 B)                 { return Vec3x8_Max(A, B); }
inline vec3x8 Lerp(vec3x8 A, vec3x8 B, f32x8 T)       { return Vec3x8_Lerp(A, B, T); }
inline vec3x8 Clamp(vec3x8 A, vec3x8 Min, vec3x8 Max) { return Vec3x8_Clamp(A, Min, Max); }
inline f32x8 HMin(vec3x8 A)                           { return Vec3x8_HMin(A); }
inline f32x8 HMax(vec3x8 A)                           { return Vec3x8_HMax(A); }
inline f32x8 Sum(vec3x8 A)                            { return Vec3x8_Sum(A); }

// TRIGONOMETRY
inline vec3x8 Sin(vec3x8 X)               { return Vec3x8_Sin(X); }
inline vec3x8 Cos(vec3x8 X)               { return Vec3x8_Cos(X); }
inline vec3x8 Tan(vec3x8 X)               { return Vec3x8_Tan(X); }
inline vec3x8 Arcsin(vec3x8 X)            { return Vec3x8_Arcsin(X); }
inline vec3x8 Arccos(vec3x8 X)            { return Vec3x8_Arccos(X); }
inline vec3x8 Arctan(vec3x8 X)            { return Vec3x8_Arctan(X); }
inline vec3x8 Arctan2(vec3x8 X, vec3x8 Y) { return Vec3x8_Arctan2(X, Y); }

// LINEAR ALGEBRA
inline f32x8 LengthSq(vec3x8 A)        { return Vec3x8_LengthSq(A); }
inline f32x8 Length(vec3x8 A)          { return Vec3x8_Length(A); }
inline f32x8 Angle(vec3x8 A, vec3x8 B) { return Vec3x8_Angle(A, B); }
inline f32x8 Dot(vec3x8 A, vec3x8 B)   { return Vec3x8_Dot(A, B); }
inline vec3x8 Log(vec3x8 X)            { return Vec3x8_Log(X); }
inline vec3x8 Exp(vec3x8 X)            { return Vec3x8_Exp(X); }
inline vec3x8 Pow(vec3x8 X, vec3x8 Y)  { return Vec3x8_Pow(X, Y); }
inline vec3x8 Normalize(vec3x8 A)      { return Vec3x8_Normalize(A); }

// UNIQUE FUNCTIONS
inline vec3x8 Cross(vec3x8 A, vec3x8 B) { return Vec3x8_Cross(A, B); }

#endif

inline vec3x8
Quat_MulVec3x8(quat A, vec3x8 B)
{
    vec3x8 T = Vec3x8_Mul(Vec3x8_Cross(A.Imag, B), Vec3x8_Set1(2.0f));
    return Vec3x8_Add(B, Vec3x8_Add(Vec3x8_Mul(T, Vec3x8_Set1(A.Real)), Vec3x8_Cross(A.Imag, T)));
}
inline vec3x8 operator*(const quat A, const vec3x8 B) { return Quat_MulVec3x8(A, B); }
