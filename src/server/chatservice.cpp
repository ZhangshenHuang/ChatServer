#include "chatservice.hpp"
#include "public.hpp"
#include "json.hpp"
#include <muduo/base/Logging.h>
using json = nlohmann::json;

// 获取单例对象的接口函数
ChatService* ChatService::instance() {
    static ChatService service;
    return &service;
}

// 注册消息以及对应的回调业务
ChatService::ChatService() {
    // 用户基本业务管理相关事件处理回调注册
    _msgHandlerMap.insert({LOGIN_MSG,bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({LOGINOUT_MSG, bind(&ChatService::loginout, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG,bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, bind(&ChatService::oneChat, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_FRIEND_MSG, bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 群组业务管理相关事件处理回调注册
    _msgHandlerMap.insert({CREATE_GROUP_MSG, bind(&ChatService::createGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({ADD_GROUP_MSG, bind(&ChatService::addGroup, this, _1, _2, _3)});
    _msgHandlerMap.insert({GROUP_CHAT_MSG, bind(&ChatService::groupChat, this, _1, _2, _3)});

    // 连接redis服务器
    if (_redis.connect()) {
        // 设置上报消息的回调
        _redis.init_notify_handler(bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

// 服务器异常 业务重置方法
void ChatService::reset() {
    // 把online状态的用户全部设置成offline
    _userModel.resetState();
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& conn) {
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it) {
            if (it->second == conn) {
                // 从记录连接客户端的map表中删除连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    // 用户注销 相当于下线 在redis中取消订阅通道
    _redis.unsubscribe(user.getId());

    // 更新用户在数据库中的状态
    if (user.getId() != -1) {
        // 确实在数据库中找到了该异常退出的用户
        user.setState("offline");
        _userModel.updateState(user);
    }
}

// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid) {
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end()) {
        // 返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr& conn, json& js, Timestamp time) {
            LOG_ERROR << "msgid: " << msgid << " can not find handler!";
        };
    } else {
        return _msgHandlerMap[msgid];
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);

    if (state) {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    } else {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

// 处理登录业务 id pwd
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int id = js["id"].get<int>();
    string pwd = js["password"];

    // 根据id查询数据库数据
    User user = _userModel.query(id);
    // 判断是否查询到了 如果没有查询到 那么user.getId() = -1
    if (user.getId() == id && user.getPwd() == pwd) {
        if (user.getState() == "online") {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "This account is using, please use another one!";
            conn->send(response.dump()); 
        } else {
            // 登录成功 记录用户连接信息 析构函数释放锁，无需手动释放
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id, conn});
            }

            // id用户登录成功后 向redis订阅channel(id)
            _redis.subscribe(id);

            // 登录成功 更新用户状态信息
            user.setState("online");
            _userModel.updateState(user);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询该用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty()) {
                response["offlinemsg"] = vec;
                // 读取完成后删除所有离线消息
                _offlineMsgModel.remove(id);
            }

            // 查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if (!userVec.empty()) {
                vector<string> vec2;
                for (User& user : userVec) {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            // 查询用户的群组信息
            vector<Group> groupuserVec = _groupModel.queryGroups(id);
            if (!groupuserVec.empty()) {
                vector<string> groupV;
                for (Group& group : groupuserVec) {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    vector<string> userV;
                    for (GroupUser& user : group.getUsers()) {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.push_back(js.dump());
                    }
                    grpjson["users"] = userV;
                    groupV.push_back(grpjson.dump());
                }
                response["groups"] = groupV;
            }

            conn->send(response.dump());
        }
    } else {
        // 用户不存在或密码错误
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "id or password is invalid!";
        conn->send(response.dump());
    }
}

// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end()) {
            _userConnMap.erase(it);
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid);

    // 更新用户的状态信息
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int toid = js["toid"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end()) {
            // toid在线，转发消息 服务器主动推送消息给toid用户
            // 服务器端的Tcp连接指针调用发送函数
            // 因为这个连接是专为此客户端建立的，所以调用send即向此目标客户端发送信息
            it->second->send(js.dump());
            return;
        }
    }

    // 在数据库中查询toid是否在线(可能在另一台服务器上登录了)
    User user = _userModel.query(toid);
    if (user.getState() == "online") {
        _redis.publish(toid, js.dump());
        return;
    }

    // toid不在线 存储离线消息
    _offlineMsgModel.insert(toid, js.dump());
}

// 添加好友业务 msgid userid friendid
void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    _friendModel.insert(userid, friendid);
}

// 创建群组业务 同时创建者也要加入组 
void ChatService::createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

    // 存储新创建的群组信息 构建allgroup表
    Group group(-1, name, desc);
    if (_groupModel.createGroup(group)) {
        // 存储群组创建人信息 构建groupuser表
        _groupModel.addGroup(userid, group.getId(), "creator");
    }
}

// 加入群组业务 ChatService::addGroup和_groupModel.addGroup是两个不同的函数 要注意！！
void ChatService::addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time) {
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);

    lock_guard<mutex> lock(_connMutex);
    {
        for (int id : useridVec) {
            auto it = _userConnMap.find(id);
            if (it != _userConnMap.end()) {
                // 转发群消息
                it->second->send(js.dump());
            } else {
                // 查询toid是否在线
                User user = _userModel.query(id);
                if (user.getState() == "online") {
                    _redis.publish(id, js.dump());
                } else {
                    // 存储离线群消息
                    _offlineMsgModel.insert(id, js.dump());
                }
            }
        }
    }
}

// 从redis消息队列中获取订阅消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg) {
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end()) {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}