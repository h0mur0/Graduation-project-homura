## 代码简介

该代码为homura的毕设代码，可实现信息论安全的多方隐私求交。

## 编译与运行

### 编译代码

首先使用 `g++` 或其他支持 C++ 的编译器编译该程序：

```bash
g++ main.cpp public_function.cpp channel.cpp client.cpp database.cpp -o main
```

### 运行程序

程序通过命令行传入参数进行控制。运行程序的基本格式如下：

```bash
./main -m [正整数a] -t [a个文件名] -n [a个整数]
```

## 参数说明

1. `-m [正整数a]`
   - **作用**：设置客户端数量M。
2. `-t [a个文件名]`
   - **作用**：设置客户端文件名列表 `fileNames`，指明存储客户端数据的文件路径。
3. `-n [a个整数]`
   - **作用**：设置整数列表 `N`，指明客户端的数据库数量。
4. `-h`
   - **作用**：输出帮助信息，解释如何使用该程序。
