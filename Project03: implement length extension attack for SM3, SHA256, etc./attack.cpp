#include "./SM4.h"

void sm3_simd(char plaintext[], int* hash_val, int lenth_for_plaintext) {
    int n = bit_stuffing(plaintext, lenth_for_plaintext) / 64;
    for (int i = 0; i < n; i++) {
        CF_for_simd((uint32_t*)IV, (int*)&plaintext_after_stuffing[i * 64]);
    }
    for (int i = 0; i < 8; i++) {
        hash_val[i] = byte_swap32(IV[i]);
    }
    memcpy(IV, IV2, 32);
}


void sm3_for_length_attack(char plaintext[], int* hash_val, int lenth_for_plaintext, int length_for_message) {
    int n = bit_stuff_for_length_attack(plaintext, lenth_for_plaintext, length_for_message) / 64;
    for (int i = 0; i < n; i++) {
        CF_for_simd((uint32_t*)IV, (int*)&plaintext_after_stuffing[i * 64]);
    }
    for (int i = 0; i < 8; i++) {
        hash_val[i] = byte_swap32(IV[i]);
    }
    memcpy(IV, IV2, 32);
}

int sm3_length_attack(char* memappend, int* hash_val, int length_formemappend, int length_for_message) {
    int new_hash_val[8];
    memcpy(IV, hash_val, 32);
    sm3_for_length_attack(memappend, new_hash_val, length_formemappend, length_for_message);
    printf("Hash value of constructed message:\n");
    dump_buf((char*)new_hash_val, 32);
    return 0;
}

int main() {
    char plaintext[] = "Hello world!";
    int hash_val[8];
    int hash_val2[8];
    sm3_simd(plaintext, hash_val, sizeof(plaintext));
    printf("Hash value of origin message:\n");
    dump_buf((char*)hash_val, 32);

    for (int i = 0; i < 8; i++) {
        hash_val2[i] = byte_swap32(hash_val[i]);
    }
    bit_stuffing(plaintext, sizeof(plaintext));
    char plaintext_for_length_attack[64 + sizeof(plaintext)];
    memcpy(plaintext_for_length_attack, plaintext_after_stuffing, 64);
    char memappend[] = "202100460055";
    memcpy(&plaintext_for_length_attack[64], memappend, sizeof(memappend));
    sm3_simd(plaintext_for_length_attack, hash_val, 64 + sizeof(memappend));
    printf("Hash value of new message:\n");
    dump_buf((char*)hash_val, 32);
    sm3_length_attack(memappend, hash_val2, sizeof(memappend), 64);

    return 0;
}
