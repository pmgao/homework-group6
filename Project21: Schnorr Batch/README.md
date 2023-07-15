# Project21: Schnorr Batch

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230715134318095.png" alt="image-20230715134318095"  />

图片来源于课程PPT，该项目主要实现的是Schnorr签名的批量验证功能，对于多条签名数据，可利用Schnorr签名的线性性质，通过上述PPT所讲流程进行一次性验证。

## 项目结果

​	所采用的曲线结构为secpk256k1曲线，所签名消息为字符串“202100460055”，并设定参与签名的成员共有2个，按照上图流程分别计算得到s，R，e与P的和。

```python
privKey_bob = 77664663271170673620859955297191590031376319879614890096024130175852238738811
privKey_alice = 89652975980192045565381556847798492396888680198332589948144044069692575244768
PublicKey_bob = elliptic_multiply(GPoint, privKey_bob)
PublicKey_alice = elliptic_multiply(GPoint, privKey_alice)
```

<img src="C:\Users\Lenovo\AppData\Roaming\Typora\typora-user-images\image-20230715142534911.png" alt="image-20230715142534911"  />

此时的Schnorr便可一齐验证通过二者的签名数据，相当于对签名进行了聚合并只需作一次验证，此种批量验证的策略提高了签名的性能优势，向外界隐藏了此次交易是一笔多签交易还是非多签交易