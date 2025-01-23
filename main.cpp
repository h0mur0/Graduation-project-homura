#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <map>
#include <memory>  // 引入智能指针头文件
#include <chrono> // 包含计时功能
#include <unordered_map>
#include "client.h"
#include "leader.h"
#include "database.h"
#include "channel.h"
#include "public_function.h"

using namespace std;

int M = 0;  // 参与方数量
vector<string> fileNames;  // 存数据的文件列表
vector<int> N;  // 数据库数量
vector<int> Sk; // 整型列表Sk
unordered_map<string, int> data2Sk; // 数据到Sk的映射
vector<vector<int>> P(fileNames.size()); // 编码后的数据
int K = 0; // 最终的Sk列表长度
vector<client> clients;  // 客户端列表
vector<database> databases;  // 数据库列表
shared_ptr<leader> ld;  // 领导者智能指针
shared_ptr<client> special_client;  // 特殊客户端智能指针
int R;  // 领导者的客户端数量
int L;  // 素数 L
int c;  // 随机系数
shared_ptr<channel> chan;  // 信道智能指针
double cul_time = 0.0;
int com_bit = 0;
int com_round = 0;

// 初始化
void initial() {
    L = select_L(M);  // 选择一个大于 M 的素数 L
    int leader_id = select_leader(P, N, M);  // 选择领导者
    cout << "leader is " << leader_id << endl;
    cout << "Kp的大小：" << P[leader_id].size() << endl;
    ofstream outFile1("output.txt",ios::app);
    outFile1 << "leader is "<<leader_id << endl;
    outFile1 <<"Kp的大小："<<P[leader_id].size() << endl;
    outFile1.close();
    int sp_id = 0;
    if (leader_id == M - 1) {
        sp_id = M - 2;  // 特殊客户端 ID
    } else {
        sp_id = M - 1;
    }

    // 创建客户端、领导者和数据库
    for (int i = 0; i < M; i++) {
        if (i == leader_id) {
            ld = make_shared<leader>(P[i], i, M, N);  // 创建领导者
            R = P[i].size();  // 领导者元素个数
        } else {
            if(i == sp_id){
                // 创建特殊客户端，并将其指针赋给 special_client
                special_client = make_shared<client>("special", i, N);
                //clients.push_back(*special_client);  // 将其副本添加到客户端列表
            }
            else{
                // 创建正常客户端并加入到列表
                client one_client("normal", i,N);  
                clients.push_back(one_client);
            }
            
            // 为每个客户端创建数据库
            for (int j = 0; j < N[i]; j++) {
                database one_database(P[i], i, j);  // 创建数据库
                databases.push_back(one_database);
            }
        }
    }

    // 创建 channel 的智能指针
    chan = make_shared<channel>();
}

// 查询阶段
map<int, vector<tuple<int, int, int>>> query() {
    map<int, vector<tuple<int, int, int>>> element_to_position;
    element_to_position = ld->create_and_send_query(L,M,N,K);  // 领导者生成查询并发送
    chan->leader_to_databases(*ld, databases);  // 领导者将查询传递给数据库
    return element_to_position;
}


// 生成随机性并发送给数据库
void create_randomness() {
    // 正常客户端
    for (auto& cl : clients) {
        int eta = ceil(R / (N[cl.client_id] - 1.0));
        cl.create_and_send_local_randomness(L, R, N);
        cl.create_and_send_relatived_randomness(L, R, M, N);
        chan->client_to_client(cl, *special_client);  // 正常客户端发送给特殊客户端
        chan->client_to_databases(cl, databases);  // 客户端发送给数据库
    }
    //特殊客户端
    client cl = *special_client;
    int eta = ceil(R / (N[cl.client_id] - 1.0));
    cl.create_and_send_local_randomness(L, R, N);
    cl.create_and_send_relatived_randomness(L, R, M, N);
    //chan->client_to_client(cl, *special_client);  // 正常客户端发送给特殊客户端
    chan->client_to_databases(cl, databases);  // 客户端发送给数据库
    c = rand() % (L - 1) + 1;  // 随机生成系数 c
}

