# Project14: Implement a PGP scheme with SM2

<img src=".\md_image\1.png" alt="image-20230716104317963"  />

具体流程如上图所示，在具体实现时，需要用到公钥加密算法与对称密码算法，我们选取的公钥加密算法为SM2曲线上的SM2加密算法，对称密码算法为AES-128-ECB模式。

整体流程为：

1、生成SM2加解密公私钥与AES对称密码密钥。

2、进行Encrypt流程，使用AES密钥加密明文，使用SM2公钥加密AES密钥。

```python
def PGP_encrypt(message, key):
    cryptor = AES.new(key.encode('utf-8'), AES.MODE_ECB)

    length = 16
    count = len(message)
    add = length - (count % length) if count % length != 0 else 0

    padded_message = message + ('\0' * add)
    ciphertext1 = cryptor.encrypt(padded_message.encode('utf-8'))
    key_encode = key.encode('utf-8')
    ciphertext2 = sm2_crypt.encrypt(key_encode)

    print("Encrypted message using the session key k (AES):", ciphertext1)
    print("Encrypted session key k using the SM2 public key:", ciphertext2)

    return ciphertext1, ciphertext2
```

其中，填充规则选择在明文的末尾补0字节，按照PPT流程分别对明文与对称密码密钥进行加密。

3、进行Decrypt流程，使用SM2私钥解密得到AES密钥，并继而使用该AES密钥解密得到原明文内容。

```python
def PGP_decrypt(ciphertext1, ciphertext2):
    session_key = sm2_crypt.decrypt(ciphertext2)
    print("Decrypted session key using SM2 private key:", session_key.decode('utf-8'))

    cryptor = AES.new(session_key, AES.MODE_ECB)
    plain_text = cryptor.decrypt(ciphertext1)
    print("Decrypted original message using session key:", plain_text.rstrip(b'\x00').decode('utf-8'))
```

这个流程中，得到明文之后将其unpad，去除末尾所填充的0字节，得到其原始的明文内容。



运行结果如下图所示：

<img src=".\md_image\2.png" alt="image-20230716105807698"  />
