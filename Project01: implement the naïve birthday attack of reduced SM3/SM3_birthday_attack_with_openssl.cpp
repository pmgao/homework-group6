#include<openssl/evp.h>
#include<openssl/rsa.h>
#include<string>
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
	clock_t start, end;
	start = clock();
	while (1) {
		itoa(i, (char*)input, 10);
		ilen = sizeof(input);
		sm3_openssl(input, ilen, output);
		if (output[0] == tagart[0] && output[1] == tagart[1] && output[2] == tagart[2])
		{
			end = clock();
			printf("find collision!\n");
			printf("time=%us\n", clock() / CLOCKS_PER_SEC);
			break;
		}
		i++;
	}
}
