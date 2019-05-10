
struct f32x4
{
    __m128 mm;

#ifdef __cplusplus

    f32x4(__m128 MM) : mm(MM) { };
    f32x4(f32 F)  : mm(_mm_set1_ps(F)) { };

#endif
};

// CONSTRUCTORS
inline f32x4 mmF32x4(__m128 mm) { f32x4 Result = { mm }; return Result; }
inline f32x4 F32x4(f32 x, f32 y, f32 z, f32 w) { return mmF32x4(_mm_set_ps(w,z,y,x)); }
inline f32x4 F32x4_Zero(void)   { return mmF32x4(_mm_setzero_ps()); }
inline f32x4 F32x4_Set1(f32 F)  { return mmF32x4(_mm_set1_ps(F)); };
inline void F32x4_Store4(f32x4 F, f32 *E) { _mm_storeu_ps(E, F.mm); }

// ARITHMETIC
inline f32x4 F32x4_Add(f32x4 A, f32x4 B) { return mmF32x4(_mm_add_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Sub(f32x4 A, f32x4 B) { return mmF32x4(_mm_sub_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Mul(f32x4 A, f32x4 B) { return mmF32x4(_mm_mul_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Div(f32x4 A, f32x4 B) { return mmF32x4(_mm_div_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Negate(f32x4 A)       { return mmF32x4(_mm_xor_ps(A.mm, _mm_set1_ps(-0.0f))); }

// COMPARISON
inline f32x4 F32x4_Equal       (f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_EQ_OQ)); }
inline f32x4 F32x4_NotEqual    (f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_NEQ_OQ)); }
inline f32x4 F32x4_GreaterEqual(f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_GE_OQ)); }
inline f32x4 F32x4_GreaterThan (f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_GT_OQ)); }
inline f32x4 F32x4_LessEqual   (f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_LE_OQ)); }
inline f32x4 F32x4_LessThan    (f32x4 A, f32x4 B) { return mmF32x4(_mm_cmp_ps(A.mm, B.mm, _CMP_LT_OQ)); }
inline b32 F32x4_Mask(f32x4 Mask) { return _mm_movemask_ps(Mask.mm); }
inline b32 F32x4_Any(f32x4 Mask)  { return _mm_movemask_ps(Mask.mm) != 0; }
inline b32 F32x4_All(f32x4 Mask)  { return _mm_movemask_ps(Mask.mm) == 255; }

