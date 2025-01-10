#ifndef CHANNEL_H
#define CHANNEL_H

#include "leader.h"
#include "client.h"
#include "database.h"

class channel {
public:
    // 默认构造函数
    channel();

    // 将 leader 数据发送到数据库
    void leader_to_databases(leader& ld, std::vector<database>& dbs);

    // 将 client 数据发送到数据库
    void client_to_databases(client& cl, std::vector<database>& dbs);

    // 将一个 client 的数据发送到另一个 client
    void client_to_client(client& client_send, client& client_recv);

    // 将数据库数据发送到 leader
    void database_to_leader(database& db, leader& ld);
};

#endif // CHANNEL_H
