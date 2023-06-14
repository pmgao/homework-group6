import random
from Crypto.Cipher import AES
from gmssl import sm2


def SM2_encrypt(plaintext):
    ciphertext = sm2_crypt.encrypt(plaintext)
    return ciphertext


def SM2_decrypt(ciphertext):
    plaintext = sm2_crypt.decrypt(ciphertext)
    return plaintext


def PGP_encrypt(message, key):
    mode = AES.MODE_OFB
    iv = b'0000000000000000'
    cryptor = AES.new(key.encode('utf-8'), mode, iv)

    length = 16
    count = len(message)
    add = length - (count % length) if count % length != 0 else 0

    message = message + ('\0' * add)
    ciphertext1 = cryptor.encrypt(message.encode('utf-8'))
    plaintext = key.encode('utf-8')
    ciphertext2 = SM2_encrypt(plaintext)

    print("Encrypted message using the session key k (AES symmetric encryption):", ciphertext1)
    print("Encrypted session key k using the SM2 public key:", ciphertext2)

    return ciphertext1, ciphertext2


def split_trailing_zeros(byte_string):
    for i in range(len(byte_string) - 1, -1, -1):
        if byte_string[i] != 0x00:
            return byte_string[:i + 1]
    return byte_string


def PGP_decrypt(ciphertext1, ciphertext2):
    mode = AES.MODE_OFB
    iv = b'0000000000000000'
    session_key = SM2_decrypt(ciphertext2)
    print("Decrypted session key using SM2 private key:", session_key.decode('utf-8'))

    cryptor = AES.new(session_key, mode, iv)
    plain_text = cryptor.decrypt(ciphertext1)
    print("Original message:", split_trailing_zeros(plain_text).decode('utf-8'))


def key_gen(a, p, n, Gx, Gy):
    d = random.randint(1, n - 1)
    Q = point_mul(d, [Gx, Gy], a, p)
    return d, Q


def point_mul(k, P, a, p):
    Q = None
    while k > 0:
        if k % 2 == 1:
            Q = point_add(Q, P, a, p)
        P = point_add(P, P, a, p)
        k = k // 2

    return Q


def point_add(P, Q, a, p):
    if P is None:
        return Q
    elif Q is None:
        return P

    if P[0] == Q[0] and P[1] == Q[1]:
        s = (3 * P[0] * P[0] + a) * mod_inverse(2 * P[1], p)
    else:
        s = (Q[1] - P[1]) * mod_inverse(Q[0] - P[0], p)

    x = (s * s - P[0] - Q[0]) % p
    y = (s * (P[0] - x) - P[1]) % p

    return [x, y]


def mod_inverse(a, m):
    if a == 0:
        return None

    lm, hm = 1, 0
    low, high = a % m, m

    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low

    return lm % m


if __name__ == '__main__':
    p = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
    a = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
    b = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
    n = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
    Gx = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
    Gy = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
    G = [Gx, Gy]

    [private_key, public_key] = key_gen(a, p, n, Gx, Gy)
    private_key_hex = hex(private_key)[2:]
    public_key_hex = hex(public_key[0])[2:] + hex(public_key[1])[2:]
    sm2_crypt = sm2.CryptSM2(public_key=public_key_hex, private_key=private_key_hex)

    message = "Shandong University"
    print("Message:", message)

    session_key = hex(random.randint(2 ** 127, 2 ** 128))[2:]
    print("Randomly generated symmetric encryption key:", session_key)

    result1, result2 = PGP_encrypt(message, session_key)
    PGP_decrypt(result1, result2)
