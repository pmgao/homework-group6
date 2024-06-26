import hashlib

# secp256k1
N = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
Pcurve = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
Acurve = 0x0000000000000000000000000000000000000000000000000000000000000000
Bcurve = 0x0000000000000000000000000000000000000000000000000000000000000007
Gx = 0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798
Gy = 0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8
GPoint = [Gx, Gy]

SK_bob = 0xabb4a442f70621a2137a8b332b6cd653de30dbb4b47fc2044cef13488e66157b
SK_alice = 89652975980192045565381556847798492396888680198332589948144044069692575244768


def modinv(a, n=Pcurve):
    lm, hm = 1, 0
    low, high = a % n, n
    while low > 1:
        ratio = high // low
        nm, new = hm - lm * ratio, high - low * ratio
        lm, low, hm, high = nm, new, lm, low
    return lm % n


def elliptic_add(a, b):
    LamAdd = ((b[1] - a[1]) * modinv(b[0] - a[0], Pcurve)) % Pcurve
    x = (LamAdd * LamAdd - a[0] - b[0]) % Pcurve
    y = (LamAdd * (a[0] - x) - a[1]) % Pcurve
    return (x, y)


def elliptic_double(a):
    Lam = ((3 * a[0] * a[0] + Acurve) * modinv((2 * a[1]), Pcurve)) % Pcurve
    x = (Lam * Lam - 2 * a[0]) % Pcurve
    y = (Lam * (a[0] - x) - a[1]) % Pcurve
    return (x, y)


def elliptic_multiply(GenPoint, ScalarHex):
    if ScalarHex == 0 or ScalarHex >= N: raise Exception("Invalid Scalar/Private Key")
    ScalarBin = str(bin(ScalarHex))[2:]
    Q = GenPoint
    for i in range(1, len(ScalarBin)):
        Q = elliptic_double(Q)
        if ScalarBin[i] == "1":
            Q = elliptic_add(Q, GenPoint)
    return (Q)


if __name__ == "__main__":
    msg = "202100460055"

    print("Message: ", msg)
    msg = msg.encode()

    PK_bob = elliptic_multiply(GPoint, SK_bob)
    PK_alice = elliptic_multiply(GPoint, SK_alice)

    print("Public Key Bob:")
    print("X: " + str(PK_bob[0]))
    print("Y: " + str(PK_bob[1]))

    print("Public Key Alice:")
    print("X: " + str(PK_alice[0]))
    print("Y: " + str(PK_alice[1]))
    print()

    P = elliptic_add(PK_bob, PK_alice)
    print("P:", P)

    k1_bob = 0xabb4a442f70621a2137a8b332b6cd653de30dbb4b47fc2044cef13488e66157b - 1
    k2_alice = 0xc635c94354e3ba92c643e82fbdc325d5dea1a5e9b95251befe09f7e7410ee1e0 - 1

    R1 = elliptic_multiply(GPoint, k1_bob)
    R2 = elliptic_multiply(GPoint, k2_alice)

    R = elliptic_add(R1, R2)
    print("R: ", R)

    P_bytes = (P[0]).to_bytes(32, 'big')
    R_bytes = (R[0]).to_bytes(32, 'big')

    hasher = hashlib.sha256()
    hasher.update(P_bytes + R_bytes + msg)
    H = int.from_bytes(hasher.digest(), 'big')
    print("H:", H)

    s1 = (k1_bob + (H * SK_bob)) % N
    s2 = (k2_alice + (H * SK_alice)) % N

    s = (s1 + s2) % N
    print("s:", s)

    v1 = elliptic_multiply(GPoint, s)

    inter = elliptic_multiply(P, H)

    v2 = elliptic_add(R, inter)

    if v1 == v2:
        print("Verified!")
