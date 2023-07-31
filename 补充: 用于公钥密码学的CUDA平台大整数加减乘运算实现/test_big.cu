#include <stdio.h>
#include <iostream>
using namespace std;
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cuda.h"
#include <cmath>
#include <inttypes.h>
#include <gmp.h>
#include "./MSM.cuh"
#define SIZE 4

template<uint32_t limbs>
__global__ void kernel(uint32_t* d_a, uint32_t* d_b, uint32_t* d_c){
	np0=computeNP0(d_c[0]);
	uint64_t evenOdd[limbs];
	bool carry=mp_mul_red_cl<limbs>(evenOdd,d_a,d_b,d_c);
	mp_merge_cl<limbs>(d_a,evenOdd,carry);
}

int main() {
	// 在主机内存中定义输入数据
	uint32_t a[SIZE] = { 0x01345670, 0xabcdef12, 0x87654321, 0x0edcba10 };
	uint32_t b[SIZE] = { 0x01345670, 0xabcdef21, 0x87654398, 0x0edcba10 };
	uint32_t c[SIZE] = { 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff };
	uint32_t r[SIZE];
	uint32_t r_[SIZE];
	
	mpz_t A, B, C, R;
	size_t size = sizeof(a) / sizeof(uint32_t);

	mpz_init(A); mpz_init(B); mpz_init(C); mpz_init(R);
	mpz_import(A, size, -1, sizeof(uint32_t), 0, 0, a);
	mpz_import(B, size, -1, sizeof(uint32_t), 0, 0, b);
	mpz_import(C, size, -1, sizeof(uint32_t), 0, 0, c);
	mpz_mul(R, A, B);
	mpz_mod(R, R, C);
	gmp_printf("Result: %Zd\n", R);
	size_t mark;
	mpz_export(r, &mark, -1, sizeof(uint32_t), 0, 0, R);
	while (mark<SIZE)
		r[mark++] = 0;

	cout << "GMP:\n";
	//cout<<"MARK:"<<mark<<endl;
	for (int i = 0; i<SIZE; i++)
	{
		cout << hex << r[i] << endl;
	}
	
	uint32_t* d_a;
	uint32_t* d_b;
	uint32_t* d_c;
	cudaMalloc(&d_a, sizeof(uint32_t)*SIZE);
	cudaMalloc(&d_b, sizeof(uint32_t)*SIZE);
	cudaMalloc(&d_c, sizeof(uint32_t)*SIZE);
	cudaMemcpy(d_a, a, sizeof(uint32_t)*SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, b, sizeof(uint32_t)*SIZE, cudaMemcpyHostToDevice);
	cudaMemcpy(d_c, c, sizeof(uint32_t)*SIZE, cudaMemcpyHostToDevice);

	kernel<SIZE> <<<1,1>>>(d_a, d_b, d_c);
	cudaDeviceSynchronize();
	cudaMemcpy(r_, d_a, sizeof(uint32_t)*SIZE, cudaMemcpyDeviceToHost);

	cout << "test:\n";
	for (int i = 0; i<SIZE; i++)
	{
		cout << hex << r_[i] << endl;
	}
	return 0;
}
