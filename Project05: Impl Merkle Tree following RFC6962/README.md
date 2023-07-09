# Project5: Impl Merkle Tree following RFC6962

## 默克尔树原理

Merkle Tree是存储hash值的一棵树，在该树的最底层可以看作一个顺序哈希列表，当层数向上走时，相邻两个哈希值合并成一个字符串，并由该字符串计算出新的哈希值，最终只剩下唯一的一个哈希值，该哈希值被称为根节点。

显然，此种计算方法要求顺序哈希列表所含元素数目为偶数个，在实际场景下有可能存在奇数的情况，此种情况有两种处理策略：1、复制出一个相同的叶子节点凑成偶数 2、将单独多出来的叶子节点作为新计算的哈希值进入上一层，而RFC6962标准建议采用后一种算法，并使用一种类似于不平衡的二叉树来构建Merkle树。



<img src=".\md_image\1.png" alt="image-20230708235635449" style="zoom: 80%;" />

## 存在性证明：

如果需要知道某个节点是否存在于Merkle树对应的集合中，只需要找出与该节点两两配对路径上的各个节点，并不断配对得到根节点，判断此根节点是否与已知的根节点相同，若相同则完成了存在性证明。

## 不存在性证明：

利用存在性证明的过程完成即可。

## 项目结果：

在代码中，我们使用随机数生成了10w条数据，并通过宏常量TEST_INSERT设定这10w条数据中所要插入的数据个数。

```c++
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
```

在main函数中，测量了构建merkle tree所需的时间，并分别使用叶子节点数组中的一个索引项来进行存在性证明，使用一个自定义变量来进行不存在性证明(1代表在merkle tree当中，0则代表不属于merkle tree)。

```c++
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
```

运行结果如下图所示：

<img src=".\md_image\2.png" alt="image-20230709105704975"  />
