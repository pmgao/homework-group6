#include <stdint.h>
#include <stdio.h>
#include <wmmintrin.h>
#define LENGTH 64
#if !defined (ALIGN16)
# if defined (__GNUC__)
# define ALIGN16 __attribute__ ( (aligned (16)))
# else
# define ALIGN16 __declspec (align (16))
# endif
#endif

ALIGN16 uint8_t AES128_TEST_KEY[] = { 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
 0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c };
ALIGN16 uint8_t AES_TEST_VECTOR[] = { 0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
 0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
 0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,
 0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,
 0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,
 0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,
 0xf6,0x9f,0x24,0x45,0xdf,0x4f,0x9b,0x17,
 0xad,0x2b,0x41,0x7b,0xe6,0x6c,0x37,0x10 };
ALIGN16 uint8_t ECB128_EXPECTED[] = { 0x3a,0xd7,0x7b,0xb4,0x0d,0x7a,0x36,0x60,
 0xa8,0x9e,0xca,0xf3,0x24,0x66,0xef,0x97,
 0xf5,0xd3,0xd5,0x85,0x03,0xb9,0x69,0x9d,
 0xe7,0x85,0x89,0x5a,0x96,0xfd,0xba,0xaf,
 0x43,0xb1,0xcd,0x7f,0x59,0x8e,0xce,0x23,
 0x88,0x1b,0x00,0xe3,0xed,0x03,0x06,0x88,
 0x7b,0x0c,0x78,0x5e,0x27,0xe8,0xad,0x3f,
 0x82,0x23,0x20,0x71,0x04,0x72,0x5d,0xd4 };

void print_m128i_with_string(const char* string, __m128i data)
{
	unsigned char* pointer = (unsigned char*)&data;
	int i;
	printf("%-40s[0x", string);
	for (i = 0; i < 16; i++)
		printf("%02x", pointer[i]);
	printf("]\n");
}
void print_m128i_with_string_short(const char* string, __m128i data, int length)
{
	unsigned char* pointer = (unsigned char*)&data;
	int i;
	printf("%-40s[0x", string);
	for (i = 0; i < length; i++)
		printf("%02x", pointer[i]);
	printf("]\n");
}

typedef struct KEY_SCHEDULE {
	ALIGN16 unsigned char KEY[16 * 15];
	unsigned int nr;
}AES_KEY;

inline __m128i AES_128_ASSIST(__m128i temp1, __m128i temp2)
{
	__m128i temp3;
	temp2 = _mm_shuffle_epi32(temp2, 0xff);
	temp3 = _mm_slli_si128(temp1, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp3 = _mm_slli_si128(temp3, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp3 = _mm_slli_si128(temp3, 0x4);
	temp1 = _mm_xor_si128(temp1, temp3);
	temp1 = _mm_xor_si128(temp1, temp2);
	return temp1;
}
void AES_128_Key_Expansion(const unsigned char* userkey, unsigned char* key)
{
	__m128i temp1, temp2;
	__m128i* Key_Schedule = (__m128i*)key;

	temp1 = _mm_loadu_si128((__m128i*)userkey);
	Key_Schedule[0] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x1);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[1] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x2);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[2] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x4);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[3] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x8);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[4] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x10);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[5] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x20);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[6] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x40);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[7] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x80);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[8] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x1b);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[9] = temp1;
	temp2 = _mm_aeskeygenassist_si128(temp1, 0x36);
	temp1 = AES_128_ASSIST(temp1, temp2);
	Key_Schedule[10] = temp1;
}

