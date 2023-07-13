# Project13: Implement the above ECMH scheme

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230713183222975.png" alt="image-20230713183222975" style="zoom:80%;" />

ECMH相当于将某些消息的哈希值各自映射到椭圆曲线上的一个点，并使用椭圆曲线上各种运算将这些消息的合并起来，支持消息的插入与删除。

## 项目结果

在代码中采用了secpk256k1曲线。

```c++
# secp256k1
A = 0x0000000000000000000000000000000000000000000000000000000000000000
B = 0x0000000000000000000000000000000000000000000000000000000000000007
P = 0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f
N = 0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141
```

在将消息映射到椭圆曲线上的点时，我们利用了上图所述的哈希链来实现，首先不断对消息作哈希值，当哈希值关于椭圆曲线群大素数P的勒让德符号等于1时，便停止哈希链的计算，并使用当前哈希值计算出对应椭圆曲线上的纵坐标。代码实现中我们使用SM3杂凑算法来进行哈希运算。

```python
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
```

测试结果时，我们设定了两条消息初始值，并得到了关于这两种消息作各种组合所对应的椭圆曲线上的点。

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230713185628193.png" alt="image-20230713185628193"  />