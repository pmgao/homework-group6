# 比较Firefox和谷歌的记住密码插件的实现区别

Firefox与谷歌的记住密码插件最大区别在于Firefox允许用户使用一个主密码来解锁剩余的其它的密码(PBE技术)，而谷歌则是存储某个用户的所有密码。

## Firefox

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230703174051013.png" alt="image-20230703174051013" style="zoom: 67%;" />

首先介绍Firefox的处理策略，具体流程可如上图所示(内容来自于https://hacks.mozilla.org/2018/11/firefox-sync-privacy/?_gl=1*ar00f3*_ga*NDA3ODQ0Nzc5LjE2ODgyNjYzNTA.*_ga_2VC139B3XV*MTY4ODM3NzAyNi40LjEuMTY4ODM3NzI4Ni4wLjAuMA..)。

具体地，一个用户利用只有自己才知道的Password经过PBKDF推导出主密钥，并将其分派给两个固定的HKDF函数，派生出两个新的密钥，其中一个密钥作为参与验证的Token发送给Firefox的服务器，与用户名称共同经过脚本处理后的哈希值与服务器本地所存储的哈希值进行比较，若相同则返回加密数据，否则失败。当用户端接收到数据之后，使用另外一份密钥对该加密数据进行解密(目前所用的是对称密码技术，AES-256-CBC版本)，即可得到解密后的密码内容。

## 谷歌

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230703174946993.png" alt="image-20230703174946993" style="zoom:80%;" />

谷歌的具体流程如上图所示，使用的是公钥密码技术，基于离散对数困难问题。当用户每次需要一个不同的密码时，以上流程便需要重新进行。

## 比较结果

可以看出，Firefox是经过一次交互就可以取得剩余的所有密码，且基于对称密码技术；而谷歌用户则对于每个密码都需要一次交互，基于公钥密码技术，显然谷歌服务器还需要存储用户的所有密码。

这两种方法各有优劣，Firefox减少了交互次数，提高了用户端的使用效率，但是有可能存在用户端因使用不当而泄露所有密码的风险。谷歌虽然增多了交互次数，但是可以保证用户在某一时刻只知道自己的一个密码，减少了其它密码泄露的风险，提高了用户端使用的安全性。