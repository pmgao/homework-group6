# Project4: do your best to optimize SM3 implementation (software)

本项目文件夹中有两个cpp文件，其中SM3.cpp文件是基于Openssl源码所做的实现，主要用于对比后续优化代码的效果及结果的正确性，而SM3_SIMD.cpp文件则是利用SIMD指令对SM3进行的优化代码。

## SM3杂凑算法原理

> 以下内容来自于国家密码局的密码标准列表，具体算法细节可以参考此资料：[参考资料](http://www.gmbz.org.cn/main/viewfile/20180108023812835219.html)
>
> 假定消息输入长度为![](https://latex.codecogs.com/svg.image?l)比特，则经过SM3杂凑算法之后，杂凑输出长度为256比特。
>
> ![img](https://img2023.cnblogs.com/blog/1138214/202212/1138214-20221227194622193-899324839.png)

## SM3杂凑算法——Openssl实现：

选取的数据示例采用国家密码局标准文档所给出的示例。输入消息为字符串“abc”。

<img src=".\md_image\1.png" alt="image-20230703174051013" style="zoom: 100%;" />

<img src=".\md_image\2.png" alt="image-20230703174051013" style="zoom: 100%;" />

<img src=".\md_image\3.png" alt="image-20230703174051013" style="zoom: 100%;" />

经过比对可以看出，此时的代码实现是正确的。

## SM3杂凑算法——SIMD实现：

在对SM3杂凑算法进行加速时，综合运用了SIMD指令集、宏定义函数与循环展开等优化操作。

### 宏定义函数：

宏定义函数相当于对常规定义的函数做了内联操作，通过简单的代码替换来取消正常函数调用所带来的开销，包括：开辟空间，记录返回地址，形参压栈，从函数中返回释放堆栈。

而在SM3杂凑算法的流程当中，存在一些被反复使用的布尔函数FF、GG及置换函数P等，可以将这些被反复用到的函数使用宏函数的方式去定义，减少调用开销。

```c++
#define rol(x,j) ((x<<j)|(uint32_t(x)>>(32-j)))
#define P0(x) ((x) ^ rol((x), 9) ^ rol((x), 17))
#define P1(x) ((x) ^ rol((x), 15) ^ rol((x), 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | ((~(x)) & (z)))
```

### 循环展开：

循环展开是指将循环体的语句进行复制，此种操作虽然会增大代码文件的规模，但是可以减少预测错误和循环条件更新所带来的时间开销。

```
UNROLL_LOOP_16_1(LOAD_AND_STORE);
```

### SIMD指令集：

传统的CPU只支持64bit数的寄存器和64bit数的运算，而SIMD指令集提供了更高比特的寄存器及其相应的运算函数，包括128bit，256bit与512bit的长度，此种指令集可以为我们的各种操作提供更高的并行度，做到同时处理多个32bit数、64bit数等。

以宏定义的LOAD_AND_STORE操作为例：

```
#define LOAD_AND_STORE(i) \
    __m128i temp##i = _mm_loadu_si128(src + i); \
    _mm_storeu_si128(dst + i, temp##i);
```

通过_mm_loadu_si128函数可以一次性读出128bit的整数，相当于128/8=8个字；通过_mm_storeu_si128函数可以也一次性存入128bit的整数，减少了反复读出、存入的次数，提供了良好的并行性。

## 运行环境

编译器：Visual Studio 2019，MSVC编译器，C11/C++14标准，64位环境

第三方库：Openssl 1.1.1l版本

操作系统：Windows10

测速CPU：

| 主要参数     | 参数数值                               |
| ------------ | -------------------------------------- |
| 型号         | AMD Ryzen 7 5800H with Radeon Graphics |
| 架构         | x86架构                                |
| 主频         | 3.20GHz（基准速度）                    |
| 逻辑处理器数 | 16                                     |


### SIMD实现结果：

首先验证结果的正确性，输入的字符串仍为"abc"，输出结果如下图所示：

<img src=".\md_image\4.png" alt="image-20230703174051013" style="zoom: 100%;" />

结果与先前的一致，证明了我们结果的正确性。

而对于时间效率的比较，我们可以设定一个非常长的输入值，统计对它迭代进行128次SM3杂凑运算所需的时间。

```c++
 uint8_t plaintext[MAX_LEN] = "202100460055202100460055202100460055202100460055202100460055202100460055202100460055202100460055202100460055202100460055";
 //……
 auto t1 = steady_clock::now();
 for (uint32_t i = 0; i < 128; i++) {
     sm3_simd(plaintext, hash_val, len);
 }
 auto t2 = steady_clock::now();
```

统计结果如下表所示：

| 实现版本    | 运行时间   |
| ----------- | ---------- |
| Openssl实现 | 730~750 us |
| SIMD实现    | 640~660 us |

可以看出，相较于开源库Openssl的实现，我们的效率也有了进一步的提升，证明了我们的优化是有效的。

