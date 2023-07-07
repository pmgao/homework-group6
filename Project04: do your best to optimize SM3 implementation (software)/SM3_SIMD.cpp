#include<stdio.h>
#include<bitset>
#include<string.h>
#include<intrin.h>

#define MAX_LEN 2<<12
#define rol(x,j) ((x<<j)|(uint32_t(x)>>(32-j)))
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

#define LOAD_AND_STORE(i) \
    __m128i temp##i = _mm_loadu_si128(src + i); \
    _mm_storeu_si128(dst + i, temp##i);

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

uint32_t IV[8] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d ,0xb0fb0e4e };
char plaintext_after_stuffing[MAX_LEN] = { '\0' };


uint32_t bit_stuffing(uint8_t* plaintext, size_t len) {
    uint64_t bit_len = len * 8;
    uint64_t the_num_of_fin_group = (bit_len >> 3);
    uint32_t the_mod_of_fin_froup = bit_len & 511;
    size_t i, j, k = the_mod_of_fin_froup < 448 ? 1 : 2;
    uint32_t lenth_for_p_after_stuffing = (((len >> 6) + k) << 6);

    __m128i* src = (__m128i*)plaintext;
    __m128i* dst = (__m128i*)plaintext_after_stuffing;
    for (i = 0; i < len; i += 16, src += 4, dst += 4) {
        UNROLL_LOOP_16_1(LOAD_AND_STORE);
    }
    plaintext_after_stuffing[len] = static_cast <char>(0x80);
    for (i = len + 1; i + 8 <= lenth_for_p_after_stuffing; i += 8) {
        plaintext_after_stuffing[i] = 0;
        plaintext_after_stuffing[i + 1] = 0;
        plaintext_after_stuffing[i + 2] = 0;
        plaintext_after_stuffing[i + 3] = 0;
        plaintext_after_stuffing[i + 4] = 0;
        plaintext_after_stuffing[i + 5] = 0;
        plaintext_after_stuffing[i + 6] = 0;
        plaintext_after_stuffing[i + 7] = 0;
    }
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 8] = ((char*)&bit_len)[7];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 7] = ((char*)&bit_len)[6];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 6] = ((char*)&bit_len)[5];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 5] = ((char*)&bit_len)[4];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 4] = ((char*)&bit_len)[3];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 3] = ((char*)&bit_len)[2];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 2] = ((char*)&bit_len)[1];
    plaintext_after_stuffing[lenth_for_p_after_stuffing - 1] = ((char*)&bit_len)[0];
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


void sm3_simd(uint8_t* plaintext, uint32_t* hash_val, size_t len) {
    size_t i;
    uint32_t n = bit_stuffing(plaintext, len) / 64;
    for (i = 0; i < n; i++) {
        CF_for_simd(IV, (int*)&plaintext_after_stuffing[i * 64]);
    }
    for (i = 0; i < 8; i++) {
        hash_val[i] = byte_swap32(IV[i]);
    }
}

static void dump_buf(char* hash, size_t lenth) {
    for (size_t i = 0; i < lenth; i++) {
        printf("%02x", (uint8_t)hash[i]);
    }
}

int main() {
    uint8_t plaintext[MAX_LEN] = "202100460055\0";
    size_t len = strlen((char*)plaintext);
    uint32_t hash_val[8];

    sm3_simd(plaintext, hash_val, len);
    dump_buf((char*)hash_val, 32);

    return 0;
}
