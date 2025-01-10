#ifndef LEADER_H
#define LEADER_H
#include <map>
#include <vector>
#include "public_function.h"
#include "para.h"

class leader {
public:
    std::vector<int> P_leader;
    std::vector<std::vector<std::vector<std::vector<int>>>> leader_send;
    std::vector<std::vector<std::vector<int>>> leader_recv;
    int leader_id;

    // 构造函数声明
    leader(std::vector<int>& P_leader, int leader_id);

    // 成员函数声明
    std::map<int, std::vector<std::tuple<int, int, int>>> create_and_send_query(int L);
    int get_kappa();
    std::vector<std::vector<int>> create_random_vector(int kappa, int L);
    std::map<int, std::vector<std::tuple<int, int, int>>> create_and_query(const std::vector<std::vector<int>>& random_vector, int L);
    std::vector<int> calculate_intersection(const std::map<int, std::vector<std::tuple<int, int, int>>>& element_to_position, int L);
};

#endif // LEADER_H
