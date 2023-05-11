#include<stdint.h>
#include<stdio.h>
#include<intrin.h>
#include<chrono>
using namespace std::chrono;

#define rotl32(value, shift) ((value << shift) | value >> (32 - shift))
#define ROTL_EPI32(a, n) _mm_xor_si128(_mm_slli_epi32(a, n), _mm_srli_epi32(a, 32 - n))
#define XOR3(a, b, c) _mm_xor_si128(a, _mm_xor_si128(b, c))
#define XOR4(a, b, c, d) _mm_xor_si128(a, XOR3(b, c, d))
#define XOR5(a, b, c, d, e) _mm_xor_si128(a, XOR4(b, c, d, e))
#define XOR6(a, b, c, d, e, f) _mm_xor_si128(a, XOR5(b, c, d, e, f))
#define MulMatrix(x, higherMask, lowerMask) \
    (_mm_xor_si128(_mm_shuffle_epi8(lowerMask, _mm_and_si128(x, _mm_set1_epi32(0x0f0f0f0f))), \
                    _mm_shuffle_epi8(higherMask, _mm_and_si128(_mm_srli_epi16(x, 4), _mm_set1_epi32(0x0f0f0f0f)))))

#define LOAD_KEY(index)																									             \
    do {                                                             													             \
        k[index] = (key[index << 2] << 24) | (key[(index << 2) + 1] << 16) | (key[(index << 2) + 2] << 8) | (key[(index << 2) + 3]); \
        k[index] = k[index] ^ FK[index];																				             \
    } while (0)

#define KEY_INIT_ITERATION(index)                                               \
    do {                                                                        \
        temp = k[1] ^ k[2] ^ k[3] ^ CK[index];                                  \
        temp = (SBox[temp >> 24] << 24) | (SBox[(temp >> 16) & 0xFF] << 16) |   \
              (SBox[(temp >> 8) & 0xFF] << 8) | (SBox[temp & 0xFF]);            \
        rk[index] = k[0] ^ temp ^ rotl32(temp, 13) ^ rotl32(temp, 23);          \
        k[0] = k[1];                                                            \
        k[1] = k[2];                                                            \
        k[2] = k[3];                                                            \
        k[3] = rk[index];                                                       \
    } while (0)

#define SM4_ITERATION(index)                                                 \
    do {                                                                     \
        __m128i k = _mm_set1_epi32((enc == 0) ? rk[index] : rk[31 - index]); \
        temp = XOR4(Block[1], Block[2], Block[3], k);                        \
        temp = SM4_SBox_TO_AES(temp);                                        \
        temp = XOR6(Block[0], temp, ROTL_EPI32(temp, 2),                     \
            ROTL_EPI32(temp, 10), ROTL_EPI32(temp, 18),                      \
            ROTL_EPI32(temp, 24));                                           \
        Block[0] = Block[1];                                                 \
        Block[1] = Block[2];                                                 \
        Block[2] = Block[3];                                                 \
        Block[3] = temp;                                                     \
    } while (0)

#define UNROLL_LOOP_31_0(STATEMENT) \
    STATEMENT(0);\
    STATEMENT(1);\
    STATEMENT(2);\
    STATEMENT(3);\
    STATEMENT(4);\
    STATEMENT(5);\
    STATEMENT(6);\
    STATEMENT(7);\
    STATEMENT(8);\
    STATEMENT(9);\
    STATEMENT(10);\
    STATEMENT(11);\
    STATEMENT(12);\
    STATEMENT(13);\
    STATEMENT(14);\
    STATEMENT(15);\
    STATEMENT(16);\
    STATEMENT(17);\
    STATEMENT(18);\
    STATEMENT(19);\
    STATEMENT(20);\
    STATEMENT(21);\
    STATEMENT(22);\
    STATEMENT(23);\
    STATEMENT(24);\
    STATEMENT(25);\
    STATEMENT(26);\
    STATEMENT(27);\
    STATEMENT(28);\
    STATEMENT(29);\
    STATEMENT(30);\
    STATEMENT(31); 

uint32_t FK[4] = { 0xa3b1bac6, 0x56aa3350, 0x677d9197, 0xb27022dc };
uint32_t CK[32] = {
   0x00070e15, 0x1c232a31, 0x383f464d, 0x545b6269, 0x70777e85, 0x8c939aa1,
   0xa8afb6bd, 0xc4cbd2d9, 0xe0e7eef5, 0xfc030a11, 0x181f262d, 0x343b4249,
   0x50575e65, 0x6c737a81, 0x888f969d, 0xa4abb2b9, 0xc0c7ced5, 0xdce3eaf1,
   0xf8ff060d, 0x141b2229, 0x30373e45, 0x4c535a61, 0x686f767d, 0x848b9299,
   0xa0a7aeb5, 0xbcc3cad1, 0xd8dfe6ed, 0xf4fb0209, 0x10171e25, 0x2c333a41,
   0x484f565d, 0x646b7279 };
