#include <vector>
#include <cmath>

#include "database.h"


using namespace std;

// 构造函数实现
database::database(vector<int>& P_client, int client_id, int database_id)
    : P_client(P_client), client_id(client_id), database_id(database_id) {}

// 创建并返回关联向量
vector<int> database::create_incidence_vector() {
    vector<int> X(K, 0);
    for (int k : P_client) {
        X[k] = 1;
    }
    return X;
}

// 创建并发送回复
void database::create_and_send_reply(const vector<int>& X, int c, int L, int R) {
    auto s = database_recv_from_client_s;
    auto t = database_recv_from_client_t;
    int eta = ceil(R / (N[client_id] - 1));
    for (size_t i = 0; i < eta; i++) {
        int reply = c * (dot_product(X, database_recv_from_leader[i]) + s[i] + t[i]) % L;
        database_send.push_back(reply);
    }
}