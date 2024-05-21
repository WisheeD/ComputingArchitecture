#include <iostream>
#include <cstdint>

int main() {
    int8_t A[8] = {9, 14, 19, 24, 29, 34, 39, 44};
    int8_t B[8] = {2, 3, 4, 5, 6, 7, 8, 9};
    int8_t C[8] = {3, 4, 5, 6, 7, 8, 9, 10};
    int16_t D[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int16_t D1[4] = {0, 0, 0, 0};
    int16_t D2[4] = {0, 0, 0, 0};
    int8_t zero8[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int8_t F[8];
    int8_t temp[8];

    for (int i = 0; i < 4; ++i) {
        D1[i] = D[i];      // Initialize left part of D
        D2[i] = D[i + 4];  // Initialize right part of D
    }

    asm(
            "movq %[B], %%mm1\n"         // Move B in mm1
            "movq %[C], %%mm2\n"         // Move C in mm2
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "punpcklbw %%mm2, %%mm3\n"   // Unpack low bytes of mm2
            "movq %%mm3, %%mm2\n"        // Move mm3 in mm2
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "punpcklbw %%mm1, %%mm3\n"   // Unpack low bytes of mm1
            "movq %%mm3, %%mm1\n"        // Move mm3 in mm1
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "paddsb %%mm2, %%mm1\n"      // B[i] + C[i] left part(high)
            "psrld $8, %%mm1\n"          // Shift mm1 right by 1 byte

            "movq %[D1], %%mm0\n"        // Move D1 in mm0
            "psubsb %%mm0, %%mm1\n"      // Subtract first part of D

            "packsswb %%mm3, %%mm1\n"    // Pack in 8 bit
            "movq %%mm1, %[F]\n"         // Move result in F

            : [F] "+m" (F)
    : [B] "m" (B), [C] "m" (C), [D1] "m" (D1), [zero8] "m" (zero8)
    : "%mm0", "%mm1", "%mm2", "%mm3"
    );

    std::cout << "B + C - D  (left part): ";
    for (signed char i : F) {
        std::cout << static_cast<int>(i) << " ";
    }

    asm(
            "movq %[B], %%mm1\n"         // Move B in mm1
            "movq %[C], %%mm2\n"         // Move C in mm2
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "punpckhbw %%mm2, %%mm3\n"   // Unpack high bytes of mm2
            "movq %%mm3, %%mm2\n"        // Move mm3 in mm2
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "punpckhbw %%mm1, %%mm3\n"   // Unpack high bytes of mm1
            "movq %%mm3, %%mm1\n"        // Move mm3 in mm1
            "movq %[zero8], %%mm3\n"     // Move zeros in mm3

            "paddsb %%mm2, %%mm1\n"      // B[i] + C[i] right part(low)
            "psrld $8, %%mm1\n"          // Shift mm1 right by 1 byte

            "movq %[D2], %%mm0\n"        // Move D2 in mm0
            "psubsb %%mm0, %%mm1\n"      // Subtract second part of D

            "packsswb %%mm3, %%mm1\n"    // Pack in 8 bit
            "movq %%mm1, %[temp]\n"      // Move result in temp

            : [temp] "+m" (temp)
    : [B] "m" (B), [C] "m" (C), [D2] "m" (D2), [zero8] "m" (zero8)
    : "%mm0", "%mm1", "%mm2", "%mm3"
    );

    std::cout << "\nB + C - D (right part): ";
    for (signed char i : temp) {
        std::cout << static_cast<int>(i) << " ";
    }

    for (int i = 0; i < 4; i++) {
        temp[i + 4] = temp[i];
        temp[i] = 0;
    }

    asm(
            "movq %[F], %%mm0\n"         // Load F (left part) in mm0
            "movq %[temp], %%mm1\n"      // Load temp (right part) in mm1

            "paddsb %%mm1, %%mm0\n"      // Merge them into one array

            "movq %%mm0, %[F]\n"         // Move result in F

            : [F] "+m" (F)
    : [temp] "m" (temp)
    : "%mm0", "%mm1"
    );

    std::cout << "\nB + C - D   (all part): ";
    for (signed char i : F) {
        std::cout << static_cast<int>(i) << " ";
    }

    asm(
            "movq %[F], %%mm0\n"         // Load F (current result) in mm0
            "movq %[A], %%mm1\n"         // Load A in mm1

            "psubsw %%mm0, %%mm1\n"      // A[i] - F[i]

            "movq %%mm1, %[F]\n"         // Move result in F

            : [F] "+m" (F)
    : [A] "m" (A)
    : "%mm0", "%mm1"
    );


    std::cout << "\nA - (B + C) - D: ";
    for (unsigned char i : F) {
        std::cout << static_cast<int>(i) << " ";
    }

    return 0;
}
