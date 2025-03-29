#pragma once

#include "Define.h"

#include <DirectXMath.h>
#include <xmmintrin.h>
#include <smmintrin.h> // for _mm_dp_ps

// 4x4 행렬 연산
struct alignas(16) FMatrix
{
    float M[4][4];
    static const FMatrix Identity;
    // 기본 연산자 오버로딩
    FMatrix operator+(const FMatrix& Other) const;
    FMatrix operator-(const FMatrix& Other) const;
    FMatrix operator*(const FMatrix& Other) const;
    FMatrix operator*(float Scalar) const;
    FMatrix operator/(float Scalar) const;
    float* operator[](int row);
    const float* operator[](int row) const;

    // 유틸리티 함수
    static FMatrix Transpose(const FMatrix& Mat);
    static float Determinant(const FMatrix& Mat);
    static FMatrix Inverse(const FMatrix& Mat);
    static FMatrix CreateRotation(float roll, float pitch, float yaw);
    static FMatrix CreateScale(float scaleX, float scaleY, float scaleZ);
    static FVector TransformVector(const FVector& v, const FMatrix& m);
    static FVector4 TransformVector(const FVector4& v, const FMatrix& m);
    static FMatrix CreateTranslationMatrix(const FVector& position);

    // refactor
    FVector operator*(const FVector& Other) const;

	DirectX::XMMATRIX ToXMMATRIX() const
	{
		return DirectX::XMMatrixSet(
			M[0][0], M[1][0], M[2][0], M[3][0], // 첫 번째 열
			M[0][1], M[1][1], M[2][1], M[3][1], // 두 번째 열
			M[0][2], M[1][2], M[2][2], M[3][2], // 세 번째 열
			M[0][3], M[1][3], M[2][3], M[3][3]  // 네 번째 열
		);
	}

	FVector4 TransformFVector4(const FVector4& vector)
	{
		return FVector4(
			M[0][0] * vector.x + M[1][0] * vector.y + M[2][0] * vector.z + M[3][0] * vector.a,
			M[0][1] * vector.x + M[1][1] * vector.y + M[2][1] * vector.z + M[3][1] * vector.a,
			M[0][2] * vector.x + M[1][2] * vector.y + M[2][2] * vector.z + M[3][2] * vector.a,
			M[0][3] * vector.x + M[1][3] * vector.y + M[2][3] * vector.z + M[3][3] * vector.a
		);
	}
	FVector TransformPosition(const FVector& vector) const
	{
		float x = M[0][0] * vector.x + M[1][0] * vector.y + M[2][0] * vector.z + M[3][0];
		float y = M[0][1] * vector.x + M[1][1] * vector.y + M[2][1] * vector.z + M[3][1];
		float z = M[0][2] * vector.x + M[1][2] * vector.y + M[2][2] * vector.z + M[3][2];
		float w = M[0][3] * vector.x + M[1][3] * vector.y + M[2][3] * vector.z + M[3][3];
		return w != 0.0f ? FVector{ x / w, y / w, z / w } : FVector{ x, y, z };
	}
};

