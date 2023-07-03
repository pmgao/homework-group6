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


def extended_euclidean_algorithm(j, k):
    if j == k:
        return (j, 1, 0)
    else:
        i = 0
        j_array = [j]
        k_array = [k]
        q_array = []
        r_array = []

        prev_r_is_zero = False

        while not (prev_r_is_zero):
            q_array.append(k_array[i] // j_array[i])
            r_array.append(k_array[i] % j_array[i])
            k_array.append(j_array[i])
            j_array.append(r_array[i])
            i += 1
            if r_array[i - 1] == 0:
                prev_r_is_zero = True
        i -= 1
        gcd = j_array[i]
        x_array = [1]
        y_array = [0]

        i -= 1
        total_steps = i

        while i >= 0:
            y_array.append(x_array[total_steps - i])
            x_array.append(y_array[total_steps - i] - q_array[i] * x_array[total_steps - i])
            i -= 1

        return (gcd, x_array[-1], y_array[-1])


def mod_inverse(j, n):
    (gcd, x, y) = extended_euclidean_algorithm(j, n)

    if gcd == 1:
        return x % n
    else:
        return -1


def elliptic_add(p, q):
    if p == 0 and q == 0:
        return 0
    elif p == 0:
        return q
    elif q == 0:
        return p
    else:
        # Swap p and q if px > qx.
        if p[0] > q[0]:
            temp = p
            p = q
            q = temp
        r = []
        slope = (q[1] - p[1]) * mod_inverse(q[0] - p[0], P) % P

        r.append((slope ** 2 - p[0] - q[0]) % P)
        r.append((slope * (p[0] - r[0]) - p[1]) % P)

        return r[0], r[1]


def elliptic_inv(p):
    r = [p[0]]
    r.append(P - p[1])
    return r


def elliptic_sub(p, q):
    q_inv = elliptic_inv(q)
    return elliptic_add(p, q_inv)


def elliptic_double(p):
    r = []

    slope = (3 * p[0] ** 2 + A) * mod_inverse(2 * p[1], P) % P

    r.append((slope ** 2 - 2 * p[0]) % P)
    r.append((slope * (p[0] - r[0]) - p[1]) % P)

    return r[0], r[1]


def elliptic_multiply(s, p):
    n = p
    r = 0  # 0 representing a point at infinity

    s_binary = bin(s)[2:]  # convert s to binary and remove the "0b" in the beginning
    s_length = len(s_binary)

    for i in reversed(range(s_length)):
        if s_binary[i] == '1':
            r = elliptic_add(r, n)
        n = elliptic_double(n)

    return r


def get_bit_num(x):
    if isinstance(x, int):
        num = 0
        tmp = x >> 64
        while tmp:
            num += 64
            tmp >>= 64
        tmp = x >> num >> 8
        while tmp:
            num += 8
            tmp >>= 8
        x >>= num
        while x:
            num += 1
            x >>= 1
        return num
    elif isinstance(x, str):
        return len(x.encode()) << 3
    elif isinstance(x, bytes):
        return len(x) << 3
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
        ct = (hex(5552 + 1)[2:]).rjust(32, '0')  # ct is 32 bit counter
        tmp_b = bytes((Z + ct), encoding='utf-8')
        Kct = sm3.sm3_hash(func.bytes_to_list(tmp_b))
        K += Kct  # K is hex string
    bit_len = 256 * n
    K = (bin(int(K, 16))[2:]).rjust(bit_len, '0')
    K = K[:klen]  # MSB(K, klen)
    return K


def enc_XOR(m, t):
    m = bytes(m, encoding='utf-8')
    m = func.bytes_to_list(m)  # each element -> 8-bit
    n = len(m)  # n bytes
    ans = []
    for i in range(n):
        mm = m[i]
        tt = int(t[8 * i:8 * (i + 1)], 2)
        a = (hex(mm ^ tt)[2:]).rjust(2, '0')
        ans.append(a)
    A = ''.join(ans)
    # length of A is klen/4
    return A


def dec_XOR(C2, t):
    n = len(C2) // 2
    ans = []
    for i in range(n):
        c2c2 = int(C2[2 * i:2 * (i + 1)], 16)  # -> int
        tt = int(t[8 * i:8 * (i + 1)], 2)
        ans.append(chr(c2c2 ^ tt))
    A = ''.join(ans)
    return A


def SM2_enc(M, pk):
    if pk == 0: return 'error:public key'
    while 1:
        k = secrets.randbelow(N)
        C1 = elliptic_multiply(k, G)  # C1 = kG = (x1, y1)
        dot = elliptic_multiply(k, pk)  # kpk = (x2, y2)
        klen = get_bit_num(M)
        x2 = hex(dot[0])[2:]
        y2 = hex(dot[1])[2:]
        t = KDF(x2 + y2, klen)
        if (t != '0' * klen):  # all '0' is invallid
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
