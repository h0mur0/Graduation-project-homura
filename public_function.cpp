#include <vector>
#include <random>
#include <cmath>
#include <stdexcept>
#include "public_function.h"


using namespace std;

int mod_exp(int a, int b, int m) {
    int result = 1;
    a = a % m;  // 防止a大于m

    while (b > 0) {
        if (b % 2 == 1) {  // 如果b是奇数
            result = (result * a) % m;
        }
        a = (a * a) % m;  // 将a平方
        b /= 2;  // b右移一位，相当于除以2
    }

    return result;
}

// 生成一个长度为 K 的随机向量，每个元素在 [0, L-1] 范围内
vector<int> generate_random_vector(int L, int K) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, L - 1);

    vector<int> random_vector(K);
    for (int i = 0; i < K; i++) {
        random_vector[i] = dis(gen);
    }
    return random_vector;
}

// 计算两个向量的点积
int dot_product(const vector<int>& vec1, const vector<int>& vec2) {
    if (vec1.size() != vec2.size()) {
        throw invalid_argument("两个向量的长度必须相同");
    }
    int result = 0;
    for (size_t i = 0; i < vec1.size(); i++) {
        result += vec1[i] * vec2[i];
    }
    return result;
}

// 判断一个数是否为素数，使用 Miller-Rabin 算法
bool is_prime(int n, int k) {
    if (n <= 1) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    int d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(2, n - 2);

    for (int i = 0; i < k; i++) {
        int a = dis(gen);
        int x = mod_exp(a, d, n);
        if (x == 1 || x == n - 1) continue;
        for (int j = 0; j < s - 1; j++) {
            x = (x * x) % n;
            if (x == n - 1) break;
        }
        if (x != n - 1) return false;
    }
    return true;
}

// 查找大于 M 的下一个素数
int select_L(int M) {
    while (!is_prime(M)) {
        M++;
    }
    return M;
}

// 选择最优的领导者
int select_leader(const vector<vector<int>>& P, const vector<int>& N) {
    int t = 0;
    int min_cost = 0;

    // 计算选择每个客户端作为领导者时的代价
    auto get_cost = [&](int i) {
        int sum = 0;
        for (int j = 0; j < M; j++) {
            if (j != i) {
                sum += ceil((double)P[i].size() * N[j] / (N[j] - 1));
            }
        }
        return sum;
    };

    // 计算每个可能的领导者的代价，选择代价最小的领导者
    for (int i = 0; i < M; i++) {
        int cost = get_cost(i);
        if (cost < min_cost || min_cost == 0) {
            min_cost = cost;
            t = i;
        }
    }
    return t;
}