# Project11: impl sm2 with RFC6979

## SM2算法流程

以SM2签名版本为例，SM2签名是基于椭圆曲线上的离散对数困难问题而构造的，主要分为预计算、密钥生成、签名与验签四大阶段。

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230711142140584.png" alt="image-20230711142140584" style="zoom:80%;" />

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230711142149277.png" alt="image-20230711142149277" style="zoom:80%;" />

## RFC6979标准

当SM2算法重复使用随机数k时，会导致私钥的泄露。

![image-20230711142957738](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230711142957738.png)

因此，为了克服这种安全性上的缺陷，RFC6962标准建议了随机数k的选取方法![](https://latex.codecogs.com/svg.image?k=SHA256(sk&plus;H(m)))，其中的函数H我们选取为SM3杂凑算法，这样，当每次加密不同消息时，产生的随机数k就会以很大的概率不同。

## 项目结果

采用的椭圆曲线群为SM2曲线。

```python
A = 0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
B = 0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
P = 0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
N = 0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7
G_X = 0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
G_Y = 0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
G = (G_X, G_Y)
```

按照上述所写的SM2流程进行完整的SM2签名算法，并修改了其中随机数k的选取方法，并且当随机数k大于模素数P时，重新生成公私钥，并以此来重新选取随机数k。

```python
    k = int(sha256((str(private_key) + sm3.sm3_hash(func.bytes_to_list(bytes(message, encoding='utf-8')))).encode()).hexdigest(),16)  # 伪随机k的生成_RFC6979
```

输入为用户所要验证的消息内容及用户相应的ID，输出为签名值及验证结果。

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230711143742913.png" alt="image-20230711143742913" style="zoom:80%;" />