# Project01: implement the naïve birthday attack of reduced SM3

## 生日攻击：

定理：设杂凑函数h的输出值长度为n比特，则经过$2^n$次杂凑运算，找到一对碰撞的![](https://latex.codecogs.com/svg.image?(x,x'))概率大于1/2。

过程：不断遍历穷举不同元素的哈希值，当穷举到约$2^n$个元素时，即可找到一组碰撞。

## 实验结果：

因要找到SM3完整输出长度的碰撞耗时过久，所以在本项目中分别研究杂凑值中的头1个字节、2个字节与3个字节的碰撞，需要穷举的元素数目约为![](https://latex.codecogs.com/svg.image?&space;2^{\frac{8}{2}}=2^4,2^{\frac{16}{2}}=2^8,2^{\frac{24}{2}}=2^{12})，对于现代计算机的计算能力而言这些迭代次数都是可以接受的，时间开销并不大。

$(x,x^{'})$中的x为字符串202100460055，而另外一个元素x'自“1”开始不断累加，对其进行穷举，实验结果如下表所示。

| 碰撞字节数 | 碰撞时间  |
| ---------- | --------- |
| 1          | ![](https://latex.codecogs.com/svg.image?1794us)  |
| 2          | ![](https://latex.codecogs.com/svg.image?36285us)|
| 3          | ![](https://latex.codecogs.com/svg.image?19s)    |