uint8_t SBox[256] = {
   0xD6, 0x90, 0xE9, 0xFE, 0xCC, 0xE1, 0x3D, 0xB7, 0x16, 0xB6, 0x14, 0xC2,
   0x28, 0xFB, 0x2C, 0x05, 0x2B, 0x67, 0x9A, 0x76, 0x2A, 0xBE, 0x04, 0xC3,
   0xAA, 0x44, 0x13, 0x26, 0x49, 0x86, 0x06, 0x99, 0x9C, 0x42, 0x50, 0xF4,
   0x91, 0xEF, 0x98, 0x7A, 0x33, 0x54, 0x0B, 0x43, 0xED, 0xCF, 0xAC, 0x62,
   0xE4, 0xB3, 0x1C, 0xA9, 0xC9, 0x08, 0xE8, 0x95, 0x80, 0xDF, 0x94, 0xFA,
   0x75, 0x8F, 0x3F, 0xA6, 0x47, 0x07, 0xA7, 0xFC, 0xF3, 0x73, 0x17, 0xBA,
   0x83, 0x59, 0x3C, 0x19, 0xE6, 0x85, 0x4F, 0xA8, 0x68, 0x6B, 0x81, 0xB2,
   0x71, 0x64, 0xDA, 0x8B, 0xF8, 0xEB, 0x0F, 0x4B, 0x70, 0x56, 0x9D, 0x35,
   0x1E, 0x24, 0x0E, 0x5E, 0x63, 0x58, 0xD1, 0xA2, 0x25, 0x22, 0x7C, 0x3B,
   0x01, 0x21, 0x78, 0x87, 0xD4, 0x00, 0x46, 0x57, 0x9F, 0xD3, 0x27, 0x52,
   0x4C, 0x36, 0x02, 0xE7, 0xA0, 0xC4, 0xC8, 0x9E, 0xEA, 0xBF, 0x8A, 0xD2,
   0x40, 0xC7, 0x38, 0xB5, 0xA3, 0xF7, 0xF2, 0xCE, 0xF9, 0x61, 0x15, 0xA1,
   0xE0, 0xAE, 0x5D, 0xA4, 0x9B, 0x34, 0x1A, 0x55, 0xAD, 0x93, 0x32, 0x30,
   0xF5, 0x8C, 0xB1, 0xE3, 0x1D, 0xF6, 0xE2, 0x2E, 0x82, 0x66, 0xCA, 0x60,
   0xC0, 0x29, 0x23, 0xAB, 0x0D, 0x53, 0x4E, 0x6F, 0xD5, 0xDB, 0x37, 0x45,
   0xDE, 0xFD, 0x8E, 0x2F, 0x03, 0xFF, 0x6A, 0x72, 0x6D, 0x6C, 0x5B, 0x51,
   0x8D, 0x1B, 0xAF, 0x92, 0xBB, 0xDD, 0xBC, 0x7F, 0x11, 0xD9, 0x5C, 0x41,
   0x1F, 0x10, 0x5A, 0xD8, 0x0A, 0xC1, 0x31, 0x88, 0xA5, 0xCD, 0x7B, 0xBD,
   0x2D, 0x74, 0xD0, 0x12, 0xB8, 0xE5, 0xB4, 0xB0, 0x89, 0x69, 0x97, 0x4A,
   0x0C, 0x96, 0x77, 0x7E, 0x65, 0xB9, 0xF1, 0x09, 0xC5, 0x6E, 0xC6, 0x84,
   0x18, 0xF0, 0x7D, 0xEC, 0x3A, 0xDC, 0x4D, 0x20, 0x79, 0xEE, 0x5F, 0x3E,
   0xD7, 0xCB, 0x39, 0x48 };

inline static void SM4_generate_RK(uint8_t* key, uint32_t* rk) {
	uint32_t k[4];
	uint32_t temp;
	LOAD_KEY(0);
	LOAD_KEY(1);
	LOAD_KEY(2);
	LOAD_KEY(3);
	UNROLL_LOOP_31_0(KEY_INIT_ITERATION);
}

