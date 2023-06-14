#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>

EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
EC_POINT* G = EC_POINT_new(group);
BIGNUM* xG = BN_new();
BIGNUM* yG = BN_new();
BIGNUM* order = BN_new();
unsigned char* pubkey = NULL;
int pubkey_len = 0;
EC_KEY* key = EC_KEY_new();
EC_POINT* P = EC_POINT_new(group);

void init() {
	BN_hex2bn(&xG, "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798");
	BN_hex2bn(&yG, "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8");
	EC_POINT_set_affine_coordinates_GFp(group, G, xG, yG, NULL);

	BN_hex2bn(&order, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
	const char* GENESIS_BLOCK_PUBKEY = "04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f";
	EC_KEY_set_group(key, group);
}

void forge() {
	EC_KEY_oct2key(key, pubkey, pubkey_len, NULL);
	EC_POINT_oct2point(group, P, pubkey, pubkey_len, NULL);

	BIGNUM* u = BN_new();
	BIGNUM* v = BN_new();
	BN_rand_range(u, order);
	BN_rand_range(v, order);

	EC_POINT* R_ = EC_POINT_new(group);
	EC_POINT* uG = EC_POINT_new(group);
	EC_POINT* vP = EC_POINT_new(group);
	EC_POINT_mul(group, uG, u, G, NULL, BN_CTX_new());
	EC_POINT_mul(group, vP, v, P, NULL, BN_CTX_new());
	EC_POINT_add(group, R_, uG, vP, NULL);

	BIGNUM* r_ = BN_new();
	BIGNUM* xR = BN_new();
	BIGNUM* yR = BN_new();
	EC_POINT_get_affine_coordinates_GFp(group, R_, xR, yR, NULL);
	BN_mod(r_, xR, order, BN_CTX_new());

	BIGNUM* e_ = BN_new();
	BIGNUM* u_inverse = BN_new();
	BN_mod_inverse(u_inverse, u, order, NULL);
	BIGNUM* r_u_v_inverse = BN_new();
	BN_mod_mul(r_u_v_inverse, r_, u_inverse, order, BN_CTX_new());
	BN_mod_mul(e_, r_u_v_inverse, v, order, BN_CTX_new());
	BIGNUM* s_ = BN_new();
	BN_mod_inverse(s_, v, order, NULL);

	char* r_str = BN_bn2hex(r_);
	char* s_str = BN_bn2hex(s_);

	printf("sigma = (%s, %s)\n", r_str, s_str);

	OPENSSL_free(r_str);
	OPENSSL_free(s_str);
}

int main() {
	init();
	forge();

	return 0;
}
