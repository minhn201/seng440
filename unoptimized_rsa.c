#include <stdio.h>
#include <stdint.h>

// Modular exponentiation: base^exp mod m
uint32_t mod_exp(uint32_t base, uint32_t exp, uint32_t m) {
    uint32_t result = 1;
    base = base % m;
    while (exp > 0) {
        if (exp & 1) // If exp is odd
            result = (result * base) % m;
        exp = exp >> 1;
        base = (base * base) % m;
    }
    return result;
}

int main() {
    uint32_t P = 61, Q = 53;
    uint32_t N = P * Q; // 3233
    uint32_t E = 17, D = 2753;
    uint32_t T = 123; // Plaintext

    // Encryption
    uint32_t C = mod_exp(T, E, N);
    printf("Ciphertext: %u\n", C); // Should print 855

    // Decryption
    uint32_t decrypted = mod_exp(C, D, N);
    printf("Decrypted: %u\n", decrypted); // Should print 123

    return 0;
}
