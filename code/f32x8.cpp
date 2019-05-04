
struct f32x8
{
    __m256 mm;

#ifdef __cplusplus

    f32x8(__m256 MM) : mm(MM) { };
    f32x8(f32 F)  : mm(_mm256_set1_ps(F)) { };

#endif
};

// CONSTRUCTORS
inline f32x8 mmF32x8(__m256 mm) { f32x8 Result = { mm }; return Result; }
inline f32x8 F32x8(f32 a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g, f32 h) { return mmF32x8(_mm256_set_ps(h,g,f,e,d,c,b,a)); }
inline f32x8 F32x8_Zero(void)   { return mmF32x8(_mm256_setzero_ps()); }
inline f32x8 F32x8_Set1(f32 F)  { return mmF32x8(_mm256_set1_ps(F)); };
inline void F32x8_Store8(f32x8 F, f32 *E) { _mm256_storeu_ps(E, F.mm); }

// ARITHMETIC
inline f32x8 F32x8_Add(f32x8 A, f32x8 B) { return mmF32x8(_mm256_add_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Sub(f32x8 A, f32x8 B) { return mmF32x8(_mm256_sub_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Mul(f32x8 A, f32x8 B) { return mmF32x8(_mm256_mul_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Div(f32x8 A, f32x8 B) { return mmF32x8(_mm256_div_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Negate(f32x8 A)       { return mmF32x8(_mm256_xor_ps(A.mm, _mm256_set1_ps(-0.0f))); }

// COMPARISON
inline f32x8 F32x8_Equal       (f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_EQ_OQ)); }
inline f32x8 F32x8_NotEqual    (f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_NEQ_OQ)); }
inline f32x8 F32x8_GreaterEqual(f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_GE_OQ)); }
inline f32x8 F32x8_GreaterThan (f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_GT_OQ)); }
inline f32x8 F32x8_LessEqual   (f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_LE_OQ)); }
inline f32x8 F32x8_LessThan    (f32x8 A, f32x8 B) { return mmF32x8(_mm256_cmp_ps(A.mm, B.mm, _CMP_LT_OQ)); }
inline b32 F32x8_Mask(f32x8 Mask) { return _mm256_movemask_ps(Mask.mm); }
inline b32 F32x8_Any(f32x8 Mask)  { return _mm256_movemask_ps(Mask.mm) != 0; }
inline b32 F32x8_All(f32x8 Mask)  { return _mm256_movemask_ps(Mask.mm) == 255; }

