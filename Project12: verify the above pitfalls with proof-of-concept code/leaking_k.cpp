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
BIGNUM* k = BN_new();
BIGNUM* SK = BN_new();
BIGNUM* PK[2];
BIGNUM* sig1[2];
BIGNUM* N = BN_new();

void init() {
	PK[0] = BN_new();
	PK[1] = BN_new();
	sig1[0] = BN_new();
	sig1[1] = BN_new();


	BN_hex2bn(&k, "c1b286ef1bbe49174b9a4c3439e738e5b025eefe135a246d50f7652726f6ecd0");
	BN_hex2bn(&PK[0], "2b4a8cabec11ba85b0cbe50ddd5a36f9449a79fdee442e11333a49ef32c9e5cb");
	BN_hex2bn(&PK[1], "668c94526e0b8a25f2bc0ad63ef119b2091a02496a75001da1dc2420c08830f9");
	BN_hex2bn(&SK, "83bb90c52a7aabc2cfc4eaf90bd32e19894454d1f0e8e526a936f47cfd2a1e91");
	BN_hex2bn(&sig1[0], "466fa9345f2a71c18acd335c4ab376b9ec4b93fa148f215c4051bce1d7f16911");
	BN_hex2bn(&sig1[1], "3440fee94d2c78c17eec665d285fade5d081a7666a5bd0448b41b5c02ba622d5");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void leaking_k() {
	BIGNUM* da = BN_new();
	BIGNUM* temp1 = BN_new();
	BIGNUM* temp2 = BN_new();
	BIGNUM* temp3 = BN_new();

	BN_hex2bn(&temp3, "1");
	BN_mod_sub(temp2, k, sig1[1], N, BN_CTX_new());
	BN_mod_add(temp1, sig1[0], sig1[1], N, BN_CTX_new());
	temp1 = BN_mod_inverse(temp3, temp1, N, BN_CTX_new());
	BN_mod_mul(da, temp1, temp2, N, BN_CTX_new());
	cout << BN_cmp(da,SK);
}

int main(void) {
	init();
	//leaking random k
	leaking_k();
	return 0;
}
