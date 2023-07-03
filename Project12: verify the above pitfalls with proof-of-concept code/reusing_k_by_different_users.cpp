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

	BN_hex2bn(&PK2[0], "e397d6c4054aa8823889eea700b82b72144dde5f2e3916157295d171114b415e");
	BN_hex2bn(&PK2[1], "1457163f00ee73f8f0f452c59a62c1be593a5cd311702a03d983c84c51c347c");
	BN_hex2bn(&SK2, "fc07e720c13f31fc02a783e790152d6f6dc30beb41fadff72e5af59668e00038");
	BN_hex2bn(&sig2[0], "100befb6ea7136100d89d54c6bcce4239398086a19f179be530cbefb9ef1454");
	BN_hex2bn(&sig2[1], "28152fa35e1b67c4bf88462c10bc06f394cb255d87f2a7e8028113b8638ead2b");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

void reusing_k_by_different_users() {
	BIGNUM* da = BN_new();
	BIGNUM* temp1 = BN_new();
	BIGNUM* temp2 = BN_new();
	BIGNUM* temp3 = BN_new();

	BN_hex2bn(&temp3, "1");
	BN_mod_sub(temp2, k, sig1[1], N, BN_CTX_new());
	BN_mod_add(temp1, sig1[0], sig1[1], N, BN_CTX_new());
	temp1 = BN_mod_inverse(temp3, temp1, N, BN_CTX_new());
	BN_mod_mul(da, temp1, temp2, N, BN_CTX_new());
	cout << BN_cmp(da, SK1) << endl;

	BIGNUM* db = BN_new();
	BIGNUM* temp1_ = BN_new();
	BIGNUM* temp2_ = BN_new();
	BIGNUM* temp3_ = BN_new();
	BN_hex2bn(&temp3_, "1");
	BN_mod_sub(temp2_, k, sig2[1], N, BN_CTX_new());
	BN_mod_add(temp1_, sig2[0], sig2[1], N, BN_CTX_new());
	temp1_ = BN_mod_inverse(temp3_, temp1_, N, BN_CTX_new());
	BN_mod_mul(db, temp1_, temp2_, N, BN_CTX_new());
	cout << BN_cmp(db, SK2) << endl;
}

int main(void) {
	init();
	reusing_k_by_different_users();
	return 0;
}
