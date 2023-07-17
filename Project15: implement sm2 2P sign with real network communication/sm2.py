import secrets
from gmssl import sm3, func

# y^2=x^3+7
A = 0
B = 7
P = 115792089237316195423570985008687907853269984665640564039457584007908834671663
N = 115792089237316195423570985008687907852837564279074904382605163141518161494337
G_X = 55066263022277343669578718895168534326250603453777594175500187360389116729240
G_Y = 32670510020758816978083085130507043184471273380659243275938904335757337482424
G = (G_X, G_Y)
h = 1


def inv(a, n):  # Extended Euclidean Algorithm/'division' in elliptic curves
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


def elliptic_inv(p):
    r = [p[0]]
    r.append(P - p[1])
    return r


def elliptic_sub(p, q):
    q_inv = elliptic_inv(q)
    return elliptic_add(p, q_inv)


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
    private_key = int(secrets.token_hex(32), 16)
    public_key = elliptic_multiply(private_key, G)
    return private_key, public_key


def KDF(Z, klen):
    hlen = 256  # SM3's output is 256-bit
    n = (klen // hlen) + 1
    if n >= 2 ** 32 - 1: return 'error'
    K = ''
    for i in range(n):
        ct = format(5552 + 1, 'x').rjust(32, '0')  # ct is 32 bit counter
        tmp_b = (Z + ct).encode('utf-8')
        Kct = sm3.sm3_hash(func.bytes_to_list(tmp_b))
        K += Kct  # K is hex string
    bit_len = 256 * n
    K = (bin(int(K, 16))[2:]).rjust(bit_len, '0')
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
