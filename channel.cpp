#include <vector>
#include <iostream>
#include <type_traits>
#include "channel.h"

using namespace std;
extern int com_bit;
template <typename T>
size_t countElements(const T& container) {
    if constexpr (std::is_arithmetic_v<typename T::value_type>) {
        // 如果是一维向量，直接返回其大小
        return container.size();
    } else {
        // 如果是多维向量，递归统计内部每一部分的大小
        size_t count = 0;
        for (const auto& sub : container) {
            count += countElements(sub);
        }
        return count;
    }
}

// 构造函数实现
channel::channel() {}

// leader 到 databases 的数据传输
void channel::leader_to_databases(leader& ld, std::vector<database>& dbs) {
    auto query = ld.leader_send;
    for (auto& db : dbs) {
        int client_id = db.client_id;
        int database_id = db.database_id;
        db.database_recv_from_leader = query[client_id][database_id];
        com_bit += countElements(query[client_id][database_id]);
    }
}

// client 到 databases 的数据传输
void channel::client_to_databases(client& cl, std::vector<database>& dbs) {
    auto rvs = cl.client_send_to_database_s;
    auto rvt = cl.client_send_to_database_t;
    for (auto& db : dbs) {
        if (cl.client_id != db.client_id) {
            continue;
        }
        int database_id = db.database_id;
        db.database_recv_from_client_s = rvs[database_id];
        com_bit += countElements(rvs[database_id]);
        db.database_recv_from_client_t = rvt[database_id];
        com_bit += countElements(rvt[database_id]);
    }
}

// client 之间的数据传输
void channel::client_to_client(client& client_send, client& client_recv) {
    client_recv.client_recv_from_client.push_back(client_send.client_send_to_client);
    com_bit += countElements(client_send.client_send_to_client);
}

// database 到 leader 的数据传输
void channel::database_to_leader(database& db, leader& ld) {
    int client_id = db.client_id;
    int database_id = db.database_id;
    ld.leader_recv[client_id][database_id] = db.database_send;
    com_bit += countElements(db.database_send);
}