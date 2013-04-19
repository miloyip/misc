#include <cassert>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <ctime>
#include <emmintrin.h>

// Naive
bool IsPointInCircularSector(
    float cx, float cy, float ux, float uy, float r, float theta,
    float px, float py)
{
    assert(theta > 0 && theta < M_PI);
    assert(r > 0.0f);

    // D = P - C
    float dx = px - cx;
    float dy = py - cy;

    // |D| = (dx^2 + dy^2)^0.5
    float length = sqrt(dx * dx + dy * dy);

    // |D| > r
    if (length > r)
        return false;

    // Normalize D
    dx /= length;
    dy /= length;

    // acos(D dot U) < theta
    return acos(dx * ux + dy * uy) < theta;
}

// Basic: use squareR and cosTheta as parameters, defer sqrt(), eliminate division
bool IsPointInCircularSector1(
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    assert(cosTheta > -1 && cosTheta < 1);
    assert(squaredR > 0.0f);

    // D = P - C
    float dx = px - cx;
    float dy = py - cy;

    // |D|^2 = (dx^2 + dy^2)
    float squaredLength = dx * dx + dy * dy;

    // |D|^2 > r^2
    if (squaredLength > squaredR)
        return false;

    // |D|
    float length = sqrt(squaredLength);

    // D dot U > |D| cos(theta)
    return dx * ux + dy * uy > length * cosTheta;
}

// Eliminate sqrt()
bool IsPointInCircularSector2(
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    assert(cosTheta > -1 && cosTheta < 1);
    assert(squaredR > 0.0f);

    // D = P - C
    float dx = px - cx;
    float dy = py - cy;

    // |D|^2 = (dx^2 + dy^2)
    float squaredLength = dx * dx + dy * dy;

    // |D|^2 > r^2
    if (squaredLength > squaredR)
        return false;

    // D dot U
    float DdotU = dx * ux + dy * uy;

    // D dot U > |D| cos(theta)
    // <=>
    // (D dot U)^2 > |D|^2 (cos(theta))^2 if D dot U >= 0 and cos(theta) >= 0
    // (D dot U)^2 < |D|^2 (cos(theta))^2 if D dot U <  0 and cos(theta) <  0
    // true                               if D dot U >= 0 and cos(theta) <  0
    // false                              if D dot U <  0 and cos(theta) >= 0
    if (DdotU >= 0 && cosTheta >= 0)
        return DdotU * DdotU > squaredLength * cosTheta * cosTheta;
    else if (DdotU < 0 && cosTheta < 0)
        return DdotU * DdotU < squaredLength * cosTheta * cosTheta;
    else
        return DdotU >= 0;
}

// Bit trick
bool IsPointInCircularSector3(
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    assert(cosTheta > -1 && cosTheta < 1);
    assert(squaredR > 0.0f);

    // D = P - C
    float dx = px - cx;
    float dy = py - cy;

    // |D|^2 = (dx^2 + dy^2)
    float squaredLength = dx * dx + dy * dy;

    // |D|^2 > r^2
    if (squaredLength > squaredR)
        return false;

    // D dot U
    float DdotU = dx * ux + dy * uy;

    // D dot U > |D| cos(theta)
    // <=>
    // (D dot U)^2 > |D|^2 (cos(theta))^2 if D dot U >= 0 and cos(theta) >= 0
    // (D dot U)^2 < |D|^2 (cos(theta))^2 if D dot U <  0 and cos(theta) <  0
    // true                               if D dot U >= 0 and cos(theta) <  0
    // false                              if D dot U <  0 and cos(theta) >= 0
    const unsigned cSignMask = 0x80000000;
    union {
        float f;
        unsigned u;
    }a, b, lhs, rhs;
    a.f = DdotU;
    b.f = cosTheta;
    unsigned asign = a.u & cSignMask;
    unsigned bsign = b.u & cSignMask;
    if (asign == bsign) {
        lhs.f = DdotU * DdotU;
        rhs.f = squaredLength * cosTheta * cosTheta;
        lhs.u |= asign;
        rhs.u |= asign;
        return lhs.f > rhs.f;
    }
    else
        return asign == 0;
}

const union VectorI32 {
    unsigned u[4];
    __m128 v;
}cSignMask = { 0x80000000, 0x80000000, 0x80000000, 0x80000000 };

