#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <memory>  // 引入智能指针头文件
#include <fstream>
#include <chrono> // 包含计时功能
#include <thread>
#include <mutex>
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
map<string, int> data2Sk; // 数据到Sk的映射
vector<vector<int>> P(fileNames.size()); // 编码后的数据
int L;  // 素数 L
int c;  // 随机系数
int sp_id = 0;
int leader_id;
shared_ptr<channel> chan = make_shared<channel>();  // 信道智能指针
int group_count;
int size_per_group;
int size_bucket;
CuckooHashTableProducer cuckoo_hash_table_producer;
vector<CuckooHashTableConsumer> hash_tables_for_leader;
vector<CuckooHashTableProducer> hash_tables_for_clients;   
double cul_time = 0.0;
int com_bit = 0;
int com_round = 0;
mutex mtx;
vector<int> full_intersection;

// 初始化
void initial() {
    L = select_L(M);  // 选择一个大于 M 的素数 L
    leader_id = select_leader(P, N, M);  // 选择领导者
    if (leader_id == M - 1) {
        sp_id = M - 2;  // 特殊客户端 ID
    } else {
        sp_id = M - 1;
    }
    int R = P[leader_id].size();
    group_count = (int)ceil(sqrt(R));
    size_per_group = (R + group_count - 1) / group_count;
    size_bucket = 1.5 * size_per_group;
    cuckoo_hash_table_producer = CuckooHashTableProducer(size_bucket, 2 * size_bucket);
}

void pre_cuckoo_hash(){
    vector<int> dataset = P[leader_id];
    for (const auto& element : Sk) {
        cuckoo_hash_table_producer.insert(element);
    }

    for (int i = 0; i < group_count; ++i) {
        hash_tables_for_leader.push_back(CuckooHashTableConsumer(size_bucket,5)); // 创建并存储哈希表实例

        for (int j = i * size_per_group; j < (i + 1) * size_per_group && j < dataset.size(); ++j) {
            hash_tables_for_leader[i].insert(dataset[j]);
        }
    }

    for (int i = 0; i < P.size(); i++) {
        hash_tables_for_clients.push_back(CuckooHashTableProducer(size_bucket, 2 * size_bucket));
        if (i == leader_id){
            continue;
        }
        for (const auto& element : P[i]) {
            hash_tables_for_clients[i].insert(element);
        }
    }
}

void get_data(int i, int& K, vector<int>& P_leader, vector<vector<int>>& data, vector<int>& reverse_map){
    vector<int> full = cuckoo_hash_table_producer.table[i];
    vector<vector<int>> leader_recode;
    vector<int> tmp;
    //更新leader元素
    for (const auto& instance : hash_tables_for_leader){
        if (instance.table[i]!=-1){
            tmp.push_back(instance.table[i]);
        }
    }
    leader_recode.push_back(tmp);
    MappingResult map_result = mapVectors(full,leader_recode);
    vector<int> new_P_leader = map_result.mappedVector[0];
    // 更新R
    int R = new_P_leader.size();
    P_leader = new_P_leader;
    //更新client元素
    vector<vector<int>> client_recode;
    for (const auto& instance : hash_tables_for_clients){
        client_recode.push_back(instance.table[i]);
    }
    MappingResult map_result2 = mapVectors(full,client_recode);
    data = map_result2.mappedVector;
    K = map_result.reverseMap.size();
    reverse_map = map_result.reverseMap;
}

void create_part_classes(int& R, vector<int> P_leader, vector<vector<int>> data,shared_ptr<leader>& ld,vector<client>& clients,vector<database>& databases,shared_ptr<client>& special_client){
    clients.clear();
    databases.clear();
    // 创建客户端、领导者和数据库
    for (int i = 0; i < M; i++) {
        if (i == leader_id) {
            ld = make_shared<leader>(P_leader, i, M, N);  // 创建领导者
            R = P_leader.size();  // 领导者元素个数
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
                database one_database(data[i], i, j);  // 创建数据库
                databases.push_back(one_database);
            }
        }
    }

}

// 查询阶段
map<int, vector<tuple<int, int, int>>> query(shared_ptr<leader>& ld,vector<database>& databases, int K) {
    map<int, vector<tuple<int, int, int>>> element_to_position;
    element_to_position = ld->create_and_send_query(L,M,N,K);  // 领导者生成查询并发送
    chan->leader_to_databases(*ld, databases);  // 领导者将查询传递给数据库
    return element_to_position;
}

// 生成随机性并发送给数据库
void create_randomness(int R, vector<client>& clients, shared_ptr<client>& special_client, vector<database>& databases) {
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
void reply(int K, int R, shared_ptr<leader>& ld, vector<database>& databases) {
    for (auto& db : databases) {
        vector<int> X = db.create_incidence_vector(K);  // 创建数据库的关联向量
        db.create_and_send_reply(X, c, L, R, N);  // 生成并发送回复
        chan->database_to_leader(db, *ld);  // 数据库将回复发送给领导者
    }
}

// 获取交集
vector<int> get_intersection(const map<int, vector<tuple<int, int, int>>>& element_to_position, shared_ptr<leader>& ld) {
    return ld->calculate_intersection(element_to_position, L);  // 计算交集
}

void process_bucket(int i) {
    // 原循环体内的所有局部变量保持为线程局部
    std::vector<client> clients;
    std::vector<database> databases;
    std::shared_ptr<leader> ld;
    std::shared_ptr<client> special_client;
    std::vector<std::vector<int>> data;
    std::vector<int> P_leader;
    std::vector<int> reverse_map;
    int K, R;

    get_data(i, K, P_leader, data, reverse_map);
    create_part_classes(R, P_leader, data, ld, clients, databases, special_client);
    auto element_to_position = query(ld, databases, K);
    create_randomness(R, clients, special_client, databases);
    reply(K, R, ld, databases);
    std::vector<int> intersection = get_intersection(element_to_position, ld);

    if (!intersection.empty()) {
        auto original_intersection = reverseMapVectors(intersection, reverse_map);
        
        // 互斥锁保护共享资源
        std::lock_guard<std::mutex> lock(mtx);
        full_intersection.insert(full_intersection.end(),
                               original_intersection.begin(),
                               original_intersection.end());
    }
}


// 主函数
int main(int argc, char* argv[]) {
    ofstream outFile("output_PBC.txt",ios::app);
    outFile << "------------------------- "<< endl;
    auto st_time_1 = chrono::high_resolution_clock::now();
    parse_args(argc,argv,M,fileNames,N);
    encode(fileNames, Sk, data2Sk, P);
    initial();  // 初始化
    pre_cuckoo_hash();
    //多线程操作
    vector<thread> threads;
    for (int i = 0;i < size_bucket;i++){
        threads.emplace_back(process_bucket, i);
    }   

    for (auto& t : threads) {
        t.join();
    }
    vector<string> intersection_string;
    decode(full_intersection,data2Sk,intersection_string);
    // 打印交集结果
    cout << "intersection is:";
    outFile << "intersection is:";
    for (string elem : intersection_string) {
        cout << elem << endl;
        outFile << elem << endl;
    }
    cout << endl;
    cout << "交集大小：" << intersection_string.size() << endl;
    outFile << "交集大小：" << intersection_string.size() << endl;
    cout << "通信开销：" << com_bit << endl;
    outFile << "通信开销：" << com_bit << endl;
    outFile.close();
    return 0;
}
