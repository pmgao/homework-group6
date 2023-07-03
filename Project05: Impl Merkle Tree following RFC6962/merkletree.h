#include "binarytree.h"
#include <string.h>
#include <openssl/sha.h>

#define SHA256_LENGTH 64

struct ProofNode {
	char* left, * right, * parent;
	ProofNode() :left(NULL), right(NULL), parent(NULL) {}
	ProofNode(char* _left, char* _right, char* _parent) :left(_left), right(_right), parent(_parent) {}
};


// combin and hash by sha256
static void combin(char* leftData, char* rightData, char out_buff[65]) {

	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, leftData, strlen((const char*)leftData));
	SHA256_Update(&sha256, rightData, strlen((const char*)rightData));
	SHA256_Final(hash, &sha256);

	for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(out_buff + (i * 2), "%02x", hash[i]);
	}
	out_buff[64] = 0;
}

bool verifyProof(char* leaf, char* expectedMerkleRoot, vector<ProofNode> proofArr) {
	if (proofArr.size() == 0) {
		if (strcmp(leaf, expectedMerkleRoot) == 0)
			return true;
		return false;
	}

	// the merkle root should be the parent of the last part
	char* actualMekleRoot = proofArr[proofArr.size() - 1].parent;

	if (strcmp(actualMekleRoot, expectedMerkleRoot) != 0)
		return false;

	char* prevParent = leaf;
	for (int pIdx = 0; pIdx < proofArr.size(); pIdx++) {
		ProofNode part = proofArr[pIdx];

		if (strcmp(part.left, prevParent) != 0 && strcmp(part.right, prevParent) != 0)
			return false;
		char* parentData = new char[65];
		combin(part.left, part.right, parentData);

		// Parent in proof is incorrect
		if (strcmp(parentData, part.parent) != 0)
			return false;

		prevParent = parentData;
	}

	return strcmp(prevParent, expectedMerkleRoot) == 0;
}

class merkletree {
private:
	vector<char*> tree;
public:
	merkletree() {}
	merkletree(vector<char*> leaves, size_t length) {
		tree = computeTree(combin, leaves, length);
	}

	size_t size() { return tree.size(); }
	char* root() { return tree[0]; }

	void calSHA256(char* inp, char out_buff[65]) {
		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256_CTX sha256;
		SHA256_Init(&sha256);
		SHA256_Update(&sha256, inp, strlen(inp));
		SHA256_Final(hash, &sha256);

		//char buffx[65];
		for (size_t i = 0; i < SHA256_DIGEST_LENGTH; i++)
		{
			sprintf(out_buff + (i * 2), "%02x", hash[i]);
		}
		out_buff[64] = 0;
	}



	vector<char*> computeTree(void (*combineFn)(char*, char*, char*), vector<char*> leaves, size_t length) {
		// compute nodeCount and create vector<T> tree
		size_t nodeCount = leafCountToNodeCount(leaves.size());
		size_t delta = nodeCount - leaves.size();
		vector<char*> tree(nodeCount);

		// deep copy
		for (size_t i = 0; i < leaves.size(); i++) {
			tree[delta + i] = new char[65];
			memcpy(tree[delta + i], leaves[i], length);
			tree[delta + i][length] = 0;
		}

		size_t idx = nodeCount - 1;
		while (idx > 0) {
			int parent = (idx - 1) / 2;

			tree[parent] = new char[65];
			combineFn(tree[idx - 1], tree[idx], tree[parent]);
			tree[parent][64] = 0;

			idx -= 2;
		}

		return tree;
	}



	vector<ProofNode> proof(char* leafData) {
		int idx = findLeaf(tree, leafData);
		//printf("idx %d\n",idx);
		if (idx == -1)
			return vector<ProofNode>();
		int proofArrSize = floor(log(tree.size()) / log(2));

		vector<ProofNode> proof(proofArrSize);
		int proofIdx = 0;
		while (idx > 0) {
			idx = getParent(tree, idx);
			int left = getLeft(tree, idx);
			int right = getRight(tree, idx);

			proof[proofIdx++] = ProofNode(tree[left], tree[right], tree[idx]);
		}


		proof.resize(proofIdx);
		return proof;
	}
	void pushleaf(char* leaf) {
		pushleafworker(combin, leaf);
	}

	void pushleafworker(void (*combineFn)(char*, char*, char*), char* leaf) {

		// push two
		tree.push_back(new char[65]);
		tree.push_back(new char[65]);

		int pidx = getParent(tree, tree.size() - 1);

		// push parent and newleaf
		memcpy(tree[tree.size() - 2], tree[pidx], 65);
		memcpy(tree[tree.size() - 1], leaf, 65);

		// climb up and compute
		int idx = tree.size() - 1;
		while (idx > 0) {
			idx = getParent(tree, idx);
			//cout<<&combineFn<<'\n';
			char* buff = new char[65];
			combineFn(tree[getLeft(tree, idx)], tree[getRight(tree, idx)], buff);
			tree[idx] = buff;
		}
	}

};