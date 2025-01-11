#include <vector>
#include <cmath>
#include <string>

#include "client.h"

using namespace std;

// 构造函数实现
client::client(string state, int client_id) : state(state), client_id(client_id) {
    client_send_to_database_s = vector<vector<int>>(N[client_id], vector<int>());
    client_send_to_database_t = vector<vector<int>>(N[client_id], vector<int>());
    client_send_to_client = vector<int>();
    client_recv_from_client = vector<vector<int>>();
}

// 创建并发送本地随机数
void client::create_and_send_local_randomness(int L, int R) {
    int eta = ceil(R / (N[client_id] - 1.0));
    auto rv = generate_random_vector(L, eta);
    for (int i = 0; i < N[client_id]; i++) {
        for (int elem : rv) {
            client_send_to_database_s[i].push_back(elem);
        }
    }
}

// 创建并发送相关随机数
void client::create_and_send_relatived_randomness(int L, int R) {
    int eta = ceil(R / (N[client_id] - 1.0));
    vector<int> random_vector;
    if (state == "normal") {
        random_vector = generate_random_vector(L, R);
        for (int elem : random_vector) {
            client_send_to_client.push_back(elem);
        }
    } else if (state == "special") {
        int initial = L - M + 1;
        random_vector = vector<int>(R, initial);
        for (int i = 0; i < R; i++) {
            for (auto& recv : client_recv_from_client) {
                random_vector[i] = (random_vector[i] - recv[i]) % L;
            }
        }
    }

    int counter = 0;
    vector<vector<int>> rv(N[client_id]);
    for (int j = 0; j < eta; j++) {
        for (int i = 0; i < N[client_id]; i++) {
            int random = 0;
            if (i != 0){
                random = random_vector[counter];
                counter++;
            }
            rv[i].push_back(random);
            if(counter==R)break;
        }
        if(counter==R)break;
    }

    for (int i = 0; i < N[client_id]; i++) {
        for (int elem : rv[i]) {
            client_send_to_database_t[i].push_back(elem);
        }
    }
}
