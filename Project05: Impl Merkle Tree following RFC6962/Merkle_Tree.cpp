#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
#include"merkletree.h"
#include<time.h>
#include<math.h>
#include<random>

#define TEST_INSERT 1
#define MAX_SIZE 100000
#define length uint64_t(log(double(MAX_SIZE)) / log(double(16))) + 1

static uint8_t* table[MAX_SIZE];


void generate_data() {
	for (size_t i = 0; i < MAX_SIZE; i++) {
		srand(time(NULL));
		table[i] = new uint8_t[length];
		for (size_t j = 0; j < length; j++) {
			table[i][j] = rand() % 255;
		}
	}

}

int main() {
	generate_data();

	clock_t t;
	vector<char*> v(MAX_SIZE - TEST_INSERT);
	for (int i = 0; i < MAX_SIZE - TEST_INSERT; i++) {
		v[i] = (char*)table[i];
	}

	t = clock();
	merkletree mtree = merkletree(v, length);
	printf("maked merkle\n");
	for (int i = MAX_SIZE - TEST_INSERT; i < MAX_SIZE; i++) {
		v.push_back((char*)table[i]);
		mtree.pushleaf(v[i]);
	}
	t = clock() - t;
	printf("root: %s\n", mtree.root());
	printf("[build mtree] took %d clocks (%f secs)\n", t, (float)t / CLOCKS_PER_SEC);

	{
		//Inclusion Proof
		vector<ProofNode> proof = mtree.proof(v[127]);
		bool verproof = verifyProof(v[127], mtree.root(), proof);
		printf("[verify proof][ => %d ]\n", verproof);
	}

	{
		//Exclusion Proof
		char temp = 0x01;
		vector<ProofNode> proof = mtree.proof(&temp);
		bool verproof = verifyProof(&temp, mtree.root(), proof);
		printf("[verify proof][ => %d ]\n", verproof);
	}

	return 0;
}
