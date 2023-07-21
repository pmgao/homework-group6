# Project2: implement the Rho method of reduced SM3

## Rho算法：


> <img src=".\md_image\20230330-sm3-public.jpg" alt="image-20230703174051013" style="zoom: 67%;" />

该算法的原理可以认为是生日攻击算法的一个扩展，主要是构造出一个终归周期序列，设定两个初始值x,y，经过一个固定的函数F(x)来更新这两个初始值，尝试找到一组碰撞满足![](https://latex.codecogs.com/svg.image?&space;H(x_i)=H(H(y_i)))。

## 运行环境

编译器：Visual Studio 2019，MSVC编译器，C11/C++14标准，64位环境

第三方库：Openssl 1.1.1l版本

操作系统：Windows10

测速CPU：

| 主要参数     | 参数数值                               |
| ------------ | -------------------------------------- |
| 型号         | AMD Ryzen 7 5800H with Radeon Graphics |
| 架构         | x86架构                                |
| 主频         | 3.20GHz（基准速度）                    |
| 逻辑处理器数 | 16                                     |
| 核心数       | 8                                      |

## 项目结果：

本项目所用的SM3算法利用Openssl库所提供的API接口来实现。因要找到SM3完整输出长度的碰撞耗时过久，所以在本项目中分别研究杂凑值中的头1个字节、2个字节、3个字节与4个字节的碰撞。

![](https://latex.codecogs.com/svg.image?(x,y))中的两个初始值分别设为11与13，变换函数如下所示。
```c++
#define MAX 32768
#define F(x) ((x*x+128)%MAX)
```
不断用这个函数更新这两个初始值，探究![](https://latex.codecogs.com/svg.image?&space;H(x_i)=H(H(y_i)))是否成立，若成立则认为找到了一组碰撞

实验结果如下表所示。

| 碰撞字节数 | 碰撞时间                                         |
| ---------- | ------------------------------------------------ |
| 1          | ![](https://latex.codecogs.com/svg.image?1453us) |
| 2          | ![](https://latex.codecogs.com/svg.image?1488us) |
| 3          | ![](https://latex.codecogs.com/svg.image?1516us) |
| 4          | ![](https://latex.codecogs.com/svg.image?1723us) |

