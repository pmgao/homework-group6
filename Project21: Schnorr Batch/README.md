# Project21: Schnorr Batch

<img src=".\md_image\1.png" alt="image-20230715134318095"  />

图片来源于课程PPT，该项目主要实现的是Schnorr签名的批量验证功能，对于多条签名数据，可利用Schnorr签名的线性性质，通过上述PPT所讲流程进行一次性验证。

## 项目结果

​	具体实现中，椭圆曲线上的运算函数来自于Project11所写的代码，所采用的曲线结构为secpk256k1曲线，所签名消息为字符串“202100460055”，并设定参与签名的成员共有2个，所签消息内容均为“202100460055”，按照上图流程分别计算得到s，R，e与P的和。采用的公私钥数据为：

```python
msg = "202100460055"
SK_bob = 0xabb4a442f70621a2137a8b332b6cd653de30dbb4b47fc2044cef13488e66157b
SK_alice = 89652975980192045565381556847798492396888680198332589948144044069692575244768
PK_bob = elliptic_multiply(GPoint, SK_bob)
PK_alice = elliptic_multiply(GPoint, SK_alice)
```

关于签名分量s的和的计算：

```python
    s1 = (k1_bob + (H * SK_bob)) % N
    s2 = (k2_alice + (H * SK_alice)) % N

    s = (s1 + s2) % N
    print("s:", s)
```

关于签名分量R的和的计算：

```python
    R1 = elliptic_multiply(GPoint, k1_bob)
    R2 = elliptic_multiply(GPoint, k2_alice)

    R = elliptic_add(R1, R2)
    print("R: ", R)
```

关于P的和的计算：

```python
    P = elliptic_add(PK_bob, PK_alice)
    print("P:", P)
```


<img src=".\md_image\2.png" alt="image-20230715142534911"  />

此时的Schnorr便可一齐验证通过二者的签名数据，相当于对签名进行了聚合并只需作一次验证，此种批量验证的策略提高了签名的性能优势，向外界隐藏了此次交易是一笔多签交易还是非多签交易的隐私信息。
