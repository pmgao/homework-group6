import random
from Crypto.Cipher import AES
from gmssl import sm2

P = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF
A = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFC
B = 0x28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93
N = 0xFFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123
Gx = 0x32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7
Gy = 0xBC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0
G = [Gx, Gy]


def PGP_encrypt(message, key):
    cryptor = AES.new(key.encode('utf-8'), AES.MODE_ECB)

    length = 16
    count = len(message)
    add = length - (count % length) if count % length != 0 else 0

    padded_message = message + ('\0' * add)
    ciphertext1 = cryptor.encrypt(padded_message.encode('utf-8'))
    key_encode = key.encode('utf-8')
    ciphertext2 = sm2_crypt.encrypt(key_encode)

    print("Encrypted message using the session key k (AES):", ciphertext1)
    print("Encrypted session key k using the SM2 public key:", ciphertext2)

    return ciphertext1, ciphertext2


def PGP_decrypt(ciphertext1, ciphertext2):
    session_key = sm2_crypt.decrypt(ciphertext2)
    print("Decrypted session key using SM2 private key:", session_key.decode('utf-8'))

    cryptor = AES.new(session_key, AES.MODE_ECB)
    plain_text = cryptor.decrypt(ciphertext1)
    print("Decrypted original message using session key:", plain_text.rstrip(b'\x00').decode('utf-8'))


def key_gen():
    d = random.randint(2, N - 1)
    Q = elliptic_multiply(d, [Gx, Gy])
    return d, Q


def inv(a, n):
    lm, hm = 1, 0
    low, high = a % n, n
    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low
    return lm % n


def elliptic_add(a, b):
    LamAdd = ((b[1] - a[1]) * inv(b[0] - a[0], P)) % P
    x = (LamAdd * LamAdd - a[0] - b[0]) % P
    y = (LamAdd * (a[0] - x) - a[1]) % P
    return (x, y)


def elliptic_double(a):
    Lam = ((3 * a[0] * a[0] + A) * inv((2 * a[1]), P)) % P
    x = (Lam * Lam - 2 * a[0]) % P
    y = (Lam * (a[0] - x) - a[1]) % P
    return (x, y)


def elliptic_multiply(ScalarHex, GenPoint):
    if ScalarHex == 0 or ScalarHex >= N: raise Exception("Invalid Scalar/Private Key")
    ScalarBin = str(bin(ScalarHex))[2:]
    Q = GenPoint
    for i in range(1, len(ScalarBin)):
        Q = elliptic_double(Q)
        if ScalarBin[i] == "1":
            Q = elliptic_add(Q, GenPoint)
    return (Q)


if __name__ == '__main__':
    [private_key, public_key] = key_gen()
    private_key_hex = hex(private_key)[2:]
    public_key_hex = hex(public_key[0])[2:] + hex(public_key[1])[2:]
    sm2_crypt = sm2.CryptSM2(public_key=public_key_hex, private_key=private_key_hex)

    message = "Shandong University 202100460055"
    print("Message:", message)

    session_key = hex(random.randint(2 ** 127, 2 ** 128))[2:]
    print("Randomly generated symmetric encryption key:", session_key)

    result1, result2 = PGP_encrypt(message, session_key)
    PGP_decrypt(result1, result2)