// eg.
// FMatrixSIMD simd= this->ToSIMD;
// return simd.TransformFVector4(vector);
//struct FMatrixSIMD
//{
//    // 근데 FMatrix 기본 생성자도 없긴 한데 상관없지 않나 왜 안돼지
//    FMatrixSIMD() = default;
//
//    __m128 rows[4]; // 행 기준 저장 (각 __m128 = 4개의 float)
//
//    FMatrixSIMD(const FMatrix& mat)
//    {
//        for (int i = 0; i < 4; ++i)
//        {
//            rows[i] = _mm_set_ps(mat.M[i][3], mat.M[i][2], mat.M[i][1], mat.M[i][0]);
//        }
//    }
//
//    // 행렬-벡터 곱
//    FVector MultiplyVector(const FVector& Other) const
//    {
//        __m128 vec = _mm_set_ps(0.0f, Other.z, Other.y, Other.x); // w = 0
//        float result[4];
//
//        for (int i = 0; i < 3; ++i)
//        {
//            __m128 mul = _mm_mul_ps(rows[i], vec);          // 요소별 곱
//            __m128 temp = _mm_hadd_ps(mul, mul);            // 수평 덧셈 1
//            temp = _mm_hadd_ps(temp, temp);                 // 수평 덧셈 2
//            _mm_store_ss(&result[i], temp);
//        }
//
//        return FVector{ result[0], result[1], result[2] };
//    }
//
//    // 벡터4 곱 (w 포함)
//    FVector4 MultiplyVector4(const FVector4& Other) const
//    {
//        __m128 vec = _mm_set_ps(Other.a, Other.z, Other.y, Other.x);
//        float result[4];
//
//        for (int i = 0; i < 4; ++i)
//        {
//            __m128 mul = _mm_mul_ps(rows[i], vec);
//            __m128 temp = _mm_hadd_ps(mul, mul);
//            temp = _mm_hadd_ps(temp, temp);
//            _mm_store_ss(&result[i], temp);
//        }
//
//        return FVector4{ result[0], result[1], result[2], result[3] };
//    }
//
//    // 기존 함수1
//    FVector4 TransformFVector4SIMD(const FVector4& Other) const
//    {
//        __m128 vec = _mm_set_ps(Other.a, Other.z, Other.y, Other.x);
//        float result[4];
//
//        for (int i = 0; i < 4; ++i)
//        {
//            __m128 mul = _mm_mul_ps(rows[i], vec);      // 요소별 곱
//            __m128 sum = _mm_hadd_ps(mul, mul);         // 수평 더하기 1
//            sum = _mm_hadd_ps(sum, sum);                // 수평 더하기 2
//            _mm_store_ss(&result[i], sum);                 // 첫 값만 저장
//        }
//
//        return FVector4{ result[0], result[1], result[2], result[3] };
//    }
//
//    // 기존 함수2
//    FVector TransformPositionSIMD(const FVector& Other) const
//    {
//        __m128 vec = _mm_set_ps(1.0f, Other.z, Other.y, Other.x); // w = 1 (position용)
//        float result[4];
//
//        for (int i = 0; i < 4; ++i)
//        {
//            __m128 mul = _mm_mul_ps(rows[i], vec);
//            __m128 sum = _mm_hadd_ps(mul, mul);
//            sum = _mm_hadd_ps(sum, sum);
//            _mm_store_ss(&result[i], sum);
//        }
//
//        float w = result[3];
//        if (fabs(w) > 1e-6f)
//            return FVector{ result[0] / w, result[1] / w, result[2] / w };
//        else
//            return FVector{ result[0], result[1], result[2] };
//    }
//
//    FMatrixSIMD& operator=(const FMatrix& mat)
//    {
//        for (int i = 0; i < 4; ++i)
//        {
//            rows[i] = _mm_set_ps(mat.M[i][3], mat.M[i][2], mat.M[i][1], mat.M[i][0]);
//        }
//        return *this;
//    }
//
//    FMatrixSIMD operator*(FMatrixSIMD& Other) const;
//    FMatrix operator*(const FMatrix& Other) const;
//};

struct alignas(16) FMatrixSIMD
{
    __m128 rows[4];

    // 기본 생성자: 모든 행을 0으로 초기화
    FMatrixSIMD() {
        rows[0] = _mm_setzero_ps();
        rows[1] = _mm_setzero_ps();
        rows[2] = _mm_setzero_ps();
        rows[3] = _mm_setzero_ps();
    }

    // FMatrix로부터 생성 (행-주 방식)
    FMatrixSIMD(const FMatrix& mat) {
        for (int i = 0; i < 4; ++i)
        {
            // FMatrix가 16바이트 정렬되어 있다고 가정
            rows[i] = _mm_set_ps(mat.M[i][3], mat.M[i][2], mat.M[i][1], mat.M[i][0]);
        }
    }

    // 3D 벡터 변환 (w=0)
    FVector MultiplyVector(const FVector& Other) const {
        __m128 vec = _mm_set_ps(0.0f, Other.z, Other.y, Other.x);
        float ResultX = _mm_cvtss_f32(_mm_dp_ps(rows[0], vec, 0xF1));
        float ResultY = _mm_cvtss_f32(_mm_dp_ps(rows[1], vec, 0xF1));
        float ResultZ = _mm_cvtss_f32(_mm_dp_ps(rows[2], vec, 0xF1));
        return FVector(ResultX, ResultY, ResultZ);
    }

