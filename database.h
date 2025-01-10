#ifndef DATABASE_H
#define DATABASE_H

#include <vector>

#include "public_function.h"
#include "para.h"

class database {
public:
    std::vector<int> P_client;  // 客户端列表
    int client_id;  // 数据库的客户端ID
    int database_id;  // 数据库ID
    std::vector<int> database_send;  // 数据库发送的数据
    std::vector<int> database_recv_from_client_s;  // 来自客户端S的接收数据
    std::vector<int> database_recv_from_client_t;  // 来自客户端T的接收数据
    std::vector<std::vector<int>> database_recv_from_leader;  // 来自leader的接收数据

    // 构造函数，初始化数据库相关属性
    database(std::vector<int>& P_client, int client_id, int database_id);

    // 创建和返回一个与客户端列表相关的邻接向量
    std::vector<int> create_incidence_vector();

    // 创建并发送回复
    void create_and_send_reply(const std::vector<int>& X, int c, int L, int R);
};

#endif // DATABASE_H
