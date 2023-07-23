Aliyun-Ubuntu Ampere Altra / AltraMax ARM架构处理器，使用Ubuntu 20.04系统，经过gcc/g++编译后运行: g++ aes.cc -march=armv8-a+crypto

代码主要使用arm_neon.h中所提供的CPU指令集来实现。

```c++
vld1q_u8(src): 从src中加载连续的 16 个无符号 8 位整数数据，并返回uint8x16_t的结果  
vaeseq_u8(src1, src2)：src2作为轮密钥，进行AK操作、S-BOX替换与行移位，即应用于加密过程。
vaesmcq_u8(src)：对输入进行进行列混合操作。
vaesdq_u8(src1, src2)：src2作为轮密钥，进行AK操作、逆向S-BOX替换与逆向行移位，即应用于解密过程。
vaesimcq_u8(src)：对输入进行进行逆向列混合操作。
```

<img src=".\md_image\process.jpg" alt="process" style="zoom:50%;" />

<img src=".\md_image\process2.jpg" alt="process2" style="zoom:50%;" />

具体加解密流程如上图所示，其中要注意到加密过程的最后一轮不需要列混合操作；解密过程的最后一轮不需要列混合操作，且中间8轮对逆向列混合与AK操作互换了顺序，需要先求出RK经过逆向列混合之后的数据再进行AK操作。

```c++
void aes128_enc_armv8(const uint8_t in[16], uint8_t ou[16], const uint32_t rk[44]) {
	uint8x16_t block = vld1q_u8(in);

	uint8_t* p8 = (uint8_t*)rk;
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 0)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 1)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 2)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 3)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 4)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 5)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 6)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 7)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 * 8)));

	//final round 
	block = vaeseq_u8(block, vld1q_u8(p8 + 16 * 9));

	//final xor subkey
	block = veorq_u8(block, vld1q_u8(p8 + 16 * 10));

	vst1q_u8(ou, block);
}

void aes128_dec_armv8(const uint8_t in[16], uint8_t ou[16], const uint32_t rk[44]) {
	uint8x16_t block = vld1q_u8(in);

	uint8_t* p8 = (uint8_t*)rk;
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 * 10)));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 9))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 8))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 7))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 6))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 5))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 4))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 3))));
	block = vaesimcq_u8(vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 2))));


	//final round 
	block = vaesdq_u8(block, vaesimcq_u8(vld1q_u8(p8 + 16 * 1)));
	//final xor subkey
	block = veorq_u8(block, vld1q_u8(p8 + 16 * 0));

	vst1q_u8(ou, block);
}
```

并且在实现时要注意到大多数ARM处理器默认为小端模式，与传统的x86处理器不同，所以在开始加解密操作之前，还要对轮密钥进行大小端的数据转换。

```c++
uint32_t byte_swap32(uint32_t val) {
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}
```

运行结果如下图所示。所采用的数据样本来自于Dworkin M J, Barker E B, Nechvatal J R, et al. Advanced encryption standard (AES)[J]. 2001.，加解密结果经过比对均一致。

<img src=".\md_image\example1.png" alt="example1" style="zoom:80%;" />

<img src=".\md_image\example2.png" alt="example2" style="zoom:80%;" />

<img src=".\md_image\AES.png" alt="AES" style="zoom:80%;" />

可以看出，密钥扩展耗时大约为600~700ns，AES一个分组的加解密则耗时2*240ns左右，如果并行化程度足够高，则对数据进行AES加解密的时间开销可以控制在纳秒级。
