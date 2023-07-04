#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/objects.h>
#include <openssl/ecdsa.h>
#include <openssl/bn.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <string>
#include <iostream>

using namespace std;
BIGNUM* SK1 = BN_new();
BIGNUM* PK1[2];
BIGNUM* sig1[2];
BIGNUM* sig2[2];
BIGNUM* N = BN_new();

void init() {
	PK1[0] = BN_new();
	PK1[1] = BN_new();
	sig1[0] = BN_new();
	sig1[1] = BN_new();

	sig2[0] = BN_new();
	sig2[1] = BN_new();

	BN_hex2bn(&PK1[0], "2b4a8cabec11ba85b0cbe50ddd5a36f9449a79fdee442e11333a49ef32c9e5cb");
	BN_hex2bn(&PK1[1], "668c94526e0b8a25f2bc0ad63ef119b2091a02496a75001da1dc2420c08830f9");
	BN_hex2bn(&SK1, "83bb90c52a7aabc2cfc4eaf90bd32e19894454d1f0e8e526a936f47cfd2a1e91");
	BN_hex2bn(&sig1[0], "466fa9345f2a71c18acd335c4ab376b9ec4b93fa148f215c4051bce1d7f16911");
	BN_hex2bn(&sig1[1], "3440fee94d2c78c17eec665d285fade5d081a7666a5bd0448b41b5c02ba622d5");

	BN_hex2bn(&sig2[0], "91282674d71ee9a0395108230967805e5e65856f1d287b33e76717aa312d92a4");
	BN_hex2bn(&sig2[1], "ba1aca04ed6a8ea5b0c7112bb2b75f41af2ae08a3ade342e96ef020c2ad0494d");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void same_d_and_k() {
	BIGNUM* da = BN_new();
	BIGNUM* temp1 = BN_new();
	BIGNUM* temp2 = BN_new();
	BIGNUM* temp3 = BN_new();
	BIGNUM* temp4 = BN_new();
	BIGNUM* temp5 = BN_new();
	BIGNUM* temp6 = BN_new();
	BIGNUM* temp7 = BN_new();
	BIGNUM* e1 = BN_new();
	BN_hex2bn(&e1, "c2f2d91c2a31954e293fe038488e1a411a2fd90c0196c126c918852e3e80a7d9");

	BN_mul(temp1, sig1[1], sig2[1], BN_CTX_new());
	BN_mul(temp2, sig1[1], sig2[0], BN_CTX_new());
	BN_add(temp3, temp1, temp2);
	BN_mod_sub(temp4, sig1[0], temp3, N, BN_CTX_new());
	BN_hex2bn(&temp5, "1");
	temp4 = BN_mod_inverse(temp5, temp4, N, BN_CTX_new());

	BN_mod_mul(temp5, sig1[1], sig2[1], N, BN_CTX_new());
	BN_mod_sub(temp6, temp5, e1, N, BN_CTX_new());
	BN_mod_mul(temp7, temp6, temp4, N, BN_CTX_new());
	cout << BN_cmp(temp7, SK1);

}

int main(void) {
	init();
	same_d_and_k();
	return 0;
}