    // 4D 벡터 변환 (w 포함)
    FVector4 MultiplyVector4(const FVector4& Other) const {
        __m128 vec = _mm_set_ps(Other.a, Other.z, Other.y, Other.x);
        float ResultX = _mm_cvtss_f32(_mm_dp_ps(rows[0], vec, 0xF1));
        float ResultY = _mm_cvtss_f32(_mm_dp_ps(rows[1], vec, 0xF1));
        float ResultZ = _mm_cvtss_f32(_mm_dp_ps(rows[2], vec, 0xF1));
        float ResultA = _mm_cvtss_f32(_mm_dp_ps(rows[3], vec, 0xF1));
        return FVector4(ResultX, ResultY, ResultZ, ResultA);
    }

    // 위치 변환 (w=1로 가정)
    FVector TransformPositionSIMD(const FVector& Other) const {
        __m128 vec = _mm_set_ps(1.0f, Other.z, Other.y, Other.x);
        float result[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        for (int i = 0; i < 4; ++i) {
            __m128 mul = _mm_mul_ps(rows[i], vec);
            __m128 sum = _mm_hadd_ps(mul, mul);
            sum = _mm_hadd_ps(sum, sum);
            _mm_store_ss(&result[i], sum);
        }
        float w = result[3];
        if (fabs(w) > 1e-6f)
            return FVector(result[0] / w, result[1] / w, result[2] / w);
        else
            return FVector(result[0], result[1], result[2]);
    }

    // FMatrix 대입
    FMatrixSIMD& operator=(const FMatrix& mat) {
        for (int i = 0; i < 4; ++i)
            rows[i] = _mm_set_ps(mat.M[i][3], mat.M[i][2], mat.M[i][1], mat.M[i][0]);
        return *this;
    }

    // FMatrixSIMD와 FMatrixSIMD의 덧셈 (element-wise)
    FMatrixSIMD operator+(const FMatrixSIMD& Other) const {
        FMatrixSIMD result;
        for (int i = 0; i < 4; ++i)
            result.rows[i] = _mm_add_ps(this->rows[i], Other.rows[i]);
        return result;
    }

    // FMatrixSIMD와 FVector의 덧셈
    // 여기서는 FVector의 x, y, z를 각각 행 0, 1, 2에 더하고, 행 3은 그대로 둡니다.
    FMatrixSIMD operator+(const FVector& Other) const {
        FMatrixSIMD result;
        result.rows[0] = _mm_add_ps(this->rows[0], _mm_set1_ps(Other.x));
        result.rows[1] = _mm_add_ps(this->rows[1], _mm_set1_ps(Other.y));
        result.rows[2] = _mm_add_ps(this->rows[2], _mm_set1_ps(Other.z));
        result.rows[3] = this->rows[3]; // 변경 없음
        return result;

    }

    // FMatrixSIMD와 FMatrixSIMD의 뺄셈 (element-wise)
    FMatrixSIMD operator-(const FMatrixSIMD& Other) const {
        FMatrixSIMD result;
        for (int i = 0; i < 4; ++i)
            result.rows[i] = _mm_sub_ps(this->rows[i], Other.rows[i]);
        return result;
    }

    // FMatrixSIMD와 FMatrixSIMD의 곱셈
    FMatrixSIMD operator*(const FMatrixSIMD& Other) const {
        FMatrixSIMD OtherCopy = Other;
        _MM_TRANSPOSE4_PS(OtherCopy.rows[0], OtherCopy.rows[1], OtherCopy.rows[2], OtherCopy.rows[3]);
        FMatrixSIMD result;
        for (int i = 0; i < 4; ++i) {
            float r0 = _mm_cvtss_f32(_mm_dp_ps(rows[i], OtherCopy.rows[0], 0xF1));
            float r1 = _mm_cvtss_f32(_mm_dp_ps(rows[i], OtherCopy.rows[1], 0xF1));
            float r2 = _mm_cvtss_f32(_mm_dp_ps(rows[i], OtherCopy.rows[2], 0xF1));
            float r3 = _mm_cvtss_f32(_mm_dp_ps(rows[i], OtherCopy.rows[3], 0xF1));
            result.rows[i] = _mm_set_ps(r3, r2, r1, r0);
        }
        return result;
    }

    // FMatrixSIMD와 FMatrix의 곱셈 (결과를 일반 FMatrix로 반환)
    FMatrix operator*(const FMatrix& Other) const {
        FMatrix result;
        __m128 cols[4];
        for (int j = 0; j < 4; ++j)
            cols[j] = _mm_set_ps(Other.M[3][j], Other.M[2][j], Other.M[1][j], Other.M[0][j]);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                __m128 dp = _mm_dp_ps(rows[i], cols[j], 0xF1);
                _mm_store_ss(&result.M[i][j], dp);
            }
        }
        return result;
    }

    template<int RowIndex, int IndexA, int IndexB>
    FORCEINLINE float SumRowValues() const {
        __m128 row = rows[RowIndex];
        __m128 a = _mm_shuffle_ps(row, row, _MM_SHUFFLE(IndexA, IndexA, IndexA, IndexA));
        __m128 b = _mm_shuffle_ps(row, row, _MM_SHUFFLE(IndexB, IndexB, IndexB, IndexB));
        __m128 sum = _mm_add_ps(a, b);
        return _mm_cvtss_f32(sum);
    }

    template<int RowIndex, int IndexA, int IndexB>
    FORCEINLINE float SubtractRowValues() const {
        __m128 row = rows[RowIndex];
        __m128 a = _mm_shuffle_ps(row, row, _MM_SHUFFLE(IndexA, IndexA, IndexA, IndexA));
        __m128 b = _mm_shuffle_ps(row, row, _MM_SHUFFLE(IndexB, IndexB, IndexB, IndexB));
        __m128 diff = _mm_sub_ps(a, b);
        return _mm_cvtss_f32(diff);
    }

    // TransformVector: 인스턴스 멤버 함수로, 3D 벡터 변환 (w = 0)
    FVector TransformVector(const FVector& Other) const {
        __m128 vec = _mm_set_ps(0.0f, Other.z, Other.y, Other.x);
        float ResultX = _mm_cvtss_f32(_mm_dp_ps(rows[0], vec, 0xF1));
        float ResultY = _mm_cvtss_f32(_mm_dp_ps(rows[1], vec, 0xF1));
        float ResultZ = _mm_cvtss_f32(_mm_dp_ps(rows[2], vec, 0xF1));
        return FVector(ResultX, ResultY, ResultZ);
    }

    // TransformVector: 4D 벡터 변환 (w 포함)
    FVector4 TransformVector(const FVector4& Other) const {
        __m128 vec = _mm_set_ps(Other.a, Other.z, Other.y, Other.x);
        float ResultX = _mm_cvtss_f32(_mm_dp_ps(rows[0], vec, 0xF1));
        float ResultY = _mm_cvtss_f32(_mm_dp_ps(rows[1], vec, 0xF1));
        float ResultZ = _mm_cvtss_f32(_mm_dp_ps(rows[2], vec, 0xF1));
        float ResultA = _mm_cvtss_f32(_mm_dp_ps(rows[3], vec, 0xF1));
        return FVector4(ResultX, ResultY, ResultZ, ResultA);
    }

    // Transpose: 행렬 전치 (자기 자신을 복사하여 전치한 행렬 반환)
    FMatrixSIMD Transpose() const {
        FMatrixSIMD result = *this;
        _MM_TRANSPOSE4_PS(result.rows[0], result.rows[1], result.rows[2], result.rows[3]);
        return result;
    }

    // CreateTranslationMatrix: Identity에서 시작해, row3의 첫 3요소를 translation으로 설정
    FMatrixSIMD CreateTranslationMatrixSIMD(const FVector& position) const {
        // 우선 Identity 행렬로 시작 (FMatrix::Identity를 이용)
        FMatrix id = FMatrix::Identity;
        FMatrixSIMD result(id);
        // row3에 translation 적용: 기존 row3의 0,1,2 요소를 position으로 대체
        float row3[4];
        _mm_store_ps(row3, result.rows[3]);
        row3[0] = position.x;
        row3[1] = position.y;
        row3[2] = position.z;
        // row3[3]는 보통 1로 유지
        result.rows[3] = _mm_load_ps(row3);
        return result;
    }

    FMatrix ToFMatrix() const
    {
        FMatrix result;
        for (int i = 0; i < 4; ++i)
        {
            _mm_store_ps(result.M[i], rows[i]);
        }
        return result;
    }
};
