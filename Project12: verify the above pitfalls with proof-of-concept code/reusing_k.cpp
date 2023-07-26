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
BIGNUM* SK1 = BN_new();
BIGNUM* PK1[2];
BIGNUM* sig1[2];
BIGNUM* SK2 = BN_new();
BIGNUM* PK2[2];
BIGNUM* sig2[2];
BIGNUM* N = BN_new();

void init() {
	PK1[0] = BN_new();
	PK1[1] = BN_new();
	sig1[0] = BN_new();
	sig1[1] = BN_new();

	PK1[0] = BN_new();
	PK1[1] = BN_new();
	sig1[0] = BN_new();
	sig1[1] = BN_new();


	BN_hex2bn(&k, "c1b286ef1bbe49174b9a4c3439e738e5b025eefe135a246d50f7652726f6ecd0");
	BN_hex2bn(&PK1[0], "2b4a8cabec11ba85b0cbe50ddd5a36f9449a79fdee442e11333a49ef32c9e5cb");
	BN_hex2bn(&PK1[1], "668c94526e0b8a25f2bc0ad63ef119b2091a02496a75001da1dc2420c08830f9");
	BN_hex2bn(&SK1, "83bb90c52a7aabc2cfc4eaf90bd32e19894454d1f0e8e526a936f47cfd2a1e91");
	BN_hex2bn(&sig1[0], "466fa9345f2a71c18acd335c4ab376b9ec4b93fa148f215c4051bce1d7f16911");
	BN_hex2bn(&sig1[1], "3440fee94d2c78c17eec665d285fade5d081a7666a5bd0448b41b5c02ba622d5");

	BN_hex2bn(&sig2[0], "ebf4f3be6fafda912406626933b65d4ca08dea95ff89279c7efc206f3b6b9990");
	BN_hex2bn(&sig2[1], "854e1986b8068dba2191fba22848c8074cac4c374ffa4a74ab811190b57c1815");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void reusing_k() {
	BIGNUM* da = BN_new();
	BIGNUM* temp1 = BN_new();
	BIGNUM* temp2 = BN_new();
	BIGNUM* temp3 = BN_new();
	BIGNUM* temp4 = BN_new();
	BIGNUM* temp5 = BN_new();
	BIGNUM* temp6 = BN_new();

	BN_hex2bn(&temp5, "1");

	BN_mod_sub(temp1, sig2[1], sig1[1], N, BN_CTX_new());
	BN_mod_sub(temp2, sig1[1], sig2[1], N, BN_CTX_new());
	BN_mod_sub(temp3, sig1[0], sig2[0], N, BN_CTX_new());
	BN_mod_add(temp4, temp2, temp3, N, BN_CTX_new());
	temp6 = BN_mod_inverse(temp5, temp4, N, BN_CTX_new());
	BN_mod_mul(da, temp6, temp1, N, BN_CTX_new());
	cout << BN_cmp(da, SK1) << endl;

}

int main(void) {
	init();
	// reusing k
	reusing_k();
	return 0;
}
