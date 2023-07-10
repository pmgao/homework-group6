# Project9: AES / SM4 software implementation

## AES

代码主要使用x86处理器中所提供的AESNI指令集来实现。

```c++
_mm_aesenc_si128(__m128i /* v */, __m128i /* rkey */): 进行一轮AES加密。
_mm_aesenclast_si128(__m128i /* v */, __m128i /* rkey */)：进行AES的最后一轮加密。
_mm_aesdec_si128(__m128i /* v */, __m128i /* rkey */)：进行一轮AES解密。
_mm_aesdeclast_si128(__m128i /* v */, __m128i /* rkey */)：进行AES的最后一轮解密。
_mm_aesimc_si128(__m128i /* v */)：对输入进行进行逆向列混合操作。
```

<img src=".\md_image\process.jpg" alt="process" style="zoom:50%;" />

<img src=".\md_image\process2.jpg" alt="process2" style="zoom:50%;" />

具体加解密流程如上图所示，其中要注意到加密过程的最后一轮不需要列混合操作；解密过程的最后一轮不需要列混合操作，且中间8轮对逆向列混合与AK操作互换了顺序，需要先求出RK经过逆向列混合之后的数据再进行AK操作。

密钥扩展阶段将加密密钥与解密密钥一齐推入key_schedule数组当中。

```c++
static void aes128_load_key(uint8_t* enc_key, __m128i* key_schedule) {
    aes128_load_key_enc_only(enc_key, key_schedule);

    key_schedule[11] = _mm_aesimc_si128(key_schedule[9]);
    key_schedule[12] = _mm_aesimc_si128(key_schedule[8]);
    key_schedule[13] = _mm_aesimc_si128(key_schedule[7]);
    key_schedule[14] = _mm_aesimc_si128(key_schedule[6]);
    key_schedule[15] = _mm_aesimc_si128(key_schedule[5]);
    key_schedule[16] = _mm_aesimc_si128(key_schedule[4]);
    key_schedule[17] = _mm_aesimc_si128(key_schedule[3]);
    key_schedule[18] = _mm_aesimc_si128(key_schedule[2]);
    key_schedule[19] = _mm_aesimc_si128(key_schedule[1]);
}
```

完整的AES加密、解密流程则通过宏函数的方式来定义。

```c++
#define DO_ENC_BLOCK(m,k) \
    do{\
        m = _mm_xor_si128       (m, k[ 0]); \
        m = _mm_aesenc_si128    (m, k[ 1]); \
        m = _mm_aesenc_si128    (m, k[ 2]); \
        m = _mm_aesenc_si128    (m, k[ 3]); \
        m = _mm_aesenc_si128    (m, k[ 4]); \
        m = _mm_aesenc_si128    (m, k[ 5]); \
        m = _mm_aesenc_si128    (m, k[ 6]); \
        m = _mm_aesenc_si128    (m, k[ 7]); \
        m = _mm_aesenc_si128    (m, k[ 8]); \
        m = _mm_aesenc_si128    (m, k[ 9]); \
        m = _mm_aesenclast_si128(m, k[10]);\
    }while(0)

#define DO_DEC_BLOCK(m,k) \
    do{\
        m = _mm_xor_si128       (m, k[10+0]); \
        m = _mm_aesdec_si128    (m, k[10+1]); \
        m = _mm_aesdec_si128    (m, k[10+2]); \
        m = _mm_aesdec_si128    (m, k[10+3]); \
        m = _mm_aesdec_si128    (m, k[10+4]); \
        m = _mm_aesdec_si128    (m, k[10+5]); \
        m = _mm_aesdec_si128    (m, k[10+6]); \
        m = _mm_aesdec_si128    (m, k[10+7]); \
        m = _mm_aesdec_si128    (m, k[10+8]); \
        m = _mm_aesdec_si128    (m, k[10+9]); \
        m = _mm_aesdeclast_si128(m, k[0]);\
    }while(0)
```

### 运行结果

运行结果如下图所示。所采用的数据样本来自于Dworkin M J, Barker E B, Nechvatal J R, et al. Advanced encryption standard (AES)[J]. 2001.，加解密结果经过比对均一致。一组加解密所需时间开销为纳秒级。

![image-20230710104009810](C:\Users\Lenovo\Desktop\2.png)

# SM4

采用SIMD指令+宏函数+循环展开的方式来对SM4进行优化。

### SIMD指令

因AES中的S盒所属数域为![](https://latex.codecogs.com/svg.image?GF(2^8))的一种结构，同样SM4中的S盒所属数域也为![](https://latex.codecogs.com/svg.image?GF(2^8))，二者之间是同构的，所以我们期望采用x86处理器自身所提供的AESNI指令集来减少通用指令所带来的耗时。

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230710120339611.png" alt="image-20230710120339611" style="zoom:50%;" />

具体地，我们可以使用_mm_aesenclast_si128()函数(没有列混合操作)来完成过S盒的操作，这便需要在SM4算法经过S盒之前经过同构映射映射到AES的数域中，并在经过S盒之后经过同构逆映射映回SM4的数域中。

```c++
inline static __m128i SM4_SBox_TO_AES(__m128i x) {
    __m128i mask = _mm_set_epi8(0x03, 0x06, 0x09, 0x0c, 0x0f, 0x02, 0x05, 0x08,
        0x0b, 0x0e, 0x01, 0x04, 0x07, 0x0a, 0x0d, 0x00);

    x = _mm_shuffle_epi8(x, mask);
    x = _mm_xor_si128(MulMatrixToAES(x), _mm_set1_epi8(0b00100011));
    x = _mm_aesenclast_si128(x, _mm_setzero_si128());

    return _mm_xor_si128(MulMatrixBack(x), _mm_set1_epi8(0b00111011));
}
```

与此同时，代码中还使用了其它的SIMD指令，向量化处理多条数据，提高指令的并发性。

### 宏函数

通过宏函数的内联复制操作，可以减少函数调用所带来的开销，是以空间换时间的优化技巧。

```c++
#define SM4_ITERATION(index)                                                 \
    do {                                                                     \
        __m128i k = _mm_set1_epi32((enc == 0) ? rk[index] : rk[31 - index]); \
        temp = XOR4(Block[1], Block[2], Block[3], k);                        \
        temp = SM4_SBox_TO_AES(temp);                                        \
        temp = XOR6(Block[0], temp, ROTL_EPI32(temp, 2),                     \
            ROTL_EPI32(temp, 10), ROTL_EPI32(temp, 18),                      \
            ROTL_EPI32(temp, 24));                                           \
        Block[0] = Block[1];                                                 \
        Block[1] = Block[2];                                                 \
        Block[2] = Block[3];                                                 \
        Block[3] = temp;                                                     \
    } while (0)
```

### 循环展开

通过循环展开操作，可以减少循环体自身条件更新所带来的时间开销(但实际上也可通过编译优化来实现)。

### 运行结果

所采用数据示例来自于GM/T 0002-2012标准。

![image-20230710121502993](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230710121502993.png)

![image-20230710121531457](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230710121531457.png)

![image-20230710121555573](C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230710121555573.png)

可以看出，加解密结果均一致，证明了代码的正确性，加解密时间也接近于纳秒级(19000ns与3000ns)。
