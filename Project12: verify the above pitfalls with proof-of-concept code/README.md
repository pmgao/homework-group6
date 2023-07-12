# Project12: verify the above pitfalls with proof-of-concept code

老师在PPT中具体介绍了4种签名算法所带来的安全性漏洞：

1、SM2签名时泄露了随机数k

2、SM2签名时重复使用了随机数k

3、SM2签名时不同的用户重复使用了相同的随机数k

4、SM2签名与ECDSA签名时使用了相同的私钥与随机数k

在代码实现中我们分别生成了这4种安全性漏洞所需的签名数据。还因在验证第4种安全性漏洞时要求SM2签名与ECDSA签名所采用的椭圆曲线参数相同，在代码中均选用了secpk256k1曲线![](https://latex.codecogs.com/svg.image?y^2=x^3&plus;7)。

具体签名数据如下所示：

```c++
//SM2
        BN_hex2bn(&k, "c1b286ef1bbe49174b9a4c3439e738e5b025eefe135a246d50f7652726f6ecd0");
	BN_hex2bn(&PK1[0], "2b4a8cabec11ba85b0cbe50ddd5a36f9449a79fdee442e11333a49ef32c9e5cb");
	BN_hex2bn(&PK1[1], "668c94526e0b8a25f2bc0ad63ef119b2091a02496a75001da1dc2420c08830f9");
	BN_hex2bn(&SK1, "83bb90c52a7aabc2cfc4eaf90bd32e19894454d1f0e8e526a936f47cfd2a1e91");
	BN_hex2bn(&sig1[0], "466fa9345f2a71c18acd335c4ab376b9ec4b93fa148f215c4051bce1d7f16911");
	BN_hex2bn(&sig1[1], "3440fee94d2c78c17eec665d285fade5d081a7666a5bd0448b41b5c02ba622d5");

	BN_hex2bn(&PK2[0], "e397d6c4054aa8823889eea700b82b72144dde5f2e3916157295d171114b415e");
	BN_hex2bn(&PK2[1], "1457163f00ee73f8f0f452c59a62c1be593a5cd311702a03d983c84c51c347c");
	BN_hex2bn(&SK2, "fc07e720c13f31fc02a783e790152d6f6dc30beb41fadff72e5af59668e00038");
	BN_hex2bn(&sig2[0], "100befb6ea7136100d89d54c6bcce4239398086a19f179be530cbefb9ef1454");
	BN_hex2bn(&sig2[1], "28152fa35e1b67c4bf88462c10bc06f394cb255d87f2a7e8028113b8638ead2b");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
//ECDSA
        BN_hex2bn(&sig2[0], "91282674d71ee9a0395108230967805e5e65856f1d287b33e76717aa312d92a4"); 
	BN_hex2bn(&sig2[1], "ba1aca04ed6a8ea5b0c7112bb2b75f41af2ae08a3ade342e96ef020c2ad0494d");
	BN_hex2bn(&N, "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
//message HASH for ECDSA
	BN_hex2bn(&e1, "c2f2d91c2a31954e293fe038488e1a411a2fd90c0196c126c918852e3e80a7d9");
```

## 泄露随机数k

<img src=".\md_image\1.png" alt="image-20230712101401658" style="zoom:80%;" />

<img src=".\md_image\2.png" alt="image-20230712101401658" style="zoom:80%;" />

## 重复使用随机数k

<img src=".\md_image\3.png" alt="image-20230712101401658" style="zoom:80%;" />

<img src=".\md_image\4.png" alt="image-20230712101401658" style="zoom:80%;" />

## 不同的用户重复使用随机数k

<img src=".\md_image\5.png" alt="image-20230712101401658" style="zoom:80%;" />

<img src=".\md_image\6.png" alt="image-20230712101401658" style="zoom:80%;" />

## SM2签名与ECDSA签名时使用了相同的私钥与随机数k

<img src=".\md_image\7.png" alt="image-20230712101401658" style="zoom:80%;" />

<img src=".\md_image\8.png" alt="image-20230712101401658" style="zoom:80%;" />