// SSE2, SOA(struct of array) layout
__m128 IsPointInCircularSector4(
	__m128 cx, __m128 cy, __m128 ux, const __m128& uy, const __m128& squaredR, const __m128& cosTheta,
	const __m128& px, const __m128& py)
{
	// D = P - C
	__m128 dx = _mm_sub_ps(px, cx);
	__m128 dy = _mm_sub_ps(py, cy);

	// |D|^2 = (dx^2 + dy^2)
	__m128 squaredLength = _mm_add_ps(_mm_mul_ps(dx, dx), _mm_mul_ps(dy, dy));

	// |D|^2 < r^2
	__m128 lengthResult = _mm_cmpgt_ps(squaredR, squaredLength);

	// |D|
	__m128 length = _mm_sqrt_ps(squaredLength);

	// D dot U
	__m128 DdotU = _mm_add_ps(_mm_mul_ps(dx, ux), _mm_mul_ps(dy, uy));

	// D dot U > |D| cos(theta)
	__m128 angularResult = _mm_cmpgt_ps(DdotU, _mm_mul_ps(length, cosTheta));

	__m128 result = _mm_and_ps(lengthResult, angularResult);

	return result;
}

// SSE2, SOA(struct of array) layout without sqrt()
__m128 IsPointInCircularSector5(
    __m128 cx, __m128 cy, __m128 ux, const __m128& uy, const __m128& squaredR, const __m128& cosTheta,
    const __m128& px, const __m128& py)
{
    // D = P - C
    __m128 dx = _mm_sub_ps(px, cx);
    __m128 dy = _mm_sub_ps(py, cy);

    // |D|^2 = (dx^2 + dy^2)
    __m128 squaredLength = _mm_add_ps(_mm_mul_ps(dx, dx), _mm_mul_ps(dy, dy));

    // |D|^2 < r^2
    __m128 lengthResult = _mm_cmpgt_ps(squaredR, squaredLength);

    // D dot U
    __m128 DdotU = _mm_add_ps(_mm_mul_ps(dx, ux), _mm_mul_ps(dy, uy));

    // D dot U > |D| cos(theta)
    // <=>
    // (D dot U)^2 > |D|^2 (cos(theta))^2 if D dot U >= 0 and cos(theta) >= 0
    // (D dot U)^2 < |D|^2 (cos(theta))^2 if D dot U <  0 and cos(theta) <  0
    // true                               if D dot U >= 0 and cos(theta) <  0
    // false                              if D dot U <  0 and cos(theta) >= 0
    __m128 asign = _mm_and_ps(DdotU, cSignMask.v);
    __m128 bsign = _mm_and_ps(cosTheta, cSignMask.v);
    __m128 equalsign = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(asign), _mm_castps_si128(bsign)));
    
    __m128 lhs = _mm_or_ps(_mm_mul_ps(DdotU, DdotU), asign);
    __m128 rhs = _mm_or_ps(_mm_mul_ps(squaredLength, _mm_mul_ps(cosTheta, cosTheta)), asign);
    __m128 equalSignResult = _mm_cmpgt_ps(lhs, rhs);

    __m128 unequalSignResult = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_castps_si128(asign), _mm_setzero_si128()));

    __m128 result = _mm_and_ps(lengthResult, _mm_or_ps(
        _mm_and_ps(equalsign, equalSignResult),
        _mm_andnot_ps(equalsign, unequalSignResult)));

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Tests

bool NoOperation(
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    return false;
}

__m128 NoOperationSIMD(
    __m128 cx, __m128 cy, __m128 ux, const __m128& uy, const __m128& squaredR, const __m128& cosTheta,
    const __m128& px, const __m128& py)
{
    return _mm_setzero_ps();
}

bool AreSame(
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    bool result1 = IsPointInCircularSector1(
        cx, cy, ux, uy, squaredR, cosTheta, px, py);

    bool result2 = IsPointInCircularSector2(
        cx, cy, ux, uy, squaredR, cosTheta, px, py);

    bool result3 = IsPointInCircularSector3(
        cx, cy, ux, uy, squaredR, cosTheta, px, py);

    assert(result1 == result2);
    assert(result1 == result3);
    return result1;
}

void IsExpected(
    bool expected,
    float cx, float cy, float ux, float uy, float squaredR, float cosTheta,
    float px, float py)
{
    assert(expected == AreSame(cx, cy, ux, uy, squaredR, cosTheta, px, py));
}

// Test data

float Uniform(float minimum, float maximum) {
    return rand() * (maximum - minimum) / RAND_MAX + minimum;
}

const unsigned N = 1000;
#if _DEBUG
const unsigned M = 1000;
#else
const unsigned M = 100000;
#endif

__declspec(align(128)) float gCx[N];
__declspec(align(128)) float gCy[N];
__declspec(align(128)) float gUx[N];
__declspec(align(128)) float gUy[N];
__declspec(align(128)) float gSquaredR[N];
__declspec(align(128)) float gCosTheta[N];
__declspec(align(128)) float gPx[M];
__declspec(align(128)) float gPy[M];
__declspec(align(128)) float gR[N];		// naive
__declspec(align(128)) float gTheta[N];	// naive

// Run test

