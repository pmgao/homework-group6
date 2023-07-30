# 补充项目：用于公钥密码学的CUDA平台大整数加减乘运算实现

## CGBN库的缺点

本学期做其它的某个项目时，碰到了一个难点：便是CUDA平台上的大整数运算实现，众所周知CUDA平台所支持的数据宽度大多与原生的C++一致，只能支持到64比特(虽然自CUDA 11.5版本之后开始支持128比特宽度，但128比特寄存器的读取速度目前仍然比不上32比特寄存器，速度较慢)，但我的难点主要是在CUDA平台上的大整数模加、模减与模乘的快速实现，而通用的CUDA平台第三方大整数运算库CGBN虽然实现速度已经足够快，但存在着一定的缺陷：

> 1、必须将cgbn_add(), cgbn_sub(), cgbn_mul()等函数API包装成核函数的形式，这样只能从host或global(自CUDA 5.0版本之后开始支持此种调用方法)端去调用这个包装好的global函数，存在着不小的网格、线程创建的调用开销。
>
> 2、CGBN库调用大整数运算时，强制要求使用多线程来实现，要求线程数目至少为4，不能进行单线程的实现，但有时公钥密码学所要求的参数长度大多为256，512与1024比特，比特宽度并不长，如果开启了过分多的线程反而会因这些调用开销拖慢其运算速度，更合理的方法便是在CUDA硬件中使用单线程的方法去作这些比特长度上的运算，而CGBN库目前还未实现，CGBN主要针对的是更加庞大规模的长数据（最高支持到32K比特）。
>
> 3、CGBN库在调用加减乘运算之前需要创建相应的运算环境，且其语法很不自然（当时在这上面被折磨了非常非常久）。

## 项目实现

