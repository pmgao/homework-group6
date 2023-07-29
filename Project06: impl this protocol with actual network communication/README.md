# Project6: impl this protocol with actual network communication

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230729211101573.png" alt="image-20230729211101573"  />

来源于课程PPT，本质上是利用哈希函数进行范围证明。以本图内容为例，在证明流程中存在着三方：Trusted Issuer, Alice与Bob，而Alice想要向Bob证明她的年龄大于21，但不能向Bob透露她的具体年龄(即零知识证明)。

具体流程如下：

1、Trusted Issuer随机选择随机数种子，对其作哈希，又因k=2100-1978=122，之后进一步计算其122重哈希值，对其作签名。最后，将随机数种子与签名传给Alice

2、Alice从Trusted得到随机种子，对其作哈希，又因2000=1978=22，Alice进一步计算其22重哈希，将多重哈希值与签名值传给Bob。

3、Bob得到Alice的多重哈希值之后，继续在其基础上计算100重哈希值，判断其最终数据是否可以通过签名。

上述流程本质上是将122重哈希值拆成两部分：22重+100重，分派给Alice与Bob分别完成，最后根据签名验证即可。

## 项目结果

在具体实现时，使用C++语言，依赖于Openssl第三方库，所采用的版本为1.1.1l。实例化时，采用的哈希函数为SM3算法，签名算法采用secpk256k1上的ECDSA签名算法。

```c++
EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp256k1);
```

Trusted Issuer方需要得到随机数种子，并计算122重哈希：

```c++
uint8_t* trusted_issuer(EC_KEY* eckey, std::string& signature) {
    unsigned char seed[16];
    RAND_bytes(seed, 16);

    static uint8_t s[32];
    uint8_t temp[32];
    sm3_openssl(seed, 16, s);

    for (int i = 0; i < 122; i++) {
        sm3_openssl(s, 32, temp);
        memcpy(s, temp, sizeof(uint8_t) * 32);
    }
    ecdsaSign(temp, eckey, signature);
    sm3_openssl(seed, 16, s);
    return s;
}
```

Alice方需要根据随机数种子计算22重哈希：

```c++
uint8_t* Alice(EC_KEY* eckey, std::string& signature, uint8_t* s) {
    static uint8_t temp[32];
    for (int i = 0; i < 22; i++) {
        sm3_openssl(s, 32, temp);
        memcpy(s, temp, sizeof(uint8_t) * 32);
    }

    return temp;
}
```

Bob方需要在Alice方22重哈希的结果之上继续计算100重哈希，并对最终数据进行签名的验证：

```c++
bool Bob(EC_KEY* eckey, std::string& signature, uint8_t* p) {
    uint8_t temp[32];
    for (int i = 0; i < 100; i++) {
        sm3_openssl(p, 32, temp);
        memcpy(p, temp, sizeof(uint8_t) * 32);
    }
    return ecdsaVerify(temp, signature, eckey);
}
```

在主函数中，捕获Bob最终的验签结果：

```c++
 if (Bob(eckey, signature, p) == false)
    {
        std::cout << "verify fail" << std::endl;
        return -1;
    }

    std::cout << "success!" << std::endl;
    return 0;
```

运行结果如下图所示：

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230729212310100.png" alt="image-20230729212310100"  />