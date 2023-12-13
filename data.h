#ifndef DATA_H
#define DATA_H

// ISO C++
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include <utility>
#include <fstream>
#include <sstream>
#include <bitset>
#include <exception>

// Qt C++
#include <QFile>
#include <QDir>

// Json C++
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

// Mysql C++
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

// 类型定义
typedef enum{
    // 科幻小说
    Science_fiction= 0,
    // 言情小说
    Romance_fiction,
    // 科技书籍
    Technology,
    // 杂志
    Magazine,
    // 默认值
    None
} Type;

class Book{
public:
    Book():book_name(), isbn(0), num(0), ty(None), id(-1){}
    Book(std::string nm, long long data, size_t n, Type t, unsigned i):book_name(nm),isbn(data), num(n), ty(t), id(i){}
    Book(const char* nm, long long data, size_t n, Type t, unsigned i):book_name(nm),isbn(data), num(n), ty(t), id(i){}

    void setConfig(std::string nm, long long data, size_t n, Type t, unsigned i){
        book_name = nm;
        isbn = data;
        num = n;
        ty = t;
        id = i;
    }

    void setConfig(const char* nm, long long data, size_t n, Type t, unsigned i){
        book_name = nm;
        isbn = data;
        num = n;
        ty = t;
        id = i;
    }

    std::string toString(void){
        std::string str("书名: ");
        str += book_name;
        str += "\t库存: ";
        str += std::to_string(num);

        return str;
    }

    const long long getIsbn(void) const{
        return isbn;
    }

    const Type getType(void){
        return ty;
    }

    const char* getBookName(void) const{
        return book_name.c_str();
    }

    // 升维重载
    std::string getBookName(int) const{
        return book_name;
    }

    const size_t getNum(void) const{
        return num;
    }

    const unsigned getID(void) const{
        return id;
    }

private:
    // 书名
    std::string book_name;
    // isbn 编号
    long long isbn;
    // 剩余库存量
    size_t num;
    // 书籍类型
    Type ty;
    // ID
    unsigned id;
};

class Data{
public:
    // 构造和析构
    Data():x_pos(0),y_pos(0),_status(0){
        this->db = QSqlDatabase::addDatabase("QODBC");
    }

    Data(int x,int y):x_pos(x),y_pos(y),_status(0)
    {
        this->db = QSqlDatabase::addDatabase("QODBC");
    }
    ~Data(){
        this->logoutSQL();
    }

    // 运算符重载
    Data& operator=(Data&);
    Data& operator=(Data&&);

    // api
    bool getStatus(void){
        return _status;
    }

    void setConfig(const unsigned x, const unsigned y){
        x_pos = x;
        y_pos = y;
    }

    // 登录函数
    bool loginSQL(void);
    // 登出函数
    void logoutSQL(void);
    // 删除所有数据
    void deleteAll(std::string database);
    // 上传 booksale 表
    void updateSQL(int x, int y, bool s);
    // 上传 bookpurchase 表
    void updateSQL2(int x, int y, std::vector<Book> books, bool s);
    // 查询数据库内成员 status 状态
    int statusSQL(std::string database);
    // 更新书目函数
    void getBookSQL(Book*** base, unsigned count, std::set<unsigned> own, unsigned type);
private:
    // 私有成员
    QSqlDatabase db;
    bool _status;
    int x_pos,y_pos;
};

#endif
