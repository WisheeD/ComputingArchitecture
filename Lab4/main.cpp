#include <iostream>
#include <emmintrin.h>
#include <cstdint>

void vector_operation(int8_t* A, int8_t* B, int8_t* C, int16_t* D, int16_t* F, int size) {
    for (int i = 0; i < size; i += 8) {
        __m128i vector_a = _mm_set_epi16(A[7], A[6], A[5], A[4], A[3], A[2], A[1], A[0]);
        __m128i vector_b = _mm_set_epi16(B[7], B[6], B[5], B[4], B[3], B[2], B[1], B[0]);
        __m128i vector_c = _mm_set_epi16(C[7], C[6], C[5], C[4], C[3], C[2], C[1], C[0]);
        __m128i vector_d = _mm_loadu_si128((__m128i*)D);

        __m128i vector_result = _mm_sub_epi16(_mm_sub_epi16(vector_a, _mm_add_epi16(vector_b, vector_c)), vector_d);

        _mm_storeu_si128(reinterpret_cast<__m128i*>(&F[i]), vector_result);
    }
}

int main() {
    int8_t A[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int8_t B[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int8_t C[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int16_t D[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int16_t F[8];

    vector_operation(A, B, C, D, F, 8);

    std::cout << "A[i] - (B[i] + C[i]) - D[i]: ";
    for (short i : F) {
        std::cout << i << " ";
    }
}
