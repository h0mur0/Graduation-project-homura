#include <vector>
#include "para.h"

int M = 4;    // 领导者的数量
vector<int> N = {2, 3, 5, 4};    // 每个客户端的数据库数量
vector<int> Sk = {0, 1, 2, 3, 4, 5};    // 一些特殊的常量（例如：1, 2, 3 等）
int K = Sk.size();    // Sk 的长度
vector<vector<int>> P = {
    {1, 2, 3, 4, 5},
    {2, 3, 4, 5},
    {1, 3, 5},
    {1, 3, 4, 5}
};    // 每个客户端所对应的数据库的列表
