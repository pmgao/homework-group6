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
#include <vector>

void sm3_openssl(const void* message, size_t len, uint8_t* hash){
    EVP_MD_CTX* md_ctx;
    const EVP_MD* md;

    md = EVP_sm3();
    md_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md_ctx, md, NULL);
    EVP_DigestUpdate(md_ctx, message, len);
    EVP_DigestFinal_ex(md_ctx, hash, NULL);
    EVP_MD_CTX_free(md_ctx);
}

void _generateKeys(EC_KEY*& eckey) {
    bool success(true);
    eckey = EC_KEY_new();
    if (!eckey)
    {
        std::cout << "Failed to create new EC Key..." << std::endl;
        success = false;
    }
    EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
    if (!ecgroup)
    {
        std::cout << "Failed to create new EC Group..." << std::endl;
        success = false;
    }
    if (EC_KEY_set_group(eckey, ecgroup) == 0)
    {
        std::cout << "Failed to set group for EC Key..." << std::endl;
        success = false;
    }
    if (EC_KEY_generate_key(eckey) == 0)
    {
        std::cout << "Failed to generate EC Key..." << std::endl;
        success = false;
    }
    EC_GROUP_free(ecgroup);
    if (!success)
    {
        EC_KEY_free(eckey);
        std::cout << " create ec key failed " << std::endl;
        return;
    }
}

bool ecdsaSign(const unsigned char* hash, EC_KEY* eckey, std::string& signMessage) {

    std::vector<unsigned char> vchSig;
    if (!eckey) return false;

    ECDSA_SIG* signature = ECDSA_do_sign(hash, 32, eckey);
    if (!signature)
    {
        std::cout << "Failed to generate EC Signature..." << std::endl;
        ECDSA_SIG_free(signature);
        return false;
    }

    unsigned int nSize = ECDSA_size(eckey);
    vchSig.resize(nSize);
    unsigned char* pos = &vchSig[0];
    nSize = i2d_ECDSA_SIG(signature, &pos);
    vchSig.resize(nSize);

    for (int i = 0; i < vchSig.size(); ++i)
    {
        signMessage += vchSig[i];
    }

    ECDSA_SIG_free(signature);

    return true;
}

bool ecdsaVerify(const unsigned char* hash, const std::string& signMessage, EC_KEY* eckey) {
    std::vector<unsigned char> vchSig;
    for (int i = 0; i < signMessage.size(); ++i)
    {
        vchSig.push_back(signMessage[i]);
    }

    if (vchSig.empty()) return false;

    unsigned char* norm_der = NULL;
    ECDSA_SIG* norm_sig = ECDSA_SIG_new();
    const unsigned char* sigptr = &vchSig[0];
    if (d2i_ECDSA_SIG(&norm_sig, &sigptr, vchSig.size()) == NULL)
    {
        ECDSA_SIG_free(norm_sig);
        return false;
    }

    int derlen = i2d_ECDSA_SIG(norm_sig, &norm_der);
    if (derlen <= 0) return false;

    if (ECDSA_do_verify(hash, 32, norm_sig, eckey) != 1)
    {
        std::cout << "Failed to verify EC Signature..." << std::endl;
        return false;
    }

    ECDSA_SIG_free(norm_sig);
    return true;
}


uint8_t* trusted_issuer(EC_KEY* eckey, std::string& signature) {
    unsigned char seed[16];
    RAND_bytes(seed, 16);

    static uint8_t s[32];
    uint8_t temp[32];
    sm3_openssl(seed, 16, s);

    for (int i = 0; i < 122; i++) {
        sm3_openssl(s, 32, temp);
        memcpy(s, temp, sizeof(uint8_t) * 32);
    }
    ecdsaSign(temp, eckey, signature);
    sm3_openssl(seed, 16, s);
    return s;
}

uint8_t* Alice(EC_KEY* eckey, std::string& signature, uint8_t* s) {
    static uint8_t temp[32];
    for (int i = 0; i < 22; i++) {
        sm3_openssl(s, 32, temp);
        memcpy(s, temp, sizeof(uint8_t) * 32);
    }

    return temp;
}

bool Bob(EC_KEY* eckey, std::string& signature, uint8_t* p) {
    uint8_t temp[32];
    for (int i = 0; i < 100; i++) {
        sm3_openssl(p, 32, temp);
        memcpy(p, temp, sizeof(uint8_t) * 32);
    }
    return ecdsaVerify(temp, signature, eckey);
}

int main(){
    EC_KEY* eckey = nullptr;
    _generateKeys(eckey);
    std::string signature;

    uint8_t* s = trusted_issuer(eckey, signature);
    uint8_t* p = Alice(eckey, signature, s);

    if (Bob(eckey, signature, p) == false)
    {
        std::cout << "verify fail" << std::endl;
        return -1;
    }

    std::cout << "success!" << std::endl;
    return 0;

}
