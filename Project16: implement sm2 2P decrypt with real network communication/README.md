# Project16: implement sm2 2P decrypt with real network communication

<img src=".\md_image\1.png" alt="image-20230717163512842"  />

与Project15有所不同，SM2 two-party decrypt是指网络中的两方共同协作，参与到对同一个密文进行解密的过程当中来，而Project15所进行的是签名算法。

在具体实现时，为了模拟网络通信的环境，我们使用了Python中的socket库，并假设上图中的左侧为client，右侧为server。大致流程可分为以下两步：

1、client生成r1并发送给server，用于参与中间变量T2的生成。

2、client生成T2并发送给server，用于恢复出明文。

代码中所采用的曲线结构为SM2椭圆曲线群。

```python
A = 0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
B = 0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
P = 0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
N = 0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7
G_X = 0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
G_Y = 0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
G = (G_X, G_Y)
```

运行结果如下图所示：

<img src=".\md_image\2.png" alt="image-20230717163512842"  />

<img src=".\md_image\3.png" alt="image-20230717163529174" style="zoom:80%;" />
