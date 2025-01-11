#include <iostream>
#include <vector>
#include <map>

#include "leader.h"
#include <numeric>
#include <cmath>
#include <algorithm>


using namespace std;

// 构造函数实现
leader::leader(vector<int>& P_leader, int leader_id)
    : P_leader(P_leader), leader_id(leader_id) {
        leader_send = vector<vector<vector<vector<int>>>>(M);        
        leader_recv = vector<vector<vector<int>>>(M);
         for (int i = 0; i < M; ++i) {
            leader_send[i].resize(N[i]); 
            leader_recv[i].resize(N[i]);
        }
    }

// 创建并发送查询
map<int, vector<tuple<int, int, int>>> leader::create_and_send_query(int L) {
    int kappa = get_kappa();
    auto random_vector = create_random_vector(kappa, L);
    auto element_to_position = create_and_query(random_vector, L);
    return element_to_position;
}

// 获取 kappa 值
int leader::get_kappa() {
    int R = P_leader.size();
    int kappa = 0;
    for (int i = 0; i < M - 1; i++) {
        int tmp = ceil(R / (N[i] - 1.0));
        if (tmp > kappa) {
            kappa = tmp;
        }
    }
    return kappa;
}

// 创建随机向量
vector<vector<int>> leader::create_random_vector(int kappa, int L) {
    vector<vector<int>> random_vector(kappa);
    for (int i = 0; i < kappa; i++) {
        random_vector[i] = generate_random_vector(L, K);
    }
    return random_vector;
}

// 创建并执行查询
map<int, vector<tuple<int, int, int>>> leader::create_and_query(const vector<vector<int>>& random_vector, int L) {
    int R = P_leader.size();
    map<int, vector<tuple<int, int, int>>> element_to_position;

    for (int i = 0; i < M; i++) {
        if (i == leader_id) continue;

        int counter = 0;
        int eta = ceil(R / (N[i] - 1.0));
        for (int j = 0; j < eta; j++) {
            auto rv = random_vector[j];
            leader_send[i][0].push_back(rv);
            for (int k = 1; k < N[i]; k++) {
                int get_element = P_leader[counter];
                rv = random_vector[j];
                rv[get_element] = (rv[get_element] + 1) % L;
                element_to_position[get_element].push_back(make_tuple(i, k, j));
                leader_send[i][k].push_back(rv);
                counter++;
                if (counter == R) break;
            }
            if (counter == R) break;
        }
    }
    return element_to_position;
}

// 计算交集
vector<int> leader::calculate_intersection(const map<int, vector<tuple<int, int, int>>>& element_to_position, int L) {
    vector<int> intersection;
    for (auto& [element, positions] : element_to_position) {
        vector<int> z;
        for (auto& [i, j, k] : positions) {
            z.push_back(leader_recv[i][j][k] - leader_recv[i][0][k]);
        }
        int e = accumulate(z.begin(), z.end(), 0) % L;
        // cout << e << " ";
        if (e == 0) {
            intersection.push_back(element);
        }
    }
     // cout << endl;
    return intersection;
}