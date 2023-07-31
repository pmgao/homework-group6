# Cyberspace Security Innovation and Entrepreneurship Practice Course

山东大学网络空间安全学院（研究院）2023年网络空间安全创新创业实践课程相关project完成情况及相应代码。

------

## 小组成员：

本课程选择进行个人组队，所有项目由个人完成，队伍序号为group6 num1。

| 小组成员姓名 | 学号         |               Github账户名称               |
| :----------: | ------------ | :----------------------------------------: |
|    高培民    | 202100460055 | [**Borpord** ](https://github.com/Borpord) |

## 项目及完成情况：

本课程共有21个项目，完成了其中的20个项目，未完成项目Project 7。

| 项目序号  | 项目名称                                                     | 对应项目文件夹                                               | 项目完成情况 | 项目实现方式及效果                                           |
| :-------: | ------------------------------------------------------------ | ------------------------------------------------------------ | :----------: | ------------------------------------------------------------ |
| Project1  | implement the naïve birthday attack of reduced SM3           | [**birthday attack of SM3** ](https://github.com/Borpord/homework-group6/tree/main/Project01:%20implement%20the%20na%C3%AFve%20birthday%20attack%20of%20reduced%20SM3) |      ✔️       | 利用C++与Openssl库来进行攻击，可在秒级实现对前24bit的攻击。  |
| Project2  | implement the Rho method of reduced SM3                      | [**rho attack of SM3** ](https://github.com/Borpord/homework-group6/tree/main/Project02%3A%20implement%20the%20Rho%20method%20of%20reduced%20SM3) |      ✔️       | 利用C++与Openssl库来进行攻击，可在微秒级实现对前24bit的攻击。 |
| Project3  | implement length extension attack for SM3, SHA256, etc.      | [**length extension attack** ](https://github.com/Borpord/homework-group6/tree/main/Project03%3A%20implement%20length%20extension%20attack%20for%20SM3%2C%20SHA256%2C%20etc.) |      ✔️       | 利用C++与Project4中所实现的SM3算法，针对SM3算法开展了长度扩展攻击 |
| Project4  | do your best to optimize SM3 implementation (software)       | [**optimized SM3** ](https://github.com/Borpord/homework-group6/tree/main/Project04%3A%20do%20your%20best%20to%20optimize%20SM3%20implementation%20(software)) |      ✔️       | 通过C++语言，分别抽取了Openssl库中对于SM3的实现与自己手动实现的SM3算法，并采用了SIMD指令、循环展开与宏定义函数的方式来进行优化，最后可实现约10%~15%的加速。 |
| Project5  | Impl Merkle Tree following RFC6962                           | [**Merkle Tree** ](https://github.com/Borpord/homework-group6/tree/main/Project05%3A%20Impl%20Merkle%20Tree%20following%20RFC6962) |      ✔️       | 利用C++语言来实现，采用的哈希函数实例为SHA256算法。          |
| Project6  | impl this protocol with actual network communication         | [**Hash Wires for Range Proof** ](https://github.com/Borpord/homework-group6/tree/main/Project06:%20impl%20this%20protocol%20with%20actual%20network%20communication) |      ✔️       | 仿照课程PPT流程，为了使得Alice向Bob证明她的年龄大于21岁，依赖于可信第三方，利用C++语言与实例化的SM3算法来进行零知识的范围证明(by hash wires)。 |
| Project7  | Try to Implement this scheme                                 | ❌                                                            |      ❌       | ❌                                                            |
| Project8  | AES impl with ARM instruction                                | [**AES impl with ARM** ](https://github.com/Borpord/homework-group6/tree/main/Project08%3A%20AES%20impl%20with%20ARM%20instruction) |      ✔️       | 借助于云服务器，通过C++语言，采用ARM处理器所提供的AESNI指令来完成AES的加解密函数，并验证了其正确性，加解密时间可以做到纳秒级。 |
| Project9  | AES / SM4 software implementation                            | [**AES/SM4 impl** ](https://github.com/Borpord/homework-group6/tree/main/Project09%3A%20AES%20and%20SM4%20software%20implementation) |      ✔️       | 通过C++语言，首先利用x86处理器的AESNI指令完成了对于AES的软件实现，之后通过AES与SM4算法数域结构的同构特性，也利用AESNI指令完成了对于SM4算法的实现，对于所实现的AES与SM4分别验证了其正确性，并且加解密时间均可做到纳秒级。 |
| Project10 | report on the application of this deduce technique in Ethereum with ECDSA | [**the deduce technique of ECDSA** ](https://github.com/Borpord/homework-group6/tree/main/Project10%3A%20report%20on%20the%20application%20of%20this%20deduce%20technique%20in%20Ethereum%20with%20ECDSA) |      ✔️       | 主要介绍了从ECDSA签名中推导出公钥的方法，并介绍了此种方法对于区块链网络的优势所在。 |
| Project11 | impl sm2 with RFC6979                                        | [**impl sm2 with RFC6979** ](https://github.com/Borpord/homework-group6/tree/main/Project11%3A%20impl%20sm2%20with%20RFC6979) |      ✔️       | 通过Python语言，以SM2签名算法版本为例，并根据RFC6979标准修改了随机数选取的方法。 |
| Project12 | verify the above pitfalls with proof-of-concept code         | [**verify the pitfalls** ](https://github.com/Borpord/homework-group6/tree/main/Project12%3A%20verify%20the%20above%20pitfalls%20with%20proof-of-concept%20code) |      ✔️       | 主要根据课程PPT中所介绍了4种安全性漏洞，通过C++语言，借助于Openssl库所提供的椭圆曲线运算函数，对这4种安全性漏洞依次进行了验证。 |
| Project13 | Implement the above ECMH scheme                              | [**ECMH** ](https://github.com/Borpord/homework-group6/tree/main/Project13%3A%20Implement%20the%20above%20ECMH%20scheme) |      ✔️       | 通过Python语言，基于secpk256k1曲线，将哈希值映射到了椭圆曲线上的一个点。 |
| Project14 | Implement a PGP scheme with SM2                              | [**PGP scheme with SM2** ](https://github.com/Borpord/homework-group6/tree/main/Project14%3A%20Implement%20a%20PGP%20scheme%20with%20SM2) |      ✔️       | 通过Python语言，将公钥加密算法(SM2)与分组密码(AES)相结合，实现了课程PPT所述的PGP方案。 |
| Project15 | implement sm2 2P sign with real network communication        | [**sm2 2P sign** ](https://github.com/Borpord/homework-group6/tree/main/Project15%3A%20implement%20sm2%202P%20sign%20with%20real%20network%20communication) |      ✔️       | 通过Python语言，并通过socket库来模拟网络通信，在SM2曲线上实现了两方共同参与的SM2签名流程。 |
| Project16 | implement sm2 2P decrypt with real network communication     | [**sm2 2P decrypt** ](https://github.com/Borpord/homework-group6/tree/main/Project16%3A%20implement%20sm2%202P%20decrypt%20with%20real%20network%20communication) |      ✔️       | 通过Python语言，并通过socket库来模拟网络通信，在SM2曲线上实现了两方共同参与的SM2加解密流程。 |
| Project17 | 比较Firefox和谷歌的记住密码插件的实现区别                    | [**compare FireFox with Google** ](https://github.com/Borpord/homework-group6/tree/main/Project17%EF%BC%9A%E6%AF%94%E8%BE%83Firefox%E5%92%8C%E8%B0%B7%E6%AD%8C%E7%9A%84%E8%AE%B0%E4%BD%8F%E5%AF%86%E7%A0%81%E6%8F%92%E4%BB%B6%E7%9A%84%E5%AE%9E%E7%8E%B0%E5%8C%BA%E5%88%AB) |      ✔️       | 根据课程PPT及网络文档资料，比较了Google与Firefox对于记住密码插件的区别并分别阐述了其各自的优劣所在。 |
| Project18 | send a tx on Bitcoin testnet, and parse the tx data down to every bit, better write script yourself | [**send a tx on Bitcoin testnet** ](https://github.com/Borpord/homework-group6/tree/main/Project18%3A%20send%20a%20tx%20on%20Bitcoin%20testnet%2C%20and%20parse%20the%20tx%20data%20down%20to%20every%20bit%2C%20better%20write%20script%20yourself) |      ✔️       | 在实际的比特币测试网络上成功创建了一笔交易，并获取了其交易脚本的内容。 |
| Project19 | forge a signature to pretend that you are Satoshi            | [**forge a signature** ](https://github.com/Borpord/homework-group6/tree/main/Project19%3A%20forge%20a%20signature%20to%20pretend%20that%20you%20are%20Satoshi) |      ✔️       | 按照课程PPT所述流程，通过C++语言，借助于Openssl库来完成椭圆曲线上的运算，完成了对于ECDSA签名的伪造并成功对其进行验证。 |
| Project21 | Schnorr Batch                                                | [**Schnorr Batch** ](https://github.com/Borpord/homework-group6/tree/main/Project21%3A%20Schnorr%20Batch) |      ✔️       | 按照PPT所述流程，通过Python语言，在secpk256k1曲线上完成了对于Schnorr签名的批量验证。 |
| Project22 | research report on MPT                                       | [**research report on MPT** ](https://github.com/Borpord/homework-group6/tree/main/Project22%3A%20research%20report%20on%20MPT) |      ✔️       | 首先分别介绍了默克尔树与前缀树，之后介绍了二者的结合——即MPT树，包含其节点类型，编码方式，增删查改操作及其特点的内容介绍。 |

其中：

- ✔️：项目已完成
- ❌：项目未完成

所有项目的详细报告内容，可进入对应的项目文件夹详细查看。与此同时，本仓库还添加了本学期所作的其它的一个额外项目的子模块：[用于公钥密码学的CUDA平台大整数加减乘运算实现](https://github.com/Borpord/homework-group6/tree/main/%E8%A1%A5%E5%85%85:%20%E7%94%A8%E4%BA%8E%E5%85%AC%E9%92%A5%E5%AF%86%E7%A0%81%E5%AD%A6%E7%9A%84CUDA%E5%B9%B3%E5%8F%B0%E5%A4%A7%E6%95%B4%E6%95%B0%E5%8A%A0%E5%87%8F%E4%B9%98%E8%BF%90%E7%AE%97%E5%AE%9E%E7%8E%B0)，主要基于多标量乘法来实现对于256bit, 512bit, 1024bit或其它长度数据的快速运算，虽然与本课程内容并无直接关系，但可能会对于CUDA平台的密码学项目开发比较有用，因而也引入到本仓库当中。
