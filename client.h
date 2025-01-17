#ifndef CLIENT_H
#define CLIENT_H
#include <vector>
#include "public_function.h"

class client {
public:
    int client_id;  // 客户端ID
    std::string state;  // 客户端状态
    std::vector<std::vector<int>> client_send_to_database_s;  // 向数据库发送的随机数据（S）
    std::vector<std::vector<int>> client_send_to_database_t;  // 向数据库发送的随机数据（T）
    std::vector<int> client_send_to_client;  // 向其他客户端发送的数据
    std::vector<std::vector<int>> client_recv_from_client;  // 接收其他客户端的数据

    // 构造函数，初始化客户端状态和ID
    client(std::string state, int client_id, vector<int> N);

    // 创建并发送本地随机数
    void create_and_send_local_randomness(int L, int R, vector<int> N);

    // 创建并发送相对随机数
    void create_and_send_relatived_randomness(int L, int R, int M, vector<int> N);
};

#endif // CLIENT_H
