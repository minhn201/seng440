#include <stdio.h>
#include <stdint.h>

// Montgomery Modular Multiplication: computes Z = X * Y * R^(-1) mod M
int montgomery_multiply(int X, int Y, int M, int m) {
    int T = 0; // Initialize T to 0
    int n;     // eta in the document
    int Y0 = Y & 1; // Least significant bit of Y

    // Ensure M is odd (required for Montgomery multiplication)
    if (M % 2 == 0) {
        fprintf(stderr, "Error: Modulus M must be odd\n");
        return 0;
    }

    // Bit-wise Montgomery multiplication loop
    for (int i = 0; i < m; i++) {
        int Xi = (X >> i) & 1; // Get i-th bit of X
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
int compute_R_mod_M(int m, int M) {
    int R = 1ULL << m; // 2^m
    return R % M;
}

// Modular exponentiation using Montgomery multiplication
int montgomery_mod_exp(int base, int exp, int M) {
    // Determine bit-length m (smallest m such that 2^m > M)
    int m = 0;
    int temp = M;
    while (temp > 0) {
        temp >>= 1;
        m++;
    }

    // Compute R = 2^m mod M
    int R = compute_R_mod_M(m, M);

    // Pre-scale base: base * R mod M
    int base_scaled = montgomery_multiply(base, R * R % M, M, m);

    // Initialize result in Montgomery domain: 1 * R mod M
    int result = montgomery_multiply(1, R * R % M, M, m);

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
    int P = 61, Q = 53;
    int N = P * Q; // 3233
    int E = 17, D = 2753;
    int T = 123; // Plaintext

    // Determine bit-length m for N
    int m = 0;
    int temp = N;
    while (temp > 0) {
        temp >>= 1;
        m++;
    }

    // Encryption: C = T^E mod N
    int C = montgomery_mod_exp(T, E, N);
    printf("Ciphertext: %u\n", C); // Should print 855

    // Decryption: T = C^D mod N
    int decrypted = montgomery_mod_exp(C, D, N);
    printf("Decrypted: %u\n", decrypted); // Should print 123

    /*
    // Example from document: Montgomery multiplication for X=17, Y=22, M=23
    int X = 17, Y = 22, M = 23, m_example = 5;
    int result = montgomery_multiply(X, Y * compute_R_mod_M(m_example, M) % M, M, m_example);
    result = montgomery_multiply(result, 1, M, m_example); // Convert back
    printf("Montgomery X*Y mod M: %u\n", result); // Should print 16
    */
   
    return 0;
}