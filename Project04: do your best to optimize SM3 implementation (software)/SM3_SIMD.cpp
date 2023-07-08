#include<iostream>
#include<bitset>
#include<sstream>
#include<fstream>
#include<intrin.h>
using namespace std;

int IV[8] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d ,0xb0fb0e4e };
int IV2[8] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d ,0xb0fb0e4e };
int T[2] = { 0x79cc4519 ,0x7a879d8a };
char* plaintext_after_stuffing;
int length;

#define NUM  4294967296;
#define MAX_LEN 2<<12
#define rol(x,j) ((x<<j)|(unsigned int(x)>>(32-j)))
#define P0(x) ((x) ^ rol((x), 9) ^ rol((x), 17))
#define P1(x) ((x) ^ rol((x), 15) ^ rol((x), 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
#define _m128i_left(a, k) _mm_or_si128(_mm_slli_epi32(a, k&31), _mm_srli_epi32(a, 32 - k&31))
#define _m128i_P1_simd(X) _mm_xor_si128(_mm_xor_si128(X, _m128i_left(X, 15)), _m128i_left(X, 23))
#define byte_swap32(x) ((x & 0xff000000) >> 24) |((x & 0x00ff0000) >> 8) |((x & 0x0000ff00) << 8) |((x & 0x000000ff) << 24)
#define UNROLL_LOOP_16_64_0(STATEMENT) \
    STATEMENT(16); \
    STATEMENT(17); \
    STATEMENT(18); \
    STATEMENT(19); \
    STATEMENT(20); \
    STATEMENT(21); \
    STATEMENT(22); \
    STATEMENT(23); \
    STATEMENT(24); \
    STATEMENT(25); \
    STATEMENT(26); \
    STATEMENT(27); \
    STATEMENT(28); \
    STATEMENT(29); \
    STATEMENT(30); \
    STATEMENT(31); \
    STATEMENT(32); \
    STATEMENT(33); \
    STATEMENT(34); \
    STATEMENT(35); \
    STATEMENT(36); \
    STATEMENT(37); \
    STATEMENT(38); \
    STATEMENT(39); \
    STATEMENT(40); \
    STATEMENT(41); \
    STATEMENT(42); \
    STATEMENT(43); \
    STATEMENT(44); \
    STATEMENT(45); \
    STATEMENT(46); \
    STATEMENT(47); \
    STATEMENT(48); \
    STATEMENT(49); \
    STATEMENT(50); \
    STATEMENT(51); \
    STATEMENT(52); \
    STATEMENT(53); \
    STATEMENT(54); \
    STATEMENT(55); \
    STATEMENT(56); \
    STATEMENT(57); \
    STATEMENT(58); \
    STATEMENT(59); \
    STATEMENT(60); \
    STATEMENT(61); \
    STATEMENT(62); \
    STATEMENT(63);
#define UNROLL_LOOP_16_64_1(STATEMENT) \
   STATEMENT(0); \
   STATEMENT(4); \
   STATEMENT(8); \
   STATEMENT(12); \
   STATEMENT(16); \
   STATEMENT(20); \
   STATEMENT(24); \
   STATEMENT(28); \
   STATEMENT(32);\
   STATEMENT(36);\
   STATEMENT(40);\
   STATEMENT(44);\
   STATEMENT(48);\
   STATEMENT(52);\
   STATEMENT(56);\
   STATEMENT(60)
#define UNROLL_LOOP_16_0(STATEMENT) \
    STATEMENT(0); \
    STATEMENT(1); \
    STATEMENT(2); \
    STATEMENT(3); \
    STATEMENT(4); \
    STATEMENT(5); \
    STATEMENT(6); \
    STATEMENT(7); \
    STATEMENT(8); \
    STATEMENT(9); \
    STATEMENT(10); \
    STATEMENT(11); \
    STATEMENT(12); \
    STATEMENT(13); \
    STATEMENT(14); \
    STATEMENT(15);
#define UNROLL_LOOP_16_1(STATEMENT) \
    STATEMENT(0); \
    STATEMENT(1); \
    STATEMENT(2); \
    STATEMENT(3);
#define UNROLL_LOOP_4_16(STATEMENT) \
    STATEMENT(4); \
    STATEMENT(5); \
    STATEMENT(6); \
    STATEMENT(7); \
    STATEMENT(8); \
    STATEMENT(9); \
    STATEMENT(10); \
    STATEMENT(11); \
    STATEMENT(12); \
    STATEMENT(13); \
    STATEMENT(14); \
    STATEMENT(15); \
    STATEMENT(16); 

#define BYTE_SWAP_W(i) W[i] = byte_swap32(BB[i]);

