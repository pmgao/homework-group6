# report on the application of this deduce technique in Ethereum with ECDSA

![process](.\md_image\process.png)

流程如上图所示，对于ECDSA签名的输出值(r,s)，私钥d_A，公钥P_A，有：
$$
s\times (1+d_A)=(k-r\times d_A) \mod n \\
(s+r)\times d_A=(k-s) \mod n \\
((s+r)\times d_A)G=(k-s)G \mod n \\
d_A\times G=P_A=((s+r)^{-1}(k-s))G
$$
即可由一组确定的签名值，推导出相应的公私钥，通过此种方法有以下几种优点：

1、可以减少网络通信的压力，使得签名传输时只需要传输签名值而不需要再传输公钥，减轻了以太坊网络的通信压力。

2、避免了公钥在传输过程中被直接篡改的行为，因为在交互的过程中只需要传输签名，公钥可由签名推导出来，敌手无法直接篡改公钥。

3、在区块中可以不存储公钥，而是由签名推导出公钥，减少了以太坊中每个区块所占用的存储容量，减轻了以太坊网络的存储压力。

4、在存储压力与通信压力得到降低之后，以太坊网络用户之间的数据同步会变得更加容易。