// 数据库生成回复并发送给领导者
void reply() {
    for (auto& db : databases) {
        vector<int> X = db.create_incidence_vector(K);  // 创建数据库的关联向量
        db.create_and_send_reply(X, c, L, R, N);  // 生成并发送回复
        chan->database_to_leader(db, *ld);  // 数据库将回复发送给领导者
    }
}

// 获取交集
vector<int> get_intersection(const map<int, vector<tuple<int, int, int>>>& element_to_position) {
    return ld->calculate_intersection(element_to_position, L);  // 计算交集
}

// 主函数
int main(int argc, char* argv[]) {
    ofstream outFile("output.txt",ios::app);
    outFile << "------------------------- "<< endl;
    auto st_time_1 = chrono::high_resolution_clock::now();
    parse_args(argc,argv,M,fileNames,N);
    auto st_time_2 = chrono::high_resolution_clock::now();
    auto duration_2 = chrono::duration_cast<std::chrono::microseconds>(st_time_2 - st_time_1);
    outFile << "时间1：" << duration_2.count() << "微秒" << endl;
    encode(fileNames, Sk, data2Sk, P, K);
    auto st_time_3 = chrono::high_resolution_clock::now();
    auto duration_3 = chrono::duration_cast<std::chrono::microseconds>(st_time_3 - st_time_2);
    outFile << "计算时间：" << duration_3.count() << "微秒" << endl;
    initial();  // 初始化
    auto st_time_4 = chrono::high_resolution_clock::now();
    auto duration_4 = chrono::duration_cast<std::chrono::microseconds>(st_time_4 - st_time_3);
    outFile << "计算时间：" << duration_4.count() << "微秒" << endl;

    map<int, vector<tuple<int, int, int>>> element_to_position = query();  // 查询阶段
    auto st_time_5 = chrono::high_resolution_clock::now();
    auto duration_5 = chrono::duration_cast<std::chrono::microseconds>(st_time_5 - st_time_4);
    outFile << "计算时间：" << duration_4.count() << "微秒" << endl;
    create_randomness();  // 生成并发送随机数
    auto st_time_6 = chrono::high_resolution_clock::now();
    auto duration_6 = chrono::duration_cast<std::chrono::microseconds>(st_time_6 - st_time_5);
    outFile << "计算时间：" << duration_6.count() << "微秒" << endl;
    reply();  // 数据库生成并发送回复
    auto st_time_7 = chrono::high_resolution_clock::now();
    auto duration_7 = chrono::duration_cast<std::chrono::microseconds>(st_time_7 - st_time_6);
    outFile << "计算时间：" << duration_7.count() << "微秒" << endl;
    vector<int> intersection = get_intersection(element_to_position);  // 获取交集
    auto st_time_8 = chrono::high_resolution_clock::now();
    auto duration_8 = chrono::duration_cast<std::chrono::microseconds>(st_time_8 - st_time_7);
    outFile << "计算时间：" << duration_8.count() << "微秒" << endl;
    vector<string> intersection_string;
    decode(intersection,data2Sk,intersection_string);
    auto st_time_9 = chrono::high_resolution_clock::now();
    auto duration_9 = chrono::duration_cast<std::chrono::microseconds>(st_time_9 - st_time_8);
    outFile << "计算时间：" << duration_9.count() << "微秒" << endl;
    // 打印交集结果
    cout << "intersection is:" << endl;
    // outFile << "intersection is: "<< endl;
    for (string elem : intersection_string) {
        cout << elem << endl;
        // outFile << elem << endl;
    }
    cout << endl;
    // outFile << endl;
    auto end_time_1 = chrono::high_resolution_clock::now();
    auto duration_1 = chrono::duration_cast<std::chrono::microseconds>(end_time_1 - st_time_3);
    cout << "intersection is:" << endl;
    cout << "计算时间：" << duration_1.count() << "微秒" << endl;
    cout << "通信开销：" << com_bit << endl;
    outFile << "计算时间：" << duration_1.count() << "微秒" << endl;
    outFile << "通信开销：" << com_bit << endl;
    outFile.close();
    return 0;
}
