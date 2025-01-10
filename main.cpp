#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <map>
#include "client.h"
#include "leader.h"
#include "database.h"
#include "channel.h"
#include "para.h"
#include "public_function.h"

using namespace std;

vector<client> clients;  // 客户端列表
vector<database> databases;  // 数据库列表
leader* ld;  // 领导者
client* special_client;  // 特殊客户端
int R;  // 领导者的客户端数量
int L;  // 素数 L
int c;  // 随机系数
channel* chan;

// 初始化
void initial() {
    L = select_L(M);  // 选择一个大于 M 的素数 L
    int leader_id = select_leader(P, N);  // 选择领导者
    int sp_id = 0;
    if (leader_id == M - 1) {
        sp_id = M - 2;  // 特殊客户端 ID
    } else {
        sp_id = M - 1;
    }

    // 创建客户端、领导者和数据库
    for (int i = 0; i < M; i++) {
        if (i == leader_id) {
            ld = new leader(P[i], i);  // 创建领导者
            R = P[i].size();  // 领导者元素个数
        } else {
            if(i == sp_id){
                special_client = new client("special", i); // 创建特殊客户端
                clients.push_back(*special_client);
            }
            else{
                client one_client("normal", i);  // 创建正常客户端
                clients.push_back(one_client);
            }
            

            // 为每个客户端创建数据库
            for (int j = 0; j < N[i]; j++) {
                database one_database(P[i], i, j);  // 创建数据库
                databases.push_back(one_database);
            }
        }
    }
}

// 查询阶段
map<int, vector<tuple<int, int, int>>> query() {
    map<int, vector<tuple<int, int, int>>> element_to_position;
    element_to_position = ld->create_and_send_query(L);  // 领导者生成查询并发送
    chan->leader_to_databases(*ld, databases);  // 领导者将查询传递给数据库
    return element_to_position;
}


// 生成随机性并发送给数据库
void create_randomness() {
    for (auto& cl : clients) {
        int eta = ceil(R / (N[cl.client_id] - 1.0));
        cl.create_and_send_local_randomness(L, R);
        cl.create_and_send_relatived_randomness(L, R);
        if (cl.state == "normal") {
            chan->client_to_client(cl, *special_client);  // 正常客户端发送给特殊客户端
        }
        chan->client_to_databases(cl, databases);  // 客户端发送给数据库
    }
    c = rand() % (L - 1) + 1;  // 随机生成系数 c
}

// 数据库生成回复并发送给领导者
void reply() {
    for (auto& db : databases) {
        vector<int> X = db.create_incidence_vector();  // 创建数据库的关联向量
        db.create_and_send_reply(X, c, L, R);  // 生成并发送回复
        chan->database_to_leader(db, *ld);  // 数据库将回复发送给领导者
    }
}

// 获取交集
vector<int> get_intersection(const map<int, vector<tuple<int, int, int>>>& element_to_position) {
    return ld->calculate_intersection(element_to_position, L);  // 计算交集
}

// 主函数
int main() {
    initial();  // 初始化

    map<int, vector<tuple<int, int, int>>> element_to_position = query();  // 查询阶段
    create_randomness();  // 生成并发送随机性
    reply();  // 数据库生成并发送回复
    vector<int> intersection = get_intersection(element_to_position);  // 获取交集

    // 打印交集结果
    cout << "intersection is:";
    for (int elem : intersection) {
        cout << elem << " ";
    }
    cout << endl;

    return 0;
}
