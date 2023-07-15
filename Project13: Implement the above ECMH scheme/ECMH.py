from gmssl import sm3, func
from random import randint
from math import sqrt

# secp256k1
A = 0x0000000000000000000000000000000000000000000000000000000000000000
B = 0x0000000000000000000000000000000000000000000000000000000000000007
P = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
N = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141


def inv(a, n):  # Extended Euclidean Algorithm/'division' in elliptic curves
    lm, hm = 1, 0
    low, high = a % n, n
    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low
    return lm % n


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


def elliptic_inv(p):
    return [p[0], P - p[1]]


def elliptic_sub(p, q):
    return elliptic_add(p, elliptic_inv(q))


def ADD(ecmh, msg):
    return elliptic_add(ecmh, msg_to_dot(msg))


def EC_double(p):
    r = []
    slope = (3 * p[0] ** 2 + A) * inv(2 * p[1], P) % P
    r.append((slope ** 2 - 2 * p[0]) % P)
    r.append((slope * (p[0] - r[0]) - p[1]) % P)
    return (r[0], r[1])


def msg_to_dot(msg):
    def Legendre(y, p):
        return pow(y, (p - 1) // 2, p)

    def msg_to_x(m):
        mdigest = sm3.sm3_hash(func.bytes_to_list(bytes(m, encoding='utf-8')))
        while 1:  # cycle until x belong to QR
            x = int(mdigest, 16)
            if Legendre(x, P):
                break
            mdigest = sm3.sm3_hash(func.bytes_to_list(bytes(mdigest, encoding='utf-8')))
        return x

    def get_y(x):
        right = (x ** 3 + 7) % P
        while 1:
            a = randint(0, P)
            if Legendre(a, P) == P - 1:
                break
        base = int(a + sqrt(a ** 2 - right))
        expo = (P + 1) // 2
        y = pow(base, expo, P)
        return y

    x = msg_to_x(msg)
    y = get_y(x)
    return (x, y)


def single(msg):
    return ADD(0, msg)


def remove(ecmh, msg):
    return elliptic_sub(ecmh, msg_to_dot(msg))


def combine(msg_set):
    ans = single(msg_set[0])
    num = len(msg_set) - 1
    for i in range(num):
        ans = ADD(ans, msg_set[i + 1])
    return ans


if __name__ == "__main__":
    m1 = "202100460055"
    m2 = "1234567890"

    print('HASH(m1)\t\t\t', single(m1))
    print('HASH(m2)\t\t\t', single(m2))
    print('HASH([m1, m2])\t\t', combine([m1, m2]))
    print('HASH([m2, m1])\t\t', combine([m2, m1]))
    print('HASH(m1) + HASH(m2))', ADD(single(m1), m2))
    print('HASH([m1, m2]) + HASH(m2)', remove(combine([m1, m2]), m2))