鉴于CGBN库上述的缺点，它并不完全适用于公钥密码学中所需的大数运算，因此我参考了2022年的Z-PRIZE零知识证明加速竞赛中的[MSM(多标量乘法)](https://github.com/z-prize/2022-entries/tree/main/open-division/prize1-msm/prize1a-msm-gpu/yrrid/yrrid-msm) 实现，该实现做的是椭圆曲线上的快速基本算子运算，而我则将其中更基础的加减乘运算剥离出来，得到适用于各种公钥密码学的基础运算函数。

### 模加

这些基础函数主要集中于MP.cu文件中，首先是模加运算：

```c++
 __device__ __forceinline__ uint32_t uaddc_cc(uint32_t a, uint32_t b) {
  uint32_t r;
  
  asm volatile ("addc.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
  return r;
}

__device__ __forceinline__ uint32_t add(uint32_t a, uint32_t b) {
    if(firstOperation) 
      uadd_cc(0, 0);
    firstOperation=false;
    return uaddc_cc(a, b);
  }

template<uint32_t limbs>
__device__ __forceinline__ void mp_add(uint32_t* r, const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    r[i]=chain.add(a[i], b[i]);
}
```

很明显，所要抽出的函数就是mp_add函数，该函数使用模板化的方法，limbs代表了所传入的长比特数据由多少个uint32_t所组成，为什么不使用uint64_t？有两方面的考量：

> 1、原生CUDA平台最高只支持到64比特的运算，如果直接将64比特数相加其最高位的进位会被丢掉，无法用于下一组64比特数的加法。
>
> 2、每个CUDA核心中寄存器资源最充足的便是32比特寄存器，通过使用uint32_t寄存器可以充分利用CUDA核心的寄存器资源。

与此同时，这个实现还通过内联汇编、函数内联(by forceinline)的方式尽可能的减少了函数调用的开销。但这还不是我们所想要的，该实现缺少模运算的实现，所以需要在加法结束之后，比较加法结果是否大于模数，若大于则减去模数，否则不作处理

```c++
__device__ __forceinline__ void ADD(uint32_t* a, const uint32_t* b, uint32_t* result) {
    mp_add<NUM>(a,b,result);
    while(mp_comp_ge<NUM>(result,MODULUS)){
        mp_sub<NUM>(result,MODULUS,result);
    }
}
```

mp_comp_ge()函数也是通过内联汇编+函数内联方式进行两个数的减法，进而判断其大小关系。可见，上述实现的函数都是通过device来修饰的，不需要创建新的网格or线程便可以调用，方便了核函数对其调用。(NUM是通过宏定义所声明的uint32_t数量，MODULUS也是预先所声明的大模数数组)

### 模减

与模加十分类似，在此不予赘述。

```c++
__device__ __forceinline__ uint32_t usub_cc(uint32_t a, uint32_t b) {
  uint32_t r;

  asm volatile ("sub.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
  return r;
}

  __device__ __forceinline__ uint32_t sub(uint32_t a, uint32_t b) {
    if(firstOperation)
      uadd_cc(1, 0xFFFFFFFF);
    firstOperation=false;
    return usubc_cc(a, b);
  }

template<uint32_t limbs>
__device__ __forceinline__ void mp_sub(uint32_t* r, const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    r[i]=chain.sub(a[i], b[i]);
}

__device__ __forceinline__ void SUB(uint32_t* a, const uint32_t* b, uint32_t* result) {
    while(mp_comp_ge<NUM>(a,b)){
        mp_add<NUM>(b,MODULUS,b);
    }
    mp_sub<NUM>(b,a,result);
    while(mp_comp_ge<NUM>(result,MODULUS)){
        mp_sub<NUM>(result,MODULUS,result);
    }
}
```

### 模乘

该项目模乘函数的核心是通过蒙哥马利算法运算实现的，通过此种算法可以将模运算所需要的除法运算去掉，大幅提高运算效率，在此不赘述蒙哥马利算法的具体流程，直接展示其相关函数：

```c++
__device__ __forceinline__ static uint32_t qTerm(uint32_t lowWord, uint32_t np0) {
  uint64_t product = (uint64_t)lowWord * np0;
  uint32_t result = (uint32_t)(product & 0xFFFFFFFF); 
  return result;
}

template<uint32_t limbs>
__device__ __forceinline__ bool mp_sqr_red_cl(uint64_t* evenOdd, uint32_t* temp, const uint32_t* a, const uint32_t* n) {
  uint64_t* even=evenOdd;
  uint64_t* odd=evenOdd + limbs/2;
  chain_t   chain;
  bool      carry=false;
  uint32_t  lo=0, q, c1, c2, low, high;
  
  // This routine can be used when a+n < R (i.e. it doesn't carry out).  Hence the name cl for carryless.
  // Only works with an even number of limbs.
  //
  // Note, many chain.reset() ops have been removed.  They aren't strictly necessary, since the prior step of
  // the computation is guaranteed to clear the carry bit.  If we leave the chain.reset() ops in the PTX stream,
  // then the compiler tends to use IMAD.X to collect carries, which steals multiply cycles.  IMAD.X can't produce
  // carry outs, so embedding the last output in a carry chain forces the compiler to generate IADD ops.
  
  mp_zero<limbs>(temp);
  
  #pragma unroll
  for(int32_t i=0;i<limbs/2;i++) {
    even[i]=make_wide(0, 0);
    odd[i]=make_wide(0, 0);
  }
  
  // do odds
  for(int32_t j=limbs-1;j>0;j-=2) {
    chain.reset();
    for(int i=0;i<limbs-j;i++)
      evenOdd[j/2+i+1]=chain.madwide(a[i], a[i+j], evenOdd[j/2+i+1]);
  }

  // shift right
  for(int32_t i=0;i<limbs-1;i++)
    evenOdd[i]=make_wide(uhigh(evenOdd[i]), ulow(evenOdd[i+1]));
  evenOdd[limbs-1]=make_wide(uhigh(evenOdd[limbs-1]), 0);
   
  // do evens
  for(int32_t j=limbs-2;j>0;j-=2) {
    chain.reset();
    for(int i=0;i<limbs-j;i++) 
      evenOdd[j/2+i]=chain.madwide(a[i], a[i+j], evenOdd[j/2+i]);
    temp[limbs-j]=(chain.add(0, 0)!=0) ? 2 : 0;
  }

  // double
  chain.reset();
  for(int32_t i=0;i<limbs;i++) {
    low=chain.add(ulow(evenOdd[i]), ulow(evenOdd[i]));
    high=chain.add(uhigh(evenOdd[i]), uhigh(evenOdd[i]));
    evenOdd[i]=make_wide(low, high);
  }

  // add diagonals
  chain.reset();
  for(int32_t i=0;i<limbs;i++) 
    evenOdd[i]=chain.madwide(a[i], a[i], evenOdd[i]);

  // add high part of wide to b...
  chain.reset();
  for(int32_t i=0;i<limbs;i+=2) {
    temp[i]=chain.add(ulow(evenOdd[limbs/2+i/2]), temp[i]);
    temp[i+1]=chain.add(uhigh(evenOdd[limbs/2+i/2]), temp[i+1]);
  }

  #pragma unroll
  for(int32_t i=0;i<limbs/2;i++) 
    odd[i]=make_wide(0, 0);

  // now we need to reduce
  #pragma unroll
  for(int i=0;i<limbs/2;i++) {
    if(i!=0) {
      // integrate lo
      chain.reset(carry);
      lo=chain.add(lo, ulow(even[0]));
      carry=chain.add(0, 0)!=0;
      even[0]=make_wide(lo, uhigh(even[0]));
    }
    
    q=qTerm(ulow(even[0]));

    // shift even by 64 bits
    chain.reset();
    even[0]=chain.madwide(q, n[0], even[0]);
    lo=uhigh(even[0]);
    #pragma unroll
    for(int j=2;j<limbs;j+=2)
      even[j/2-1]=chain.madwide(q, n[j], even[j/2]);
    c1=chain.add(0, 0);

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      odd[j/2]=chain.madwide(q, n[j+1], odd[j/2]);
      
    // second half

    // integrate lo
    chain.reset(carry);
    lo=chain.add(lo, ulow(odd[0]));
    carry=chain.add(0, 0)!=0;
    odd[0]=make_wide(lo, uhigh(odd[0]));
    
    q=qTerm(ulow(odd[0]));

    // shift odd by 64 bits
    chain.reset();
    odd[0]=chain.madwide(q, n[0], odd[0]);
    lo=uhigh(odd[0]);
    for(int j=2;j<limbs;j+=2)
      odd[j/2-1]=chain.madwide(q, n[j], odd[j/2]);
    odd[limbs/2-1]=0;
    c2=chain.add(0, 0);

    chain.reset();
    for(int j=0;j<limbs-2;j+=2)
      even[j/2]=chain.madwide(q, n[j+1], even[j/2]);
    even[limbs/2-1]=chain.madwide(q, n[limbs-1], make_wide(c1, c2));
  }
  
  chain.reset();
  for(int i=0;i<limbs;i+=2) {
    low=chain.add(ulow(even[i/2]), temp[i]);
    high=chain.add(uhigh(even[i/2]), temp[i+1]);
    even[i/2]=make_wide(low, high);
  }
  
  chain.reset(carry);
  lo=chain.add(lo, ulow(even[0]));
  carry=chain.add(0, 0)!=0;
  even[0]=make_wide(lo, uhigh(even[0]));
  return carry;
}
```

当然，这个实现还做了其它的优化技巧，比如循环展开、进位预测(虽然我也不是很理解这个)等，但其核心仍然是蒙哥马利乘法。

将其包装成MUL函数，在调用完上述蒙哥马利乘法函数后，还需将在蒙哥马利域下的结果转回正常的表示方法：

```c++
__device__ __forceinline__ void MUL(uint32_t* a, uint32_t* b, uint32_t* result){
    uint64_t evenOdd[NUM];
	bool carry=mp_mul_red_cl<NUM>(evenOdd,a,b,MODULUS);
	mp_merge_cl<NUM>(result,evenOdd,carry);
}
```

至此，我们便成功剥离了用device所修饰的快速大数加减乘实现，避免了创建新线程所带来的调用开销。

对于其实现效果，在此借用原项目作者的[结论](https://github.com/yrrid/submission-msm-gpu)：

> We generally observe the running time to be between ***2500 milliseconds*** and ***2600 milliseconds*** for a full run of 4 MSMs of size 2^26. Performance is quite dependent on the other workloads running on the same physical machine.

而我在单块A100的GPU平台上所作测试之后，在无其它GPU负载的情况下上述运算效率会更加快速。
