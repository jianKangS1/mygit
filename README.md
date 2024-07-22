# 创建环境

```bash
sudo apt-get update
sudo apt-get install libssl-dev # openssl需要的库
```
# git命令参照表
| Git命令 | 功能 | 备注|
| --- | --- | --- |
| git init | 创建一个新的 Git 仓库 |
| git hash-object -w filename | 计算文件内容的哈希值并将其存储在 Git 仓库中 | 这个和git算出来的不一样，因为git还要添加一部分内容|
| git cat-file -p hash | 显示指定哈希值的对象内容 |

# 命令作用
- hash-object filename 
    - 读取文件内容，计算哈希值得到hash值 oid
    - 将 blob + ' ' + 内容，存储到.mygit/objects/oid 下。
- cat-file oid 读取指定哈希值对应的文件
- write-tree directoryname 
    - 读取当前目录下所有文件，计算分别计算每个文件的哈希值，得到一串list  type + ' ' + oid + name 
    - 计算list的hash值oid，创建文件.mygit/objects/oid，将 tree + ' ' + list写入文件
- read-tree oid
    - 读取oid的文件，获取list，逐个解析出type和oid，读取oid对应的文件，将内容写入对应目录下

初始项目文件 https://www.leshenko.net/p/ugit/#create-commit