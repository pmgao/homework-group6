#include"openssl/evp.h"
#include"openssl/rsa.h"
#include<iostream>
#define MAX 32768
#define F(x) ((x*x+128)%MAX)

int32_t sm3_openssl(const void* message, size_t len, uint8_t* hash)
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

int32_t inttohex(int32_t aa, uint8_t* buffer)
{
	static int32_t  i = 0;
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
	uint8_t tinput[256] = "";
	int32_t tlen;
	uint8_t tagart[32];

	uint8_t input[256] = "";
	int32_t ilen = 0;
	uint8_t output[32];

	int32_t q = 11;
	int32_t i = 51;
	clock_t start, end;
	start = clock();
	while (1) {

		q = F(q);
		ilen = inttohex(q, input);
		i = F(F(i));
		tlen = inttohex(i, tinput);
		sm3_openssl(tinput, tlen, tagart);
		sm3_openssl(input, ilen, output);

		if (output[0] == tagart[0] && output[1] == tagart[1] && output[2] == tagart[2] && output[3] == tagart[3])
		{
			end = clock();
			printf("solved\n");
			printf("time=%us\n", clock() / CLOCKS_PER_SEC);

			break;
		}

	}
	return 0;
}
