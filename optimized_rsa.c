#include <stdio.h>
#include <stdint.h>

// Montgomery Modular Multiplication: computes Z = X * Y * R^(-1) mod M
uint32_t montgomery_multiply(uint32_t X, uint32_t Y, uint32_t M, uint32_t m) {
    uint32_t T = 0; // Initialize T to 0
    uint32_t n;     // eta in the document
    uint32_t Y0 = Y & 1; // Least significant bit of Y

    // Ensure M is odd (required for Montgomery multiplication)
    if (M % 2 == 0) {
        fprintf(stderr, "Error: Modulus M must be odd\n");
        return 0;
    }

    // Bit-wise Montgomery multiplication loop
    for (uint32_t i = 0; i < m; i++) {
        uint32_t Xi = (X >> i) & 1; // Get i-th bit of X
        n = (T & 1) ^ (Xi & Y0);   // eta = T(0) XOR (X(i) AND Y(0))
        
        // T = (T + X(i) * Y + n * M) >> 1
        T = T + (Xi ? Y : 0) + (n ? M : 0);
        T >>= 1;

        // If T >= M, subtract M
        if (T >= M) {
            T -= M;
        }
    }

    // Final reduction if necessary
    if (T >= M) {
        T -= M;
    }

    return T;
}

// Helper function to compute R = 2^m mod M
uint32_t compute_R_mod_M(uint32_t m, uint32_t M) {
    uint64_t R = 1ULL << m; // 2^m
    return R % M;
}

// Modular exponentiation using Montgomery multiplication
uint32_t montgomery_mod_exp(uint32_t base, uint32_t exp, uint32_t M) {
    // Determine bit-length m (smallest m such that 2^m > M)
    uint32_t m = 0;
    uint32_t temp = M;
    while (temp > 0) {
        temp >>= 1;
        m++;
    }

    // Compute R = 2^m mod M
    uint32_t R = compute_R_mod_M(m, M);

    // Pre-scale base: base * R mod M
    uint32_t base_scaled = montgomery_multiply(base, R * R % M, M, m);

    // Initialize result in Montgomery domain: 1 * R mod M
    uint32_t result = montgomery_multiply(1, R * R % M, M, m);

    // Square-and-multiply algorithm
    while (exp > 0) {
        if (exp & 1) { // If exponent bit is 1
            result = montgomery_multiply(result, base_scaled, M, m);
        }
        base_scaled = montgomery_multiply(base_scaled, base_scaled, M, m);
        exp >>= 1;
    }

    // Convert result back from Montgomery domain: result * 1 mod M
    result = montgomery_multiply(result, 1, M, m);

    return result;
}

int main() {
    uint32_t P = 61, Q = 53;
    uint32_t N = P * Q; // 3233
    uint32_t E = 17, D = 2753;
    uint32_t T = 123; // Plaintext

    // Determine bit-length m for N
    uint32_t m = 0;
    uint32_t temp = N;
    while (temp > 0) {
        temp >>= 1;
        m++;
    }

    // Encryption: C = T^E mod N
    uint32_t C = montgomery_mod_exp(T, E, N);
    printf("Ciphertext: %u\n", C); // Should print 855

    // Decryption: T = C^D mod N
    uint32_t decrypted = montgomery_mod_exp(C, D, N);
    printf("Decrypted: %u\n", decrypted); // Should print 123

    // Example from document: Montgomery multiplication for X=17, Y=22, M=23
    uint32_t X = 17, Y = 22, M = 23, m_example = 5;
    uint32_t result = montgomery_multiply(X, Y * compute_R_mod_M(m_example, M) % M, M, m_example);
    result = montgomery_multiply(result, 1, M, m_example); // Convert back
    printf("Montgomery X*Y mod M: %u\n", result); // Should print 16

    return 0;
}