#define LOAD_AND_XOR(index) \
    __m128i w1_##index = _mm_loadu_si128((__m128i*)(W + index)); \
    __m128i w2_##index = _mm_loadu_si128((__m128i*)(W + index + 4)); \
    __m128i w_t_##index = _mm_xor_si128(w1_##index, w2_##index); \
    _mm_storeu_si128((__m128i*)(W_t + index), w_t_##index);

#define LOOP_BODY1(i) \
temp = rol(A, 12) + E + rol(0x79cc4519, i);\
SS1 = rol(temp, 7); SS2 = SS1 ^ rol(A, 12);\
TT1 = FF0(A, B, C) + D + SS2 + W_t[i]; TT2 = GG0(E, F, G) + H + SS1 + W[i];\
D = C; C = rol(B, 9); B = A; A = TT1; H = G; G = rol(F, 19); F = E; E = P0(TT2);

#define LOOP_BODY2(i) \
temp = rol(A, 12) + E + rol(0x7a879d8a, i % 32);\
SS1 = rol(temp, 7); SS2 = SS1 ^ rol(A, 12);\
TT1 = FF1(A, B, C) + D + SS2 + W_t[i]; TT2 = GG1(E, F, G) + H + SS1 + W[i];\
D = C; C = rol(B, 9); B = A; A = TT1; H = G; G = rol(F, 19); F = E; E = P0(TT2);

#define UPDATE_W(j) \
do { \
    W[(j << 2)] = P1(W[(j << 2) - 16] ^ W[(j << 2) - 9] ^ (rol(W[(j << 2) - 3], 15))) ^ rol(W[(j << 2) - 13], 7) ^ W[(j << 2) - 6]; \
    w16 = _mm_setr_epi32(W[(j << 2) - 16], W[(j << 2) - 15], W[(j << 2) - 14], W[(j << 2) - 13]); \
    w9 = _mm_setr_epi32(W[(j << 2) - 9], W[(j << 2) - 8], W[(j << 2) - 7], W[(j << 2) - 6]); \
    w13 = _mm_setr_epi32(W[(j << 2) - 13], W[(j << 2) - 12], W[(j << 2) - 11], W[(j << 2) - 10]); \
    w3 = _mm_setr_epi32(W[(j << 2) - 3], W[(j << 2) - 2], W[(j << 2) - 1], W[(j << 2)]); \
    w6 = _mm_setr_epi32(W[(j << 2) - 6], W[(j << 2) - 5], W[(j << 2) - 4], W[(j << 2) - 3]); \
    w16_or_w9 = _mm_xor_si128(w16, w9); \
    rsl_w3 = _m128i_left(w3, 15); \
    rsl_w13 = _m128i_left(w13, 7); \
    w16_or_w9_or_rsl_w3 = _mm_xor_si128(w16_or_w9, rsl_w3); \
    rsl_w13_or_w6 = _mm_xor_si128(rsl_w13, w6); \
    P = _m128i_P1_simd(w16_or_w9_or_rsl_w3); \
    re = _mm_xor_si128(P, rsl_w13_or_w6); \
    memcpy(&W[(j << 2)], (int*)&re, 16); \
} while (0)

//输出结果
static void dump_buf(char* ciphertext_32, int lenth) {
    for (int i = 0; i < lenth; i++) {
        printf("%02X ", (unsigned char)ciphertext_32[i]);
    }
    printf("\n");
}

int bit_stuffing(char plaintext[], int length_for_plaintext) {
    long long bit_len = uint64_t(length_for_plaintext) * 8;
    int lenth_for_p_after_stuffing = (length_for_plaintext / 64 + 1) * 64;
    plaintext_after_stuffing = new char[lenth_for_p_after_stuffing];
    memcpy(plaintext_after_stuffing, plaintext, length_for_plaintext);
    plaintext_after_stuffing[length_for_plaintext] = 0x80;
    for (int i = length_for_plaintext + 1; i < lenth_for_p_after_stuffing - 8; i++) {
        plaintext_after_stuffing[i] = 0;
    }

    for (int i = lenth_for_p_after_stuffing - 8, j = 0; i < lenth_for_p_after_stuffing; i++, j++) {
        plaintext_after_stuffing[i] = ((char*)&bit_len)[7 - j];
    }

    return lenth_for_p_after_stuffing;
}

int bit_stuff_for_length_attack(char plaintext[], int length_for_plaintext, int length_for_message) {
    long long bit_len = uint64_t(length_for_plaintext + length_for_message) * 8;
    int lenth_for_p_after_stuffing = (length_for_plaintext / 64 + 1) * 64;
    plaintext_after_stuffing = new char[lenth_for_p_after_stuffing];
    memcpy(plaintext_after_stuffing, plaintext, length_for_plaintext);
    plaintext_after_stuffing[length_for_plaintext] = 0x80;
    for (int i = length_for_plaintext + 1; i < lenth_for_p_after_stuffing - 8; i++) {
        plaintext_after_stuffing[i] = 0;
    }

    for (int i = lenth_for_p_after_stuffing - 8, j = 0; i < lenth_for_p_after_stuffing; i++, j++) {
        plaintext_after_stuffing[i] = ((char*)&bit_len)[7 - j];
    }

    return lenth_for_p_after_stuffing;
}

void CF_for_simd(uint32_t* V, int* BB) {
    uint32_t W[68];
    uint32_t W_t[64];
    __m128i w16, w9, w13, w3, w6, w16_or_w9, rsl_w3, rsl_w13, w16_or_w9_or_rsl_w3, rsl_w13_or_w6, P, re;
    uint32_t temp, SS1, SS2, TT1, TT2;

    UNROLL_LOOP_16_0(BYTE_SWAP_W);
    UNROLL_LOOP_4_16(UPDATE_W);
    UNROLL_LOOP_16_64_1(LOAD_AND_XOR);
    int A = V[0], B = V[1], C = V[2], D = V[3], E = V[4], F = V[5], G = V[6], H = V[7];
    UNROLL_LOOP_16_0(LOOP_BODY1);
    UNROLL_LOOP_16_64_0(LOOP_BODY2);
    V[0] = A ^ V[0], V[1] = B ^ V[1], V[2] = C ^ V[2], V[3] = D ^ V[3], V[4] = E ^ V[4], V[5] = F ^ V[5], V[6] = G ^ V[6], V[7] = H ^ V[7];

}

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
    dump_buf((char*)new_hash_val, 32);
    return 0;
}

int main() {
    char plaintext[] = "Hello world!";
    int hash_val[8];
    int hash_val2[8];
    sm3_simd(plaintext, hash_val, sizeof(plaintext));
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
    dump_buf((char*)hash_val, 32);
    sm3_length_attack(memappend, hash_val2, sizeof(memappend), 64);


    return 0;
}