int AES_set_encrypt_key(const unsigned char* userKey,const int bits,AES_KEY* key)
{
	if (!userKey || !key)
		return -1;
	if (bits == 128)
	{
		AES_128_Key_Expansion(userKey, key->KEY);
		key->nr = 10;
		return 0;
	}
	return -2;
}
int AES_set_decrypt_key(const unsigned char* userKey, const int bits, AES_KEY* key) {
	int i, nr;
	AES_KEY temp_key;
	__m128i* Key_Schedule = (__m128i*)key->KEY;
	__m128i* Temp_Key_Schedule = (__m128i*)temp_key.KEY;
	if (!userKey || !key)
		return -1;
	if (AES_set_encrypt_key(userKey, bits, &temp_key) == -2)
		return -2;
	nr = temp_key.nr;
	key->nr = nr;
	Key_Schedule[nr] = Temp_Key_Schedule[0];
	Key_Schedule[nr - 1] = _mm_aesimc_si128(Temp_Key_Schedule[1]);
	Key_Schedule[nr - 2] = _mm_aesimc_si128(Temp_Key_Schedule[2]);
	Key_Schedule[nr - 3] = _mm_aesimc_si128(Temp_Key_Schedule[3]);
	Key_Schedule[nr - 4] = _mm_aesimc_si128(Temp_Key_Schedule[4]);
	Key_Schedule[nr - 5] = _mm_aesimc_si128(Temp_Key_Schedule[5]);
	Key_Schedule[nr - 6] = _mm_aesimc_si128(Temp_Key_Schedule[6]);
	Key_Schedule[nr - 7] = _mm_aesimc_si128(Temp_Key_Schedule[7]);
	Key_Schedule[nr - 8] = _mm_aesimc_si128(Temp_Key_Schedule[8]);
	Key_Schedule[nr - 9] = _mm_aesimc_si128(Temp_Key_Schedule[9]);
	if (nr > 10) {
		Key_Schedule[nr - 10] = _mm_aesimc_si128(Temp_Key_Schedule[10]);
		Key_Schedule[nr - 11] = _mm_aesimc_si128(Temp_Key_Schedule[11]);
	}
	if (nr > 12) {
		Key_Schedule[nr - 12] = _mm_aesimc_si128(Temp_Key_Schedule[12]);
		Key_Schedule[nr - 13] = _mm_aesimc_si128(Temp_Key_Schedule[13]);
	}
	Key_Schedule[0] = Temp_Key_Schedule[nr];
	return 0;
}


void AES_ECB_encrypt(const unsigned char* in, //pointer to the PLAINTEXT
	unsigned char* out, //pointer to the CIPHERTEXT buffer
	unsigned long length, //text length in bytes
	const char* key, //pointer to the expanded key schedule
	int number_of_rounds) //number of AES rounds 10,12 or 14
{
	__m128i tmp;
	int i, j;
	if (length % 16)
		length = length / 16 + 1;
	else
		length = length / 16;
	for (i = 0; i < length; i++) {
		tmp = _mm_loadu_si128(&((__m128i*)in)[i]);
		tmp = _mm_xor_si128(tmp, ((__m128i*)key)[0]);
		for (j = 1; j < number_of_rounds; j++) {
			tmp = _mm_aesenc_si128(tmp, ((__m128i*)key)[j]);
		}
		tmp = _mm_aesenclast_si128(tmp, ((__m128i*)key)[j]);
		_mm_storeu_si128(&((__m128i*)out)[i], tmp);
	}
}
void AES_ECB_decrypt(const unsigned char* in, //pointer to the CIPHERTEXT
	unsigned char* out, //pointer to the DECRYPTED TEXT buffer
	unsigned long length, //text length in bytes
	const char* key, //pointer to the expanded key schedule
	int number_of_rounds) //number of AES rounds 10,12 or 14
{
	__m128i tmp;
	int i, j;
	if (length % 16)
		length = length / 16 + 1;
	else
		length = length / 16;
	for (i = 0; i < length; i++) {
		tmp = _mm_loadu_si128(&((__m128i*)in)[i]);
		tmp = _mm_xor_si128(tmp, ((__m128i*)key)[0]);
		for (j = 1; j < number_of_rounds; j++) {
			tmp = _mm_aesdec_si128(tmp, ((__m128i*)key)[j]);
		}
		tmp = _mm_aesdeclast_si128(tmp, ((__m128i*)key)[j]);
		_mm_storeu_si128(&((__m128i*)out)[i], tmp);
	}
}

