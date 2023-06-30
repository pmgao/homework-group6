首先安装好Bitcoin core钱包(https://bitcoin.org/zh_CN/download)。

新建测试网络收款地址如下，即tb1q34z2uvj2jjyvtwvnkc4mh4q6mly94jhud3hhqr。

<img src=".\md_image\image-20230630111555469.png" alt="image-20230630111555469" style="zoom:50%;" />

https://coinfaucet.eu/en/btc-testnet/ 在这个网站为我的收款地址申请一些测试比特币。

<img src=".\md_image\image-20230630111652528.png" alt="20230630111652528" style="zoom:100%;" />

之后进入https://live.blockcypher.com 查看该地址所发生的交易信息。

![image-20230630112249421](.\md_image\image-20230630112249421.png)

![image-20230630112825273](.\md_image\image-20230630112825273.png)

可以看出，此时该交易只得到了1次确认，需要累积等待到6次确认之后才可以打包进入测试网络的链上。目前的区块链网络大约耗费10分钟得到对一笔交易的确认，总计需要约60分钟。

![image-20230630121321254](.\md_image\image-20230630121321254.png)

此时，查看此笔交易的区块信息如下所示。![image-20230630131339319](.\md_image\image-20230630131339319.png)



在此主要研究几项重要的数据：

```
1、"block_hash": "00000000000000150c126ef9f70fb288a11cfde8642d386a3ee85963e4579fb0"
表明该区块对应的哈希值，可以看出，该哈希值的十六进制高14位为0，相当于求解H(x)<D的困难问题
```

```
2、"block_height": 2439692
表明了该区块所对应的高度
```

```
3、 "inputs": [
    {
      "prev_hash": "a5fab95da1b2e6d047b867b35c68cde0c48cbc11a75dbf73b0895d14981c3421",
      "output_index": 0,
      "output_value": 4148713376,
      "sequence": 4294967293,
      "addresses": [
        "tb1qdp74v89srmd3y858wv3ut848yvdhusl97g78aw"
      ],
      "script_type": "pay-to-witness-pubkey-hash",
      "age": 2439691,
      "witness": [
        "304402201589d226a8adb4477fefdf5a3029033056d868c5b236159f694f9cf8519980f20220413d2d9dbd380e9b6d5be4fc0d958e131b1f55d0afc875fc5bc47cf454d3d1c001",
        "026fbb4760ba60b8fb13f494973124b3610d9b28011b0187782690a459f3c2567d"
      ]
    }
  ]
  表明了付款方的信息，其地址为tb1qdp74v89srmd3y858wv3ut848yvdhusl97g78aw，所对应的转账金额是4148713376
```

```
4、 "outputs": [
    {
      "value": 4147058763,
      "script": "0014b9b5cc5fbe417af49ea3ff1b4a221cf1e2cbb0b6",
      "spent_by": "bcfd4a0d8ddc9fe6d237d5c7432ce5048b41e405109d7497d22e41ef99951907",
      "addresses": [
        "tb1qhx6ucha7g9a0f84rlud55gsu783vhv9ka0t39e"
      ],
      "script_type": "pay-to-witness-pubkey-hash"
    },
    {
      "value": 1640513,
      "script": "00148d44ae324a9488c5b993b62bbbd41adfc85acafc",
      "addresses": [
        "tb1q34z2uvj2jjyvtwvnkc4mh4q6mly94jhud3hhqr"
      ],
      "script_type": "pay-to-witness-pubkey-hash"
    }
  ]
  表明了收款方的信息，其地址为tb1qhx6ucha7g9a0f84rlud55gsu783vhv9ka0t39e，接受的金额数值为1640513
```

```
5、"mrkl_root": "d362af8f3247b217e54210716784ef600e42a4f874b0eff7940782ac8370f7a4"
表明了该区块所存储的merkle树根节点哈希值
```

```
6、"txids": [
    "6f8f249d46f5e96f535840557ab597660da93f203756902193722aff7b5579ac",
    "123df2fca17f2550687499ac142c97b564e79add12b1301a6d38568a9bc17442",
    "967d0f104b6cb1a653fad899171feaa2a89efd50a96eb6916f4886a2655d8f48",
    "1960210a2ee8b4777b3d59bc0c1669caa080ec735d4119086ef9b9af3fbfb8bc",
    "e65c71b3a0447178e8741d019ff7c234ecbc8afb847f3ed0be588b4a27cf23ec",
    "8c14bf4ea34dfae1a1d95563c7e22ef773e4f4f165bb8a28af5db4d6425727ab",
    "821779f510add174eefa2b6861970608a8f2070acae5ee2e4de6ab2f0f88482d",
    "6e12214c92cd45e8ce9861001a086212d1b5092fd11a8ffacd02132da1f5d1c4",
    "985406a632c78d1dcf7ce09330654ccc0ccd913a6f52ca51edb3eb5dd4ef39bc",
    "1910ace20beca25ac80eae34ab0f8c4dfb727f004968228ba4e639ecda44c8f1",
    "3c5461e36367ba8e6a659d64efbf34cc7a86970247f5c06747e259a9ea21c2fe",
    "5658c0fffbd78831f6233504729e52c63c84a02e2fb72051dc0d9948bda24c81",
    "a321294385939b6423e24e563fc1a0fae1071c88a73ee3a4d1b9c921c3439631",
    "0b3dbbca1462bdeaced77802b2badb3da581dec53b6dc80928786bf04c8f28f4",
    "eca8b99b071937e8e7b5867d1be2c876ee7c5f07b81b8291b83ca42c905b4239",
    "adf2e364cf2bc39da79b800759d77b39b9f90ff67675294c0de1f2bb9c1a468e",
    "c577c9a987d0a8b946234422c6bdb89db94ffbd90c3e4e06e8630973dd783c11",
    "e90224bbaaf963e7084ad8325e08ef23b58dc89cb69aaf819079ec0550497124",
    "8d7b06ed169bf62c5756228464a23130fc5549d4be0db3a93ee18a1d14378036",
    "7c1863eb3ab051428aee092fd3232d324e217c0377240d71cc0b4ad361db2c38"
  ]
  表明了该merkle树所存储的交易内容
```

```
7、"chain": "BTC.test3"
表明了此次交易所发生的链
```
