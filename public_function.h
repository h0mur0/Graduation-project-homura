#ifndef PUBLIC_FUNCTION_H
#define PUBLIC_FUNCTION_H

#include <vector>
#include <tuple>
#include "para.h"

using namespace std;

// 计算 a^b mod m
int mod_exp(int a, int b, int m);

// 生成一个长度为 K 的随机向量，每个元素在 [0, L-1] 范围内
vector<int> generate_random_vector(int L, int K);

// 计算两个向量的点积
int dot_product(const vector<int>& vec1, const vector<int>& vec2);

// 判断一个数是否为素数，使用 Miller-Rabin 算法
bool is_prime(int n, int k = 20);

// 查找大于 M 的下一个素数
int select_L(int M);

// 选择最优的领导者
int select_leader(const vector<vector<int>>& P, const vector<int>& N);

#endif // PUBLIC_FUNCTION_H
