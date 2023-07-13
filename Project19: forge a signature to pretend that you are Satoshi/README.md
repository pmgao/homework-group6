# Project19: forge a signature to pretend that you are Satoshi

本项目要求我们伪造ECDSA算法的签名，具体的过程如下图所示（来自于课程PPT）。



<img src=".\md_image\1.png" alt="image-20230713120247318"  />

## 项目结果

使用openssl时采用的曲线结构为secpk256k1曲线。

```c++
EC_GROUP* curve = EC_GROUP_new_by_curve_name(NID_secp256k1);
```

采用的公钥数据使用中本聪本人在比特币网络上所发布的第一笔交易，按照上图所述流程，分别生成随机数u与v。

```c++
    BN_hex2bn(&x, "678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb6");
    BN_hex2bn(&y, "49f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f");
    PK = create_ec_point(x, y);
```

```c++
    BN_rand(u, BN_num_bits(N), 0, 0);
    while (1) {
        BN_gcd(gcd, u, N, BN_CTX_new());
        if (BN_is_one(gcd)) {
            break;
        }
        BN_rand(u, BN_num_bits(N), 0, 0);
    }
    BN_rand(v, BN_num_bits(N), 0, 0);
    while (1) {
        BN_gcd(gcd, v, N, BN_CTX_new());
        if (BN_is_one(gcd)) {
            break;
        }
        BN_rand(v, BN_num_bits(N), 0, 0);
    }
```

之后计算得到R‘与r’。

```c++
    EC_POINT* R_ = EC_POINT_new(curve);
    EC_POINT_mul(curve, R_, u, PK, v, BN_CTX_new());

    BIGNUM* r_ = BN_new();
    EC_POINT_get_affine_coordinates_GFp(curve, R_, r_, nullptr, NULL);
```

此时，s'与e'便可计算得到，签名便被伪造成功。

```c++
    BIGNUM* s_ = BN_new();
    BN_mod_inverse(s_, v, N, BN_CTX_new());
    BN_mod_mul(s_, s_, r_, N, BN_CTX_new());

    BIGNUM* e_ = BN_new();
    BN_mod_inverse(e_, v, N, BN_CTX_new());
    BN_mod_mul(e_, e_, u, N, BN_CTX_new());
    BN_mod_mul(e_, e_, r_, N, BN_CTX_new());

    cout << "forged signature:(" << BN_bn2hex(r_) << ", " << BN_bn2hex(s_) << ")\n";
```

运行结果如下图所示：

<img src=".\md_image\2.png" alt="image-20230713121006179" style="zoom:80%;" />
