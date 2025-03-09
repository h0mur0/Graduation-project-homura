#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <memory>  // 引入智能指针头文件
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
int K = 0; // 最终的Sk列表长度
vector<client> clients;  // 客户端列表
vector<database> databases;  // 数据库列表
shared_ptr<leader> ld;  // 领导者智能指针
shared_ptr<client> special_client;  // 特殊客户端智能指针
int R;  // 领导者的元素数量
int L;  // 素数 L
int c;  // 随机系数
shared_ptr<channel> chan;  // 信道智能指针

// 初始化
void initial() {
    L = select_L(M);  // 选择一个大于 M 的素数 L
    int leader_id = select_leader(P, N, M);  // 选择领导者
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
    parse_args(argc,argv,M,fileNames,N);
    encode(fileNames, Sk, data2Sk, P, K);
    initial();  // 初始化
    // pre_cuckoo_hash();
    int max_Ni = *std::max_element(N.begin(), N.end());
    vector<int> dataset = ld -> P_leader;
    int size_per_group = dataset.size() / (max_Ni - 1);
    int size_bucket = 1.5 * size_per_group;
    CuckooHashTableProducer cuckoo_hash_table_producer(size_bucket, 2 * size_bucket);
    for (const auto& element : Sk) {
        cuckoo_hash_table_producer.insert(element);
    }
    vector<CuckooHashTableConsumer> hash_tables_for_leader;
    for (int i = 0; i < max_Ni; ++i) {
        hash_tables_for_leader.push_back(CuckooHashTableConsumer(size_bucket,5)); // 创建并存储哈希表实例

        for (int j = i * size_per_group; j < (i + 1) * size_per_group && j < dataset.size(); ++j) {
            hash_tables_for_leader[i].insert(dataset[j]);
        }
    }
    // for (int i = 0; i < max_Ni; ++i) {
    //     std::cout << "HashTable " << i << ":\n";
    //     hash_tables_for_leader[i].display();
    // }
    vector<CuckooHashTableProducer> hash_tables_for_clients;   
    for (int i = 0; i < P.size(); i++) {
        hash_tables_for_clients.push_back(CuckooHashTableProducer(size_bucket, 2 * size_bucket));
        if (i == ld -> leader_id){
            continue;
        }
        for (const auto& element : P[i]) {
            hash_tables_for_clients[i].insert(element);
        }
    }
    // for (int i = 0; i < max_Ni; ++i) {
    //     std::cout << "HashTable " << i << ":\n";
    //     hash_tables_for_clients[i].display();
    // }
    vector<int> full_intersection;
    for (int i=0;i<size_bucket;i++){
        for (auto &client : clients){
            client.client_recv_from_client = vector<vector<int>>();
            client.client_send_to_client = vector<int>();
            client.client_send_to_database_s = vector<vector<int>>(N[client.client_id], vector<int>());
            client.client_send_to_database_t = vector<vector<int>>(N[client.client_id], vector<int>());
        }
        special_client->client_recv_from_client = vector<vector<int>>();
        special_client->client_send_to_client = vector<int>();
        special_client->client_send_to_database_s = vector<vector<int>>(N[special_client->client_id], vector<int>());
        special_client->client_send_to_database_t = vector<vector<int>>(N[special_client->client_id], vector<int>());
        ld -> leader_recv = vector<vector<vector<int>>>(M);
        ld -> leader_send = vector<vector<vector<vector<int>>>>(M);
        for (int i = 0; i < M; ++i) {
            ld -> leader_send[i].resize(N[i]); 
            ld -> leader_recv[i].resize(N[i]);
        }
        for (auto& database : databases){
            database.database_recv_from_client_s.clear();
            database.database_recv_from_client_t.clear();
            database.database_recv_from_leader.clear();
            database.database_send.clear();
        }
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
        R = new_P_leader.size();
        ld -> P_leader = new_P_leader;
        //更新client元素
        vector<vector<int>> client_recode;
        for (const auto& instance : hash_tables_for_clients){
            client_recode.push_back(instance.table[i]);
        }
        MappingResult map_result2 = mapVectors(full,client_recode);
        vector<vector<int>> new_P_client = map_result2.mappedVector;
        for (auto& database : databases){
            database.P_client = new_P_client[database.client_id];
        }
        //更新K
        K = map_result.reverseMap.size();
        map<int, vector<tuple<int, int, int>>> element_to_position = query();  // 查询阶段
        create_randomness();  // 生成并发送随机数
        reply();  // 数据库生成并发送回复
        vector<int> intersection = get_intersection(element_to_position);  // 获取交集
        if (intersection.size()!=0){
            vector<int> original_intersection = reverseMapVectors(intersection,map_result.reverseMap);
            for (const auto element : original_intersection){
                full_intersection.push_back(element);
            }
        }
        
    }
    
    vector<string> intersection_string;
    decode(full_intersection,data2Sk,intersection_string);
    // 打印交集结果
    cout << "intersection is:";
    for (string elem : intersection_string) {
        cout << elem << endl;
    }
    cout << endl;

    return 0;
}