// BIT MANIPULATION
inline f32x8 F32x8_And(f32x8 A, f32x8 B) { return mmF32x8(_mm256_and_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Or(f32x8 A, f32x8 B)  { return mmF32x8(_mm256_or_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Xor(f32x8 A, f32x8 B) { return mmF32x8(_mm256_xor_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Not(f32x8 A)          { return mmF32x8(_mm256_xor_ps(A.mm, _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000)))); }

// HELPER FUNCTIONS
inline f32x8 F32x8_Round(f32x8 A)        { return mmF32x8(_mm256_round_ps(A.mm, _MM_FROUND_TO_NEAREST_INT|_MM_FROUND_NO_EXC)); }
inline f32x8 F32x8_Truncate(f32x8 A)     { return mmF32x8(_mm256_round_ps(A.mm, _MM_FROUND_TO_ZERO|_MM_FROUND_NO_EXC)); }
inline f32x8 F32x8_Floor(f32x8 A)        { return mmF32x8(_mm256_floor_ps(A.mm)); }
inline f32x8 F32x8_Ceil(f32x8 A)         { return mmF32x8(_mm256_ceil_ps(A.mm)); }
inline f32x8 F32x8_Fract(f32x8 X)        { return F32x8_Sub(X, F32x8_Floor(X)); }
inline f32x8 F32x8_Mod(f32x8 X, f32x8 Y) { return F32x8_Sub(X, F32x8_Mul(Y, F32x8_Floor(F32x8_Div(X, Y)))); }
inline f32x8 F32x8_Saturate(f32x8 A)     { return mmF32x8(_mm256_max_ps(_mm256_min_ps(A.mm, _mm256_set1_ps(1.0f)), _mm256_set1_ps(0.0f))); }
inline f32x8 F32x8_Sign(f32x8 A)         { return mmF32x8(_mm256_or_ps(_mm256_and_ps(A.mm, _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000))), _mm256_set1_ps(1.0f))); }
inline f32x8 F32x8_Positive(f32x8 A)     { return mmF32x8(_mm256_and_ps(A.mm, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF)))); }
inline f32x8 F32x8_Negative(f32x8 A)     { return mmF32x8(_mm256_or_ps(A.mm, _mm256_castsi256_ps(_mm256_set1_epi32(0x80000000)))); }
inline f32x8 F32x8_Abs(f32x8 A)          { return mmF32x8(_mm256_and_ps(A.mm, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF)))); }
inline f32x8 F32x8_Sqrt(f32x8 A)         { return mmF32x8(_mm256_sqrt_ps(A.mm)); }
inline f32x8 F32x8_Min(f32x8 A, f32x8 B) { return mmF32x8(_mm256_min_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Max(f32x8 A, f32x8 B) { return mmF32x8(_mm256_max_ps(A.mm, B.mm)); }
inline f32x8 F32x8_Blend(f32x8 A, f32x8 B, f32x8 Mask)  { return mmF32x8(_mm256_blendv_ps(A.mm, B.mm, Mask.mm)); }
inline f32x8 F32x8_Clamp(f32x8 A, f32x8 Min, f32x8 Max) { return mmF32x8(_mm256_max_ps(_mm256_min_ps(A.mm, Max.mm), Min.mm)); }
inline f32x8
F32x8_Lerp(f32x8 F0, f32x8 F1, f32x8 T)
{
    __m256 mT1 = T.mm;
    __m256 mT0 = _mm256_sub_ps(_mm256_set1_ps(1.0f), mT1);
    mT0 = _mm256_mul_ps(mT0, F0.mm);
    mT1 = _mm256_mul_ps(mT1, F1.mm);
    return mmF32x8(_mm256_add_ps(mT0, mT1));
}


#ifdef __cplusplus

// OPERATORS
inline f32x8 operator+ (f32x8 A, f32x8 B) { return F32x8_Add(A, B); }
inline f32x8 operator- (f32x8 A, f32x8 B) { return F32x8_Sub(A, B); }
inline f32x8 operator* (f32x8 A, f32x8 B) { return F32x8_Mul(A, B); }
inline f32x8 operator/ (f32x8 A, f32x8 B) { return F32x8_Div(A, B); }
inline f32x8 operator- (f32x8 A)          { return F32x8_Negate(A); }

inline void operator+= (f32x8 &A, f32x8 B) { A = F32x8_Add(A, B); }
inline void operator-= (f32x8 &A, f32x8 B) { A = F32x8_Sub(A, B); }
inline void operator*= (f32x8 &A, f32x8 B) { A = F32x8_Mul(A, B); }
inline void operator/= (f32x8 &A, f32x8 B) { A = F32x8_Div(A, B); }

inline f32x8 operator== (f32x8 A, f32x8 B) { return F32x8_Equal(A, B); }
inline f32x8 operator!= (f32x8 A, f32x8 B) { return F32x8_NotEqual(A, B); }
inline f32x8 operator>= (f32x8 A, f32x8 B) { return F32x8_GreaterEqual(A, B); }
inline f32x8 operator>  (f32x8 A, f32x8 B) { return F32x8_GreaterThan(A, B); }
inline f32x8 operator<= (f32x8 A, f32x8 B) { return F32x8_LessEqual(A, B); }
inline f32x8 operator<  (f32x8 A, f32x8 B) { return F32x8_LessThan(A, B); }

inline f32x8 operator&& (f32x8 A, f32x8 B) { return F32x8_And(A, B); }
inline f32x8 operator|| (f32x8 A, f32x8 B) { return F32x8_Or(A, B); }
inline f32x8 operator^ (f32x8 A, f32x8 B) { return F32x8_Xor(A, B); }
inline f32x8 operator! (f32x8 A) { return F32x8_Not(A); }
inline f32x8 operator~ (f32x8 A) { return F32x8_Not(A); }

// OVERLOADS
inline f32x8 Round(f32x8 A)                       { return F32x8_Round(A); }
inline f32x8 Truncate(f32x8 A)                    { return F32x8_Truncate(A); }
inline f32x8 Floor(f32x8 A)                       { return F32x8_Floor(A); }
inline f32x8 Ceil(f32x8 A)                        { return F32x8_Ceil(A); }
inline f32x8 Fract(f32x8 X)                       { return F32x8_Fract(X); }
inline f32x8 Mod(f32x8 X, f32x8 Y)                { return F32x8_Mod(X, Y); }
inline f32x8 Saturate(f32x8 A)                    { return F32x8_Saturate(A); }
inline f32x8 Sign(f32x8 A)                        { return F32x8_Sign(A); }
inline f32x8 Positive(f32x8 A)                    { return F32x8_Positive(A); }
inline f32x8 Negative(f32x8 A)                    { return F32x8_Negative(A); }
inline f32x8 Abs(f32x8 A)                         { return F32x8_Abs(A); }
inline f32x8 Sqrt(f32x8 A)                        { return F32x8_Sqrt(A); }
inline f32x8 Min(f32x8 A, f32x8 B)                { return F32x8_Min(A, B); }
inline f32x8 Max(f32x8 A, f32x8 B)                { return F32x8_Max(A, B); }
inline f32x8 Blend(f32x8 A, f32x8 B, f32x8 Mask)  { return F32x8_Blend(A, B, Mask); }
inline f32x8 Lerp(f32x8 F0, f32x8 F1, f32x8 T)    { return F32x8_Lerp(F0, F1, T); }
inline f32x8 Clamp(f32x8 A, f32x8 Min, f32x8 Max) { return F32x8_Clamp(A, Min, Max); }

inline b32 Mask(f32x8 Mask) { return F32x8_Mask(Mask); }
inline b32 Any(f32x8 Mask)  { return F32x8_Any(Mask); }
inline b32 All(f32x8 Mask)  { return F32x8_All(Mask); }

#endif
