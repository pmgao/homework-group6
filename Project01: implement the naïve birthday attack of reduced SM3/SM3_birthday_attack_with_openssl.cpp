#include<openssl/evp.h>
#include<openssl/rsa.h>
#include<string>
#include<iostream>
#include<chrono>
using namespace std::chrono;
using std::cout;
using std::endl;
using std::string;

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
	string message = "202100460055";
	uint8_t tagart[32];
	sm3_openssl(message.c_str(), message.size(), tagart);

	uint8_t input[256] = "";
	uint8_t output[32];
	uint64_t ilen = 0;
	uint64_t i = 0;
	auto t1 = steady_clock::now();
	auto t2 = steady_clock::now();
	while (1) {
		itoa(i, (char*)input, 10);
		ilen = sizeof(input);
		sm3_openssl(input, ilen, output);
		if (output[0] == tagart[0] && output[1] == tagart[1] && output[2] == tagart[2])
		{
			t2 = steady_clock::now();
			cout << "find collision!\n";
			cout << "time = " << duration_cast<seconds>(t2 - t1).count() << " us\n";
			break;
		}
		i++;
	}
}