// BIT MANIPULATION
inline f32x4 F32x4_And(f32x4 A, f32x4 B) { return mmF32x4(_mm_and_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Or(f32x4 A, f32x4 B)  { return mmF32x4(_mm_or_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Xor(f32x4 A, f32x4 B) { return mmF32x4(_mm_xor_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Not(f32x4 A)          { return mmF32x4(_mm_xor_ps(A.mm, _mm_castsi128_ps(_mm_set1_epi32(0x80000000)))); }

// HELPER FUNCTIONS
inline f32x4 F32x4_Round(f32x4 A)        { return mmF32x4(_mm_round_ps(A.mm, _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline f32x4 F32x4_Truncate(f32x4 A)     { return mmF32x4(_mm_round_ps(A.mm, _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline f32x4 F32x4_Floor(f32x4 A)        { return mmF32x4(_mm_floor_ps(A.mm)); }
inline f32x4 F32x4_Ceil(f32x4 A)         { return mmF32x4(_mm_ceil_ps(A.mm)); }
inline f32x4 F32x4_Fract(f32x4 X)        { return F32x4_Sub(X, F32x4_Floor(X)); }
inline f32x4 F32x4_Mod(f32x4 X, f32x4 Y) { return F32x4_Sub(X, F32x4_Mul(Y, F32x4_Floor(F32x4_Div(X, Y)))); }
inline f32x4 F32x4_Saturate(f32x4 A)     { return mmF32x4(_mm_max_ps(_mm_min_ps(A.mm, _mm_set1_ps(1.0f)), _mm_set1_ps(0.0f))); }
inline f32x4 F32x4_Sign(f32x4 A)         { return mmF32x4(_mm_or_ps(_mm_and_ps(A.mm, _mm_castsi128_ps(_mm_set1_epi32(0x80000000))), _mm_set1_ps(1.0f))); }
inline f32x4 F32x4_Positive(f32x4 A)     { return mmF32x4(_mm_and_ps(A.mm, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); }
inline f32x4 F32x4_Negative(f32x4 A)     { return mmF32x4(_mm_or_ps(A.mm, _mm_castsi128_ps(_mm_set1_epi32(0x80000000)))); }
inline f32x4 F32x4_Abs(f32x4 A)          { return mmF32x4(_mm_and_ps(A.mm, _mm_castsi128_ps(_mm_set1_epi32(0x7FFFFFFF)))); }
inline f32x4 F32x4_Sqrt(f32x4 A)         { return mmF32x4(_mm_sqrt_ps(A.mm)); }
inline f32x4 F32x4_Min(f32x4 A, f32x4 B) { return mmF32x4(_mm_min_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Max(f32x4 A, f32x4 B) { return mmF32x4(_mm_max_ps(A.mm, B.mm)); }
inline f32x4 F32x4_Blend(f32x4 A, f32x4 B, f32x4 Mask)  { return mmF32x4(_mm_blendv_ps(A.mm, B.mm, Mask.mm)); }
inline f32x4 F32x4_Clamp(f32x4 A, f32x4 Min, f32x4 Max) { return mmF32x4(_mm_max_ps(_mm_min_ps(A.mm, Max.mm), Min.mm)); }
inline f32x4
F32x4_Lerp(f32x4 F0, f32x4 F1, f32x4 T)
{
    __m128 mT1 = T.mm;
    __m128 mT0 = _mm_sub_ps(_mm_set1_ps(1.0f), mT1);
    mT0 = _mm_mul_ps(mT0, F0.mm);
    mT1 = _mm_mul_ps(mT1, F1.mm);
    return mmF32x4(_mm_add_ps(mT0, mT1));
}


#ifdef __cplusplus

// OPERATORS
inline f32x4 operator+ (f32x4 A, f32x4 B) { return F32x4_Add(A, B); }
inline f32x4 operator- (f32x4 A, f32x4 B) { return F32x4_Sub(A, B); }
inline f32x4 operator* (f32x4 A, f32x4 B) { return F32x4_Mul(A, B); }
inline f32x4 operator/ (f32x4 A, f32x4 B) { return F32x4_Div(A, B); }
inline f32x4 operator- (f32x4 A)          { return F32x4_Negate(A); }

inline void operator+= (f32x4 &A, f32x4 B) { A = F32x4_Add(A, B); }
inline void operator-= (f32x4 &A, f32x4 B) { A = F32x4_Sub(A, B); }
inline void operator*= (f32x4 &A, f32x4 B) { A = F32x4_Mul(A, B); }
inline void operator/= (f32x4 &A, f32x4 B) { A = F32x4_Div(A, B); }

inline f32x4 operator== (f32x4 A, f32x4 B) { return F32x4_Equal(A, B); }
inline f32x4 operator!= (f32x4 A, f32x4 B) { return F32x4_NotEqual(A, B); }
inline f32x4 operator>= (f32x4 A, f32x4 B) { return F32x4_GreaterEqual(A, B); }
inline f32x4 operator>  (f32x4 A, f32x4 B) { return F32x4_GreaterThan(A, B); }
inline f32x4 operator<= (f32x4 A, f32x4 B) { return F32x4_LessEqual(A, B); }
inline f32x4 operator<  (f32x4 A, f32x4 B) { return F32x4_LessThan(A, B); }

inline f32x4 operator&& (f32x4 A, f32x4 B) { return F32x4_And(A, B); }
inline f32x4 operator|| (f32x4 A, f32x4 B) { return F32x4_Or(A, B); }
inline f32x4 operator^ (f32x4 A, f32x4 B) { return F32x4_Xor(A, B); }
inline f32x4 operator! (f32x4 A) { return F32x4_Not(A); }
inline f32x4 operator~ (f32x4 A) { return F32x4_Not(A); }

// OVERLOADS
inline f32x4 Round(f32x4 A)                       { return F32x4_Round(A); }
inline f32x4 Truncate(f32x4 A)                    { return F32x4_Truncate(A); }
inline f32x4 Floor(f32x4 A)                       { return F32x4_Floor(A); }
inline f32x4 Ceil(f32x4 A)                        { return F32x4_Ceil(A); }
inline f32x4 Fract(f32x4 X)                       { return F32x4_Fract(X); }
inline f32x4 Mod(f32x4 X, f32x4 Y)                { return F32x4_Mod(X, Y); }
inline f32x4 Saturate(f32x4 A)                    { return F32x4_Saturate(A); }
inline f32x4 Sign(f32x4 A)                        { return F32x4_Sign(A); }
inline f32x4 Positive(f32x4 A)                    { return F32x4_Positive(A); }
inline f32x4 Negative(f32x4 A)                    { return F32x4_Negative(A); }
inline f32x4 Abs(f32x4 A)                         { return F32x4_Abs(A); }
inline f32x4 Sqrt(f32x4 A)                        { return F32x4_Sqrt(A); }
inline f32x4 Min(f32x4 A, f32x4 B)                { return F32x4_Min(A, B); }
inline f32x4 Max(f32x4 A, f32x4 B)                { return F32x4_Max(A, B); }
inline f32x4 Blend(f32x4 A, f32x4 B, f32x4 Mask)  { return F32x4_Blend(A, B, Mask); }
inline f32x4 Lerp(f32x4 F0, f32x4 F1, f32x4 T)    { return F32x4_Lerp(F0, F1, T); }
inline f32x4 Clamp(f32x4 A, f32x4 Min, f32x4 Max) { return F32x4_Clamp(A, Min, Max); }

inline b32 Mask(f32x4 Mask) { return F32x4_Mask(Mask); }
inline b32 Any(f32x4 Mask)  { return F32x4_Any(Mask); }
inline b32 All(f32x4 Mask)  { return F32x4_All(Mask); }

#endif
