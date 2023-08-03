#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#define NUM 8
__device__ const uint32_t MODULUS[NUM] = { 
    0xf0000001, 0x43e1f593, 
    0x79b97091, 0x2833e848, 
    0x8181585d, 0xb85045b6, 
    0xe131a029, 0x30644e72};

__device__ uint32_t np0;
__device__ const uint32_t BIT_WIDE = 256;  // Adjustment according to the actual situation

__device__ __forceinline__ uint32_t load_shared_u32(uint32_t sAddr) {
	uint32_t r;
	
	asm volatile ("ld.shared.u32 %0,[%1];" : "=r"(r) : "r"(sAddr));
	return r;
  }
  
  __device__ __forceinline__ void store_shared_u32(uint32_t sAddr, uint32_t value) {
	asm volatile ("st.shared.u32 [%0],%1;" : : "r"(sAddr), "r"(value));
  }
  
  __device__ __forceinline__ uint32_t shared_atomic_add_u32(uint32_t sAddr, uint32_t value) {
	uint32_t r;
	
	asm volatile ("atom.shared.add.u32 %0,[%1],%2;" : "=r"(r) : "r"(sAddr), "r"(value));
	return r;
  }
  
  __device__ __forceinline__ uint32_t shared_atomic_exch_u32(uint32_t sAddr, uint32_t value) {
	uint32_t r;
	
	asm volatile ("atom.shared.exch.b32 %0,[%1],%2;" : "=r"(r) : "r"(sAddr), "r"(value));
	return r;
  }
  
  __device__ __forceinline__ void shared_reduce_add_u32(uint32_t sAddr, uint32_t value) {
	asm volatile ("red.shared.add.u32 [%0],%1;" : : "r"(sAddr), "r"(value)); 
  }
  
  __device__ __forceinline__ uint2 load_shared_u2(uint32_t sAddr) {
	uint2 r;
	
	asm volatile ("ld.shared.v2.u32 {%0,%1},[%2];" : "=r"(r.x), "=r"(r.y) : "r"(sAddr));
	return r;
  }
  
  __device__ __forceinline__ void store_shared_u2(uint32_t sAddr, uint2 value) {
	asm volatile ("st.shared.v2.u32 [%0],{%1,%2};" : : "r"(sAddr), "r"(value.x), "r"(value.y));
  }
  
  __device__ __forceinline__ uint4 load_shared_u4(uint32_t sAddr) {
	uint4 r;
	
	asm volatile ("ld.shared.v4.u32 {%0,%1,%2,%3},[%4];" : "=r"(r.x), "=r"(r.y), "=r"(r.z), "=r"(r.w) : "r"(sAddr));
	return r;
  }
  
  __device__ __forceinline__ void store_shared_u4(uint32_t sAddr, uint4 value) {
	asm volatile ("st.shared.v4.u32 [%0],{%1,%2,%3,%4};" : : "r"(sAddr), "r"(value.x), "r"(value.y), "r"(value.z), "r"(value.w));
  }
  
  __device__ __forceinline__ void shared_async_copy_u4(uint32_t sAddr, void* ptr) {
	asm volatile ("cp.async.cg.shared.global [%0], [%1], 16;" : : "r"(sAddr), "l"(ptr));
  }
  
  __device__ __forceinline__ void shared_async_copy_commit() {
	asm volatile ("cp.async.commit_group;");
  }
  
  __device__ __forceinline__ void shared_async_copy_wait() {
	asm volatile ("cp.async.wait_all;");
  }
  
  __device__ __forceinline__ uint32_t prmt(uint32_t lo, uint32_t hi, uint32_t control) {
	uint32_t r;
	
	asm volatile ("prmt.b32 %0,%1,%2,%3;" : "=r"(r) : "r"(lo), "r"(hi), "r"(control));
	return r;
  }
  
  __device__ __forceinline__ uint32_t uadd_cc(uint32_t a, uint32_t b) {
	uint32_t r;
  
	asm volatile ("add.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint32_t uaddc_cc(uint32_t a, uint32_t b) {
	uint32_t r;
	
	asm volatile ("addc.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint32_t uaddc(uint32_t a, uint32_t b) {
	uint32_t r;
	
	asm volatile ("addc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint32_t usub_cc(uint32_t a, uint32_t b) {
	uint32_t r;
  
	asm volatile ("sub.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint32_t usubc_cc(uint32_t a, uint32_t b) {
	uint32_t r;
	
	asm volatile ("subc.cc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint32_t usubc(uint32_t a, uint32_t b) {
	uint32_t r;
	
	asm volatile ("subc.u32 %0,%1,%2;" : "=r"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ bool getCarry() {
	return uaddc(0, 0)!=0;
  }
  
  __device__ __forceinline__ void setCarry(bool cc) {
	uadd_cc(cc ? 1 : 0, 0xFFFFFFFF);
  }
  
  __device__ __forceinline__ uint64_t mulwide(uint32_t a, uint32_t b) {
	uint64_t r;
	
	asm volatile ("mul.wide.u32 %0,%1,%2;" : "=l"(r) : "r"(a), "r"(b));
	return r;
  }
  
  __device__ __forceinline__ uint64_t madwide(uint32_t a, uint32_t b, uint64_t c) {
	uint64_t r;
	
	asm volatile ("mad.wide.u32 %0,%1,%2,%3;" : "=l"(r) : "r"(a), "r"(b), "l"(c));
	return r;
  }
  
  __device__ __forceinline__ uint64_t madwide_cc(uint32_t a, uint32_t b, uint64_t c) {
	uint64_t r;
	
	asm volatile ("{\n\t"
				  ".reg .u32 lo,hi;\n\t"
				  "mov.b64        {lo,hi},%3;\n\t"
				  "mad.lo.cc.u32  lo,%1,%2,lo;\n\t"
				  "madc.hi.cc.u32 hi,%1,%2,hi;\n\t"
				  "mov.b64        %0,{lo,hi};\n\t"
				  "}" : "=l"(r) : "r"(a), "r"(b), "l"(c));                
	return r;
  }
  
  __device__ __forceinline__ uint64_t madwidec_cc(uint32_t a, uint32_t b, uint64_t c) {
	uint64_t r;
	
	asm volatile ("{\n\t"
				  ".reg .u32 lo,hi;\n\t"
				  "mov.b64        {lo,hi},%3;\n\t"
				  "madc.lo.cc.u32 lo,%1,%2,lo;\n\t"
				  "madc.hi.cc.u32 hi,%1,%2,hi;\n\t"
				  "mov.b64        %0,{lo,hi};\n\t"
				  "}" : "=l"(r) : "r"(a), "r"(b), "l"(c));                
	return r;
  }
  
  __device__ __forceinline__ uint64_t madwidec(uint32_t a, uint32_t b, uint64_t c) {
	uint64_t r;
	
	asm volatile ("{\n\t"
				  ".reg .u32 lo,hi;\n\t"
				  "mov.b64        {lo,hi},%3;\n\t"
				  "madc.lo.cc.u32 lo,%1,%2,lo;\n\t"
				  "madc.hi.cc.u32 hi,%1,%2,hi;\n\t"
				  "mov.b64        %0,{lo,hi};\n\t"
				  "}" : "=l"(r) : "r"(a), "r"(b), "l"(c));                
	return r;
  }
  
  __device__ __forceinline__ uint2 u2madwidec_cc(uint32_t a, uint32_t b, uint2 c) {
	uint2 r;
	
	asm volatile ("madc.lo.cc.u32  %0,%2,%3,%4;\n\t"
				  "madc.hi.cc.u32 %1,%2,%3,%5;" : "=r"(r.x), "=r"(r.y) : "r"(a), "r"(b), "r"(c.x), "r"(c.y));
	return r;
  }
  
  __device__ __forceinline__ uint32_t ulow(uint2 xy) {
	return xy.x;
  }
  
  __device__ __forceinline__ uint32_t uhigh(uint2 xy) {
	return xy.y;
  }
  
  __device__ __forceinline__ uint32_t ulow(uint64_t wide) {
	uint32_t r;
  
	asm volatile ("mov.b64 {%0,_},%1;" : "=r"(r) : "l"(wide));
	return r;
  }
  
  __device__ __forceinline__ uint32_t uhigh(uint64_t wide) {
	uint32_t r;
  
	asm volatile ("mov.b64 {_,%0},%1;" : "=r"(r) : "l"(wide));
	return r;
  }
  
  __device__ __forceinline__ uint64_t make_wide(uint32_t lo, uint32_t hi) {
	uint64_t r;
	
	asm volatile ("mov.b64 %0,{%1,%2};" : "=l"(r) : "r"(lo), "r"(hi));
	return r;
  }
  
  __device__ __forceinline__ uint64_t make_wide(uint2 xy) {
	return make_wide(xy.x, xy.y);
  }

  /***

Copyright (c) 2022, Yrrid Software, Inc.  All rights reserved.
Licensed under the Apache License, Version 2.0, see LICENSE for details.

Written by Niall Emmart.

***/

#include <stdint.h>  

class chain_t {
  public:
  bool firstOperation;
  
  __device__ __forceinline__ chain_t() {
    firstOperation=true;
  }
  
  __device__ __forceinline__ chain_t(bool carry) {
    firstOperation=false;
    uadd_cc(carry ? 1 : 0, 0xFFFFFFFF);
  }
  
  __device__ __forceinline__ void reset() {
    firstOperation=true;
    uadd_cc(0, 0);
  }
  
  __device__ __forceinline__ void reset(bool carry) {
    firstOperation=false;
    uadd_cc(carry ? 1 : 0, 0xFFFFFFFF);
  }
  
  __device__ __forceinline__ bool getCarry() {
    return uaddc(0, 0)!=0;
  }
  
  __device__ __forceinline__ uint32_t add(uint32_t a, uint32_t b) {
    if(firstOperation) 
      uadd_cc(0, 0);
    firstOperation=false;
    return uaddc_cc(a, b);
  }
  
  __device__ __forceinline__ uint32_t sub(uint32_t a, uint32_t b) {
    if(firstOperation)
      uadd_cc(1, 0xFFFFFFFF);
    firstOperation=false;
    return usubc_cc(a, b);
  }
  
  __device__ __forceinline__ uint2 madwide(uint32_t a, uint32_t b, uint2 c) {
    if(firstOperation) 
      uadd_cc(0, 0);
    firstOperation=false;    
    return u2madwidec_cc(a, b, c);
  }

  __device__ __forceinline__ uint64_t madwide(uint32_t a, uint32_t b, uint64_t c) {
    if(firstOperation) 
      uadd_cc(0, 0);
    firstOperation=false;    
    return madwidec_cc(a, b, c);
  }
};

__device__ __forceinline__ static uint32_t qTerm(uint32_t lowWord, uint32_t np0) {
  uint64_t product = (uint64_t)lowWord * np0;
  uint32_t result = (uint32_t)(product & 0xFFFFFFFF); 
  return result;
}

__device__ __forceinline__ uint32_t computeNP0(uint32_t x) {
  uint32_t inv=x;

  inv=inv*(inv*x+14);
  inv=inv*(inv*x+2);
  inv=inv*(inv*x+2);
  inv=inv*(inv*x+2);
  return inv;
}

template<uint32_t limbs>
__device__ __forceinline__ bool mp_comp_gt(const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  // a>b --> b-a is negative
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    chain.sub(b[i], a[i]);
  return !chain.getCarry();
}

template<uint32_t limbs>
__device__ __forceinline__ bool mp_comp_ge(const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    chain.sub(a[i], b[i]);
  return chain.getCarry();
}

template<uint32_t limbs>
__device__ __forceinline__ void mp_add(uint32_t* r, const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    r[i]=chain.add(a[i], b[i]);
}

template<uint32_t limbs>
__device__ __forceinline__ void mp_sub(uint32_t* r, const uint32_t* a, const uint32_t* b) {
  chain_t chain;
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i++)
    r[i]=chain.sub(a[i], b[i]);
}

template<uint32_t limbs>
__device__ __forceinline__ void mp_merge_cl(uint32_t* r, const uint64_t* evenOdd, bool carry) {
  chain_t chain(carry);
 
  r[0]=ulow(evenOdd[0]);
  for(int i=0;i<limbs/2-1;i++) {
    r[2*i+1]=chain.add(uhigh(evenOdd[i]), ulow(evenOdd[limbs/2 + i]));
    r[2*i+2]=chain.add(ulow(evenOdd[i+1]), uhigh(evenOdd[limbs/2 + i]));
  }
  r[limbs-1]=chain.add(uhigh(evenOdd[limbs/2-1]), 0);
}

template<uint32_t limbs>
__device__ __forceinline__ bool mp_mul_red_cl(uint64_t* evenOdd, const uint32_t* a, const uint32_t* b, const uint32_t* n) {
  uint64_t* even=evenOdd;
  uint64_t* odd=evenOdd + limbs/2;
  chain_t   chain;
  bool      carry=false;
  uint32_t  lo=0, q, c1, c2;
  
  // This routine can be used when max(a, b)+n < R (i.e. it doesn't carry out).  Hence the name cl for carryless.
  // Only works with an even number of limbs.
     
  #pragma unroll
  for(int32_t i=0;i<limbs/2;i++) {
    even[i]=make_wide(0, 0);
    odd[i]=make_wide(0, 0);
  }
  
  #pragma unroll
  for(int32_t i=0;i<limbs;i+=2) {
    if(i!=0) {
      // integrate lo
      chain.reset(carry);
      lo=chain.add(lo, ulow(even[0]));
      carry=chain.add(0, 0)!=0;
      even[0]=make_wide(lo, uhigh(even[0]));
    }

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      even[j/2]=chain.madwide(a[i], b[j], even[j/2]);
    c1=chain.add(0, 0);

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      odd[j/2]=chain.madwide(a[i], b[j+1], odd[j/2]);

    q=qTerm(ulow(even[0]),np0);

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      odd[j/2]=chain.madwide(q, n[j+1], odd[j/2]);

    chain.reset();
    even[0]=chain.madwide(q, n[0], even[0]);
    lo=uhigh(even[0]);
    #pragma unroll
    for(int j=2;j<limbs;j+=2)
      even[j/2-1]=chain.madwide(q, n[j], even[j/2]);
    c1=chain.add(c1, 0);
      
    // integrate lo
    
    chain.reset(carry);
    lo=chain.add(lo, ulow(odd[0]));
    carry=chain.add(0, 0)!=0;
    odd[0]=make_wide(lo, uhigh(odd[0]));

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      odd[j/2]=chain.madwide(a[i+1], b[j], odd[j/2]);
    c2=chain.add(0, 0);

    q=qTerm(ulow(odd[0]),np0);

    // shift odd by 64 bits

    chain.reset();
    odd[0]=chain.madwide(q, n[0], odd[0]);
    lo=uhigh(odd[0]);
    #pragma unroll
    for(int j=2;j<limbs;j+=2)
      odd[j/2-1]=chain.madwide(q, n[j], odd[j/2]);
    c2=chain.add(c2, 0);

    odd[limbs/2-1]=make_wide(0, 0);
    even[limbs/2-1]=make_wide(c1, c2);
    
    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      even[j/2]=chain.madwide(a[i+1], b[j+1], even[j/2]);

    chain.reset();
    #pragma unroll
    for(int j=0;j<limbs;j+=2)
      even[j/2]=chain.madwide(q, n[j+1], even[j/2]);
  }

  chain.reset(carry);
  lo=chain.add(lo, ulow(even[0]));
  carry=chain.add(0, 0)!=0;
  even[0]=make_wide(lo, uhigh(even[0]));
  return carry;
}


__device__ __forceinline__ void ADD(uint32_t* a, const uint32_t* b, uint32_t* result) {
    mp_add<NUM>(a,b,result);
    if(mp_comp_ge<NUM>(result,MODULUS)){
        mp_sub<NUM>(result,MODULUS,result);
    }
}

__device__ __forceinline__ void SUB(uint32_t* a, const uint32_t* b, uint32_t* result) {
    mp_sub<NUM>(a,b,result);
    if(mp_comp_gt<NUM>(result,a)){
        mp_add<NUM>(result,MODULUS,result);
    }
}

__device__ __forceinline__ void MUL(uint32_t* a, uint32_t* b, uint32_t* result){
    uint64_t evenOdd[NUM];
	bool carry=mp_mul_red_cl<NUM>(evenOdd,a,b,MODULUS);
	mp_merge_cl<NUM>(result,evenOdd,carry);
}
