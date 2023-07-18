#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

// User表的ORM类
/**
 * 在业务层和数据库层之间使用ORM.
 * ORM（对象关系映射）是一种编程技术，用于将数据库中的数据映射到应用程序中的对象，
 * 并提供方便的方法来操作和处理这些对象，从而使开发人员可以使用面向对象的方式进行数据库操作。
*/
class User {
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline") {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }

    void setId(int id) {this->id = id;}
    void setName(string name) {this->name = name;}
    void setPwd(string pwd) {this->password = pwd;}
    void setState(string state) {this->state = state;}

    int getId() {return this->id;}
    string getName() {return this->name;}
    string getPwd() {return this->password;}
    string getState() {return this->state;}

protected:
    int id;
    string name;
    string password;
    string state;
};

#endif