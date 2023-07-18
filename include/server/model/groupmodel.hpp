#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"

// 维护群组信息的操作接口方法
class GroupModel {
public:
    // 创建群组
    bool createGroup(Group& group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    // 根据指定的groupid查询群组用户id列表(除自己的)，主要用户给除自己以外的其他组员发送消息
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif