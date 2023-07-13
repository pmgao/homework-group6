#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <iostream>

using namespace std;

const BIGNUM* N;
const EC_POINT* G;
EC_GROUP* curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
EC_POINT* PK = EC_POINT_new(curve);
BIGNUM* SK = BN_new();


void init() {
	N = EC_GROUP_get0_order(curve);
	G = EC_GROUP_get0_generator(curve);
	//BN_hex2bn(&SK, "83bb90c52a7aabc2cfc4eaf90bd32e19894454d1f0e8e526a936f47cfd2a1e91");
	//EC_POINT_mul(curve, PK, SK, nullptr, nullptr, BN_CTX_new());
	BN_hex2bn(&PK, "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f");
}

bool verify(BIGNUM* e_,BIGNUM* r_,BIGNUM* s_) {
    BIGNUM* w = BN_new();
    BN_mod_inverse(w, s_, N, BN_CTX_new());

    BIGNUM* temp1 = BN_new(), * temp2 = BN_new();
    BN_mul(temp1, e_, w, BN_CTX_new());
    BN_mul(temp2, r_, w, BN_CTX_new());

    EC_POINT* temp3 = EC_POINT_new(curve);
    EC_POINT_mul(curve, temp3, temp1, PK, temp2, BN_CTX_new());

    BIGNUM* x = BN_new();
    EC_POINT_get_affine_coordinates_GFp(curve, temp3, x, nullptr, NULL);
    return BN_cmp(x, r_);
}

void forge() {
    BIGNUM* u = BN_new();
    BIGNUM* v = BN_new();
    BIGNUM* gcd = BN_new();

    BN_rand(u, BN_num_bits(N), 0, 0);
    while (1) {
        BN_gcd(gcd, u, N, BN_CTX_new());
        if (BN_is_one(gcd)) {
            break;
        }
        BN_rand(u, BN_num_bits(N), 0, 0);
    }
    BN_rand(v, BN_num_bits(N), 0, 0);
    while (1) {
        BN_gcd(gcd, v, N, BN_CTX_new());
        if (BN_is_one(gcd)) {
            break;
        }
        BN_rand(v, BN_num_bits(N), 0, 0);
    }

    EC_POINT* R_ = EC_POINT_new(curve);
    EC_POINT_mul(curve, R_, u, PK, v, BN_CTX_new());

    BIGNUM* r_ = BN_new();
    EC_POINT_get_affine_coordinates_GFp(curve, R_, r_, nullptr, NULL);

    BIGNUM* s_ = BN_new();
    BN_mod_inverse(s_, v, N, BN_CTX_new());
    BN_mod_mul(s_, s_, r_, N, BN_CTX_new());

    BIGNUM* e_ = BN_new();
    BN_mod_inverse(e_, v, N, BN_CTX_new());
    BN_mod_mul(e_, e_, u, N, BN_CTX_new());
    BN_mod_mul(e_, e_, r_, N, BN_CTX_new());

    cout << "forged signature:(" << BN_bn2hex(r_) << ", " << BN_bn2hex(s_) << ")\n";
    if (!verify(e_, r_, s_)) {
        cout << "verified success!\n";
    }
    else {
        cout << "verified failed!\n";
    }
}

int main() {
	init();
	forge();

	return 0;
}