template <bool naiveParam, typename TestFunc>
float Test(TestFunc f, float rmax = 2.0f) {
    unsigned count = 0;
    for (int i = 0; i < N; i++) {
        float cx = gCx[i];
        float cy = gCy[i];
        float ux = gUx[i];
        float uy = gUy[i];
        float r = naiveParam ? gR[i] : gSquaredR[i];
        float t = naiveParam ? gTheta[i] : gCosTheta[i];

        for (int j = 0; j < M; j++) {
            if (f(cx, cy, ux, uy, r, t, gPx[j], gPy[j]))
                count++;
        }
    }
    return (float)count / (N * M);
}

template <typename TestFunc>
float TestSIMD(TestFunc f, float rmax = 2.0f) {
    static const unsigned cCountTable[] = { 
        0, // 0000
        1, // 0001
        1, // 0010
        2, // 0011
        1, // 0100
        2, // 0101
        2, // 0110
        3, // 0111
        1, // 1000
        2, // 1001
        2, // 1010
        3, // 1011
        2, // 1100
        3, // 1101
        3, // 1110
        4, // 1111
    };

    unsigned count = 0;
    for (int i = 0; i < N; i++) {
        __m128 cx = _mm_set1_ps(gCx[i]);
        __m128 cy = _mm_set1_ps(gCy[i]);
        __m128 ux = _mm_set1_ps(gUx[i]);
        __m128 uy = _mm_set1_ps(gUy[i]);
        __m128 squaredR = _mm_set1_ps(gSquaredR[i]);
        __m128 cosTheta = _mm_set1_ps(gCosTheta[i]);

        for (int j = 0; j < M; j += 4) {
            __m128 px = _mm_load_ps(&gPx[j]);
            __m128 py = _mm_load_ps(&gPy[j]);
            int mask = _mm_movemask_ps(f(cx, cy, ux, uy, squaredR, cosTheta, px, py));
            count += cCountTable[mask];
        }
    }
    return (float)count / (N * M);
}

template <bool naiveParam, typename TestFunc>
float Performance(const char* name, TestFunc f, float overhead = 0.0f) {
    clock_t start = clock();
    float hit = Test<naiveParam>(f);
    clock_t end = clock();
    float time = (float)(end - start) / CLOCKS_PER_SEC - overhead;
    printf("%s hit=%g%% time=%gs\n", name, hit * 100.0f, time);
    return time;
}

template <typename TestFunc>
float PerformanceSIMD(const char* name, TestFunc f, float overhead = 0.0f) {
    clock_t start = clock();
    float hit = TestSIMD(f);
    clock_t end = clock();
    float time = (float)(end - start) / CLOCKS_PER_SEC - overhead;
    printf("%s hit=%g%% time=%gs\n", name, hit * 100.0f, time);
    return time;
}

int main(int argc, char* argv[]) {
    srand(0);
    for (int i = 0; i < N; i++) {
        gCx[i] = Uniform(-1.0f, 1.0f);
        gCy[i] = Uniform(-1.0f, 1.0f);

        float ux = Uniform(-1.0f, 1.0f);
        float uy = Uniform(-1.0f, 1.0f);
        float ru = 1.0f / (sqrt(ux * ux + uy * uy));
        gUx[i] = ux * ru;
        gUy[i] = uy * ru;

        gR[i] = Uniform(0.0f, 2.0f);
        gSquaredR[i] = gR[i] * gR[i];
		gTheta[i] = Uniform(0.0f, float(M_PI));
		gCosTheta[i] = cos(gTheta[i]);
    }

    for (int j = 0; j < N; j++) {
        gPx[j] = Uniform(-1.0f, 1.0f);
        gPy[j] = Uniform(-1.0f, 1.0f);
    }

#ifdef _DEBUG
    IsExpected(true, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.5f, 1.0f, 0.0f);
    IsExpected(false, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.5f, 4.0f, 1.0f);
    IsExpected(true, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.0f);
    IsExpected(false, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -0.5f, 0.0f);

    Test<false>(AreSame);
#else
#define PERF(f, naiveParam, overhead) Performance<naiveParam>(#f, f, overhead)
#define PERFSIMD(f, overhead) PerformanceSIMD(#f, f, overhead)
	float overhead = PERF(NoOperation, true, 0.0f);
	PERF(IsPointInCircularSector, true, overhead);
	printf("\n");

	overhead = PERF(NoOperation, false, 0.0f);
    PERF(IsPointInCircularSector1, false, overhead);
    PERF(IsPointInCircularSector2, false, overhead);
    PERF(IsPointInCircularSector3, false, overhead);
	printf("\n");

    overhead = PERFSIMD(NoOperationSIMD, 0.0f);
    PERFSIMD(IsPointInCircularSector4, overhead);
	PERFSIMD(IsPointInCircularSector5, overhead);
#undef PERF
#undef PERFSIMD
#endif
    return 0;
}
