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
    def ext_gcd(a, b, arr):
        if b == 0:
            arr[0] = 1
            arr[1] = 0
            return a
        g = ext_gcd(b, a % b, arr)
        t = arr[0]
        arr[0] = arr[1]
        arr[1] = t - int(a / b) * arr[1]
        return g

    arr = [0, 1, ]
    gcd = ext_gcd(a, n, arr)
    if gcd == 1:
        return (arr[0] % n + n) % n
    else:
        return -1


def elliptic_add(p, q):
    if p == 0:
        return q
    elif q == 0:
        return p
    else:
        # Swap p and q if px > qx.
        if p[0] > q[0]:
            p, q = q, p
        slope = (q[1] - p[1]) * inv(q[0] - p[0], P) % P

        x = (slope ** 2 - p[0] - q[0]) % P
        y = (slope * (p[0] - x) - p[1]) % P

        return x, y


def elliptic_double(p):
    slope = (3 * p[0] ** 2 + A) * inv(2 * p[1], P) % P

    x = (slope ** 2 - 2 * p[0]) % P
    y = (slope * (p[0] - x) - p[1]) % P

    return x, y


def elliptic_multiply(s, p):
    r = (0, 0)  # 0 representing a point at infinity

    while s > 0:
        if s & 1:
            r = elliptic_add(r, p)
        p = elliptic_double(p)
        s >>= 1

    return r


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
    a = str(a)
    b = str(b)
    G_X = str(G_X)
    G_Y = str(G_Y)
    x_A = str(x_A)
    y_A = str(y_A)
    ENTL = str(get_bit_num(ID))

    joint = ENTL + ID + a + b + G_X + G_Y + x_A + y_A
    joint_b = bytes(joint, encoding='utf-8')

    digest = sm3.sm3_hash(func.bytes_to_list(joint_b))
    return int(digest, 16)


def generate_key():
    private_key = int(secrets.token_hex(32), 16)
    public_key = elliptic_multiply(private_key, G)
    return private_key, public_key


def sign(private_key, message, Z_A):
    _M = Z_A + message
    _M_b = bytes(_M, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(_M_b))  # str
    e = int(e, 16)
    k = int(sha256(
        (str(private_key) + sm3.sm3_hash(func.bytes_to_list(bytes(message, encoding='utf-8')))).encode()).hexdigest(),
            16)  # 伪随机k的生成_RFC6979
    if k >= P:
        return None
    random_point = elliptic_multiply(k, G)

    r = (e + random_point[0]) % N
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
        Z_A = precompute(ID, A, B, G_X, G_Y , pk[0], pk[1])
        signature = sign(sk, message, str(Z_A))

    print("sign: ", signature)
    if verify(pk, ID, message, signature) == 1:
        print('verified!')
