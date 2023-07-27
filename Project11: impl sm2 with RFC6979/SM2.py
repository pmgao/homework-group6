import secrets
from hashlib import sha256
from gmssl import sm3, func

A = 0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
B = 0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
P = 0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
N = 0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7
G_X = 0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
G_Y = 0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
G = (G_X, G_Y)


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


def precompute(ID, a, b, G_X, G_Y, x_A, y_A):
    joint = str(get_bit_num(ID)) + ID + str(a) + str(b) + str(G_X) + str(G_Y) + str(x_A) + str(y_A)
    joint_b = bytes(joint, encoding='utf-8')

    digest = sm3.sm3_hash(func.bytes_to_list(joint_b))
    return int(digest, 16)


def generate_key():
    private_key = int(secrets.token_hex(32), 16) % N
    public_key = elliptic_multiply(private_key, G)
    return private_key, public_key


def sign(private_key, message, Z_A):
    _M_b = bytes(Z_A + message, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(_M_b))  # str
    e = int(e, 16)
    k = int(sha256(
        (str(private_key) + sm3.sm3_hash(func.bytes_to_list(bytes(message, encoding='utf-8')))).encode()).hexdigest(),
            16)  # 伪随机k的生成_RFC6979
    if k >= P:
        return None
    rp = elliptic_multiply(k, G)

    r = (e + rp[0]) % N
    s = (inv(1 + private_key, N) * (k - r * private_key)) % N
    return (r, s)


def verify(public_key, ID, message, signature):
    r = signature[0]
    s = signature[1]

    Z = precompute(ID, A, B, G_X, G_Y, public_key[0], public_key[1])

    _M = str(Z) + message
    _M_b = bytes(_M, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(_M_b))  # str
    e = int(e, 16)
    t = (r + s) % N

    point = elliptic_multiply(s, G)
    point1 = elliptic_multiply(t, public_key)
    point = elliptic_add(point, point1)

    x1 = point[0]
    R = (e + x1) % N

    return R == r


if __name__ == "__main__":
    sk, pk = generate_key()
    print('pk：', pk)
    message = input("Input your message: ")
    ID = input("Input your ID: ")

    Z_A = precompute(ID, A, B, G_X, G_Y, pk[0], pk[1])
    signature = sign(sk, message, str(Z_A))
    while signature is None:
        sk, pk = generate_key()
        print('new pk：', pk)
        Z_A = precompute(ID, A, B, G_X, G_Y, pk[0], pk[1])
        signature = sign(sk, message, str(Z_A))

    print("sign: ", signature)
    if verify(pk, ID, message, signature) == 1:
        print('verified!')
