#include<openssl/evp.h>
#include<openssl/rsa.h>
#include<string>
using std::string;

uint64_t sm3_openssl(const void* message, size_t len, uint8_t* hash)
{
	EVP_MD_CTX* md_ctx;
	const EVP_MD* md;

	md = EVP_sm3();
	md_ctx = EVP_MD_CTX_new();
	EVP_DigestInit_ex(md_ctx, md, NULL);
	EVP_DigestUpdate(md_ctx, message, len);
	EVP_DigestFinal_ex(md_ctx, hash, NULL);
	EVP_MD_CTX_free(md_ctx);
	return 0;
}

uint64_t inttohex(uint64_t aa, uint8_t* buffer)
{
	static uint64_t  i = 0;
	i = 0;
	if (aa < 16)
	{
		if (aa < 10)
			buffer[i] = aa + '0';
		else
			buffer[i] = aa - 10 + 'a';
		buffer[i + 1] = '\0';
	}
	else
	{
		inttohex(aa / 16, buffer);
		i++;
		aa %= 16;
		if (aa < 10)
			buffer[i] = aa + '0';
		else
			buffer[i] = aa - 10 + 'a';
	}

	return i + 1;
}

int main()
{
	string tinput = "abcdef01234569";
	uint8_t tagart[32];
	sm3_openssl(tinput.c_str(), tinput.size(), tagart);

	uint8_t input[256] = "";
	uint8_t output[32];
	uint64_t ilen = 0;
	uint64_t i = 0;
	clock_t start, end;
	start = clock();
	while (1) {
		ilen = inttohex(i, input);
		sm3_openssl(input, ilen, output);

		if (output[0] == tagart[0] && output[1] == tagart[1] && output[2] == tagart[2])
		{
			end = clock();
			printf("solved!\n");
			printf("time=%us\n", clock() / CLOCKS_PER_SEC);
			break;
		}
		i++;
	}
}
