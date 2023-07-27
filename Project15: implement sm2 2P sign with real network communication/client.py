# client
from sm2 import *
import random
from gmssl import sm3, func
import socket


def Gen_Key():
    d1 = random.randint(1, N - 1)
    P1 = elliptic_multiply(inv(d1, N), G)
    return d1, P1


def Gen_Q1_e(Z, M):
    M_ = bytes(Z + M, encoding='utf-8')
    e = sm3.sm3_hash(func.bytes_to_list(M_))
    k1 = random.randint(1, N - 1)
    Q1 = elliptic_multiply(k1, G)
    return k1, Q1, e


def Sign(d1, k1, r, s2, s3):
    s = ((d1 * k1) * s2 + d1 * s3 - r) % N
    if s != 0 or s != N - r:
        return (r, s)
    else:
        return 'error'


if __name__ == "__main__":
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    print("connected...")

    d1, P1 = Gen_Key()
    data = str(P1[0]) + '|' + str(P1[1])
    s.sendto(data.encode(), ("127.0.0.1", 12300))

    ID_client = 'client'
    ID_server = 'server'
    Z = ID_client + ID_server
    M = input("input your massage:")
    k1, Q1, e = Gen_Q1_e(Z, M)
    data = str(Q1[0]) + '|' + str(Q1[1]) + '||' + e
    s.sendto(data.encode(), ("127.0.0.1", 12300))

    data, addr = s.recvfrom(1024)
    data = data.decode()
    flag1 = data.index('|')
    flag2 = data.index('||')
    r = int(data[:flag1])
    s2 = int(data[flag1 + 1:flag2])
    s3 = int(data[flag2 + 2:])
    s_ = Sign(d1, k1, r, s2, s3)
    print("Sign:", s_)

    s.close()
    print("client finished")