inline static __m128i MulMatrixToAES(__m128i x) {
    __m128i higherMask = _mm_set_epi8(0x22, 0x58, 0x1a, 0x60, 0x02, 0x78, 0x3a, 0x40, 0x62, 0x18,
        0x5a, 0x20, 0x42, 0x38, 0x7a, 0x00);
    __m128i lowerMask = _mm_set_epi8(0xe2, 0x28, 0x95, 0x5f, 0x69, 0xa3, 0x1e, 0xd4, 0x36, 0xfc,
        0x41, 0x8b, 0xbd, 0x77, 0xca, 0x00);
    return MulMatrix(x, higherMask, lowerMask);
}

inline static __m128i MulMatrixBack(__m128i x) {
	__m128i higherMask = _mm_set_epi8(0x14, 0x07, 0xc6, 0xd5, 0x6c, 0x7f, 0xbe, 0xad, 0xb9, 0xaa,
		0x6b, 0x78, 0xc1, 0xd2, 0x13, 0x00);
	__m128i lowerMask = _mm_set_epi8(0xd8, 0xb8, 0xfa, 0x9a, 0xc5, 0xa5, 0xe7, 0x87, 0x5f, 0x3f,
		0x7d, 0x1d, 0x42, 0x22, 0x60, 0x00);
	return MulMatrix(x, higherMask, lowerMask);
}

inline static __m128i SM4_SBox_TO_AES(__m128i x) {
	__m128i mask = _mm_set_epi8(0x03, 0x06, 0x09, 0x0c, 0x0f, 0x02, 0x05, 0x08,
		0x0b, 0x0e, 0x01, 0x04, 0x07, 0x0a, 0x0d, 0x00);

	x = _mm_shuffle_epi8(x, mask);
	x = _mm_xor_si128(MulMatrixToAES(x), _mm_set1_epi8(0b00100011));
	x = _mm_aesenclast_si128(x, _mm_setzero_si128());

	return _mm_xor_si128(MulMatrixBack(x), _mm_set1_epi8(0b00111011));
}

void SM4_AESNI(uint8_t* in, uint8_t* out, uint32_t* rk, bool enc) {
    __m128i temp, Block[4];
	__m128i vindex;
	temp = _mm_loadu_si128((__m128i*)in);

	vindex = _mm_setr_epi8(3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12);
	Block[0] = _mm_unpacklo_epi64(_mm_unpacklo_epi32(temp, temp), _mm_unpacklo_epi32(temp, temp));
	Block[1] = _mm_unpackhi_epi64(_mm_unpacklo_epi32(temp, temp), _mm_unpacklo_epi32(temp, temp));
	Block[2] = _mm_unpacklo_epi64(_mm_unpackhi_epi32(temp, temp), _mm_unpackhi_epi32(temp, temp));
	Block[3] = _mm_unpackhi_epi64(_mm_unpackhi_epi32(temp, temp), _mm_unpackhi_epi32(temp, temp));
	Block[0] = _mm_shuffle_epi8(Block[0], vindex);
	Block[1] = _mm_shuffle_epi8(Block[1], vindex);
	Block[2] = _mm_shuffle_epi8(Block[2], vindex);
	Block[3] = _mm_shuffle_epi8(Block[3], vindex);
	UNROLL_LOOP_31_0(SM4_ITERATION);

	Block[0] = _mm_shuffle_epi8(Block[0], vindex);
	Block[1] = _mm_shuffle_epi8(Block[1], vindex);
	Block[2] = _mm_shuffle_epi8(Block[2], vindex);
	Block[3] = _mm_shuffle_epi8(Block[3], vindex);
	_mm_storeu_si128((__m128i*)out, _mm_unpacklo_epi64(_mm_unpacklo_epi32(Block[3], Block[2]), _mm_unpacklo_epi32(Block[1], Block[0])));
}

int main() {
	uint8_t in[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	uint8_t key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	uint32_t rk[32];
    steady_clock::time_point start, end;

    SM4_generate_RK(key, rk);
    
    start = steady_clock::now();
	SM4_AESNI(in, in, rk, 0);
    end = steady_clock::now();
    printf("%lld us for encryption\n", duration_cast<microseconds>(end - start).count());

	printf("Cipher text:\n");
	for (int i = 0; i < 16; i++) {
		printf("%02x ", in[i]);
	}
	printf("\n\n");

	
    start = steady_clock::now();
    SM4_AESNI(in, in, rk, 1);
    end = steady_clock::now();
    printf("%lld us for decryption\n", duration_cast<microseconds>(end - start).count());

    printf("Plain text:\n");
	for (int i = 0; i < 16; i++) {
		printf("%02x ", in[i]);
	}
    printf("\n\n");

	return 0;
}
