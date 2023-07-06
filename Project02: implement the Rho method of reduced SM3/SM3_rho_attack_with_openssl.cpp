#include"openssl/evp.h"
#include"openssl/rsa.h"
#include<string>
#include<iostream>
#include<chrono>
using namespace std::chrono;
using std::cout;
using std::endl;
using std::string;
#define MAX 32768
#define F(x) ((x*x+128)%MAX)

void sm3_openssl(const void* message, size_t len, uint8_t* hash)
{
	EVP_MD_CTX* md_ctx;
	const EVP_MD* md;

	md = EVP_sm3();
	md_ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, message, len);
	EVP_DigestFinal_ex(md_ctx, hash, NULL);
	EVP_MD_CTX_free(md_ctx);
}


int main()
{
	uint8_t input1[256] = "";
	int32_t len1;
	uint8_t output1[32];

	uint8_t input2[256] = "";
	int32_t len2 = 0;
	uint8_t output2[32];

	int32_t q = 11;
	int32_t i = 13;
	clock_t start, end;
	auto t1 = steady_clock::now();
	auto t2 = steady_clock::now();
	while (1) {

		q = F(q);
		itoa(q, (char*)input1, 10);
		len1 = sizeof(input1);
		i = F(F(i));
		itoa(i, (char*)input2, 10);
		len2 = sizeof(input2);
		sm3_openssl(input1, len1, output1);
		sm3_openssl(input2, len2, output2);

		if (output1[0] == output2[0] && output1[1] == output2[1] && output1[2] == output2[2] && output1[3] == output2[3])
		{
			t2 = steady_clock::now();
			cout << "find collision!\n";
			cout << "time = " << duration_cast<microseconds>(t2 - t1).count() << " us\n";
			break;
		}
	}
	return 0;
}