int main() {
	AES_KEY key;
	AES_KEY decrypt_key;
	uint8_t* PLAINTEXT;
	uint8_t* CIPHERTEXT;
	uint8_t* DECRYPTEDTEXT;
	uint8_t* EXPECTED_CIPHERTEXT;
	uint8_t* CIPHER_KEY;
	int i, j;
	int key_length;

	CIPHER_KEY = AES128_TEST_KEY;
	EXPECTED_CIPHERTEXT = ECB128_EXPECTED;
	key_length = 128;

	PLAINTEXT = (uint8_t*)malloc(LENGTH);
	CIPHERTEXT = (uint8_t*)malloc(LENGTH);
	DECRYPTEDTEXT = (uint8_t*)malloc(LENGTH);
	for (i = 0; i < LENGTH / 16 / 4; i++) {
		for (j = 0; j < 4; j++) {
			_mm_storeu_si128(&((__m128i*)PLAINTEXT)[i * 4 + j],
				((__m128i*)AES_TEST_VECTOR)[j]);
		}
	}
	for (j = i * 4; j < LENGTH / 16; j++) {
		_mm_storeu_si128(&((__m128i*)PLAINTEXT)[j],
			((__m128i*)AES_TEST_VECTOR)[j % 4]);
	}
	if (LENGTH % 16) {
		_mm_storeu_si128(&((__m128i*)PLAINTEXT)[j],
			((__m128i*)AES_TEST_VECTOR)[j % 4]);
	}

	AES_set_encrypt_key(CIPHER_KEY, key_length, &key);
	AES_set_decrypt_key(CIPHER_KEY, key_length, &decrypt_key);
	AES_ECB_encrypt(PLAINTEXT,
		CIPHERTEXT,
		LENGTH,
		(const char*)key.KEY,
		key.nr);
	AES_ECB_decrypt(CIPHERTEXT,
		DECRYPTEDTEXT,
		LENGTH,
		(const char*)decrypt_key.KEY,
		decrypt_key.nr);
	printf("The Cipher Key:\n");
	print_m128i_with_string("", ((__m128i*)CIPHER_KEY)[0]);
	if (key_length > 128)
		print_m128i_with_string_short("", ((__m128i*)CIPHER_KEY)[1], (key_length / 8) - 16);
	printf("The Key Schedule:\n");
	for (i = 0; i < key.nr; i++)
		print_m128i_with_string("", ((__m128i*)key.KEY)[i]);

	printf("The PLAINTEXT:\n");
	for (i = 0; i < LENGTH / 16; i++)
		print_m128i_with_string("", ((__m128i*)PLAINTEXT)[i]);
	if (LENGTH % 16)
		print_m128i_with_string_short("", ((__m128i*)PLAINTEXT)[i], LENGTH % 16);
	printf("\n\nThe CIPHERTEXT:\n");
	for (i = 0; i < LENGTH / 16; i++)
		print_m128i_with_string("", ((__m128i*)CIPHERTEXT)[i]);
	if (LENGTH % 16)
		print_m128i_with_string_short("", ((__m128i*)CIPHERTEXT)[i], LENGTH % 16);
	for (i = 0; i < LENGTH; i++) {
		if (CIPHERTEXT[i] != EXPECTED_CIPHERTEXT[i % (16 * 4)]) {
			printf("The CIPHERTEXT is not equal to the EXPECTED CIHERTEXT.\n\n");
			return 1;
		}
	}
	printf("The CIPHERTEXT equals to the EXPECTED CIHERTEXT.\n\n");
	for (i = 0; i < LENGTH; i++) {
		if (DECRYPTEDTEXT[i] != PLAINTEXT[i % (16 * 4)]) {
			printf("The DECRYPTED TEXT isn't equal to the original PLAINTEXT!");
			printf("\n\n");
			return 1;
		}
	}
	printf("The DECRYPTED TEXT equals to the original PLAINTEXT.\n\n");
}
