import secrets
from gmssl import sm3, func

A = 0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
B = 0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
P = 0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
N = 0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7
G_X = 0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
G_Y = 0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
G = (G_X, G_Y)
h = 1


def inv(a, n):
    lm, hm = 1, 0
    low, high = a % n, n
    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low
    return lm % n


def elliptic_add(a, b):
    if a == 0:
        return b
    elif b == 0:
        return a
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


def get_bit_num(x):
    if isinstance(x, int):
        num = 0
        while x:
            num += 1
            x >>= 1
        return num
    elif isinstance(x, str):
        return len(x.encode()) * 8
    elif isinstance(x, bytes):
        return len(x) * 8
    return 0


def generate_key():
    sk = int(secrets.token_hex(32), 16) % N
    pk = elliptic_multiply(sk, G)
    return sk, pk


def KDF(Z, klen):
    n = (klen // 256) + 1
    if n >= 2 ** 32 - 1: return 'error'
    K = ''
    for i in range(n):
        ct = format(5552 + 1, 'x').rjust(32, '0')  # ct is 32 bit counter
        tmp_b = (Z + ct).encode('utf-8')
        Kct = sm3.sm3_hash(func.bytes_to_list(tmp_b))
        K += Kct  # K is hex string
    K = (bin(int(K, 16))[2:]).rjust(256 * n, '0')
    K = K[:klen]  # MSB(K, klen)
    return K


def enc_XOR(m, t):
    m = m.encode('utf-8')
    n = len(m)
    ans = [format(m[i] ^ int(t[8 * i:8 * (i + 1)], 2), '02x') for i in range(n)]
    A = ''.join(ans)
    return A


def dec_XOR(C2, t):
    n = len(C2) // 2
    ans = [chr(int(C2[2 * i:2 * (i + 1)], 16) ^ int(t[8 * i:8 * (i + 1)], 2)) for i in range(n)]
    A = ''.join(ans)
    return A


def SM2_enc(M, pk):
    if pk == 0: return 'error:public key'
    while 1:
        k = secrets.randbelow(N)
        C1 = elliptic_multiply(k, G)
        dot = elliptic_multiply(k, pk)
        klen = get_bit_num(M)
        x2 = hex(dot[0])[2:]
        y2 = hex(dot[1])[2:]
        t = KDF(x2 + y2, klen)
        if (t != '0' * klen):
            break
    C2 = enc_XOR(M, t)
    tmp_b = bytes((x2 + M + y2), encoding='utf-8')
    C3 = sm3.sm3_hash(func.bytes_to_list(tmp_b))
    return (C1, C2, C3)


def SM2_dec(C, sk):
    C1, C2, C3 = C
    x = C1[0]
    y = C1[1]
    left = y * y % P
    right = (pow(x, 3, P) + A * x + B) % P
    if (left != right): return """error:C1 can't satisfy EC equation"""
    if C1 == 0: return 'S = hC1 =0 error'
    dot = elliptic_multiply(sk, C1)
    klen = len(C2) * 4
    x2 = hex(dot[0])[2:]
    y2 = hex(dot[1])[2:]
    t = KDF(x2 + y2, klen)
    if t == '0' * klen: return """error: t is all '0'  """
    M = dec_XOR(C2, t)
    tmp_b = bytes((x2 + M + y2), encoding='utf-8')
    u = sm3.sm3_hash(func.bytes_to_list(tmp_b))
    if u != C3: return 'error:u != C3'
    return M


if __name__ == '__main__':
    m = input('input your message:')
    sk, pk = generate_key()
    cipher = SM2_enc(m, pk)
    print('encrypted:', cipher)
    plain = SM2_dec(cipher, sk)
    print('decrypted:', plain)
