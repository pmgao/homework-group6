#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<arm_neon.h>


const uint32_t RK[44] = { 
	0x2b7e1516,0x28aed2a6,0xabf71588,0x09cf4f3c,
	0xa0fafe17,0x88542cb1,0x23a33939,0x2a6c7605,
	0xf2c295f2,0x7a96b943,0x5935807a,0x7359f67f,
	0x3d80477d,0x4716fe3e,0x1e237e44,0x6d7a8b3b,
	0xef44a541,0xa8525b7f,0xb671253b,0xdb0bad00,
	0xd4d1c6f8,0x7c839d87,0xcaf2b8bc,0x11f915bc,
	0x6d88a37a,0xaa0b3efd,0xdbf98641,0xca0093fd,
	0x4e54f70e,0x5f5fc9f3,0x84a64fb2,0x4ea6dc4f,
	0xead27321,0xb58dbad2,0x312bf560,0x7f8d292f,
	0xac7766f3,0x19fadc21,0x28d12941,0x575c006e,
	0xd014f9a8,0xc9ee2589,0xe13f0cc8,0xb6630ca6
};

 uint8_t IN[16] = {
	0x32,0x88,0x31,0xe0,
	0x43,0x5a,0x31,0x37,
	0xf6,0x30,0x98,0x07,
	0xa8,0x8d,0xa2,0x34
};
uint8_t OU[16];

void Print(uint8_t buff[16]) {
	for (size_t i = 0; i < 16; i++) {
		printf("%02x ", buff[i]);
	}
	printf("\n");
}

void aes128_enc_armv8(const uint8_t in[16], uint8_t ou[16], const uint32_t rk[44]) {
	uint8x16_t block = vld1q_u8(in);

	uint8_t* p8 = (uint8_t*)rk;
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 0)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 1)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 2)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 3)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 4)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 5)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 6)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 7)));
	block = vaesmcq_u8(vaeseq_u8(block, vld1q_u8(p8 + 16 + 8)));

	//final round 
	block = vaeseq_u8(block, vld1q_u8(p8 + 16 * 9));

	//final xor subkey
	block = veorq_u8(block, vld1q_u8(p8 + 16 * 10));

	vst1q_u8(ou, block);
}

void aes128_dec_armv8(const uint8_t in[16], uint8_t ou[16], const uint32_t rk[44]) {
	uint8x16_t block = vld1q_u8(in);

	uint8_t* p8 = (uint8_t*)rk;


	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 0)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 1)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 2)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 3)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 4)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 5)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 6)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 7)));
	block = vaesimcq_u8(vaesdq_u8(block, vld1q_u8(p8 + 16 + 8)));


	//final round 
	block = vaesdq_u8(block, vld1q_u8(p8 + 16 * 9));
	//final xor subkey
	block = veorq_u8(block, vld1q_u8(p8 + 16 * 10));

	vst1q_u8(ou, block);
}

int main() {
	Print(IN);
	Print(OU);
	aes128_enc_armv8(IN, OU, RK);
	Print(OU);
	aes128_dec_armv8(OU, IN, RK);
	Print(IN);

	return 0;

}
