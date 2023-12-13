#include "data.h"
#include <QMessageBox>
#include <unistd.h>

using namespace std;

Data& Data::operator=(Data& c1){
    x_pos = c1.x_pos;
    y_pos = c1.y_pos;

    return *this;
}

Data& Data::operator=(Data&& c1){
    x_pos = std::move(c1.x_pos);
    y_pos = std::move(c1.y_pos);

    return *this;
}


bool Data::loginSQL(void){
    if(_status == true){
        qDebug() << "尝试登录: 处于登录状态";
        return true;
    }

    unsigned timeout = 0;
    qDebug() << "尝试登录: timeout == " << timeout;

    while(_status != true && timeout<10){
        // 自己修改 Mysql 的连接，这个我真没辙
        db.setHostName("192.168.110.251");
        db.setPort(3308);
        db.setUserName("marathon");
        db.setPassword("12345678n");
        db.setDatabaseName("marathon");

        qDebug() << "准备登录";

        try{
            if (!db.open()) {
                qDebug() << "连接数据库失败：" << db.lastError().text();
            } else {
                qDebug() << "连接数据库成功";
                this->_status = true;
                return true;
            }
        }
        catch(exception e){
            qDebug() << "捕获异常: " << e.what();
        }
        catch(...){
            qDebug() << "捕获异常: Unknown\n";
        }

        qDebug() << "登录刷新次数: " << ++timeout;
        sleep(20);
    }

    this->_status = false;
    return false;
}

void Data::logoutSQL(void){
    qDebug() << "尝试退出登录";

    if (this->_status == 1) {
        this->_status = 0;
        db.close();
    }
}

void Data::updateSQL(int x, int y, bool s){
    QSqlQuery query(db);
    query.prepare("SELECT status FROM booksale");

    if (!query.exec()) {
        qDebug() << "查询执行失败：" << query.lastError().text();

        throw runtime_error("SELECT失败");
    } else {
        qDebug() << "查询执行成功";

        auto last_query = query.next();
        unsigned st = 0;

        if(last_query){
            st = stoi(query.value(0).toString().toStdString());
        }

        if(st == 1 && s == true){
            QMessageBox message(QMessageBox::NoIcon, "Error", "请等待上一次预约结束\t");
            message.exec();
            throw runtime_error("重复预约提醒");
        }
        else{
            // 判断这个表是不是空表
            if(!last_query){
                query.prepare("INSERT INTO booksale (x_pos, y_pos, status) VALUES (:x_pos, :y_pos, :status)");

                query.bindValue(":x_pos", x);
                query.bindValue(":y_pos",y);
                query.bindValue(":status",s);

                if (!query.exec()) {
                    qDebug() << "查询执行失败：" << query.lastError().text();
                                                           throw runtime_error("INSERT失败");
                } else {
                    qDebug() << "查询执行成功";
                }
            }
            else
            {
                query.prepare("UPDATE booksale SET x_pos = :x_pos, y_pos = :y_pos, status = :status");

                query.bindValue(":x_pos", x);
                query.bindValue(":y_pos",y);
                query.bindValue(":status",s);

                if (!query.exec()) {
                    qDebug() << "查询执行失败：" << query.lastError().text();
                                                           throw runtime_error("UPDATE失败");
                } else {
                    qDebug() << "查询执行成功";
                }
            }
        }
    }
}

void Data::updateSQL2(int x, int y, vector<Book> books, bool s){
    QSqlQuery query(db);
    query.prepare("SELECT status FROM bookpurchase");

    if (!query.exec()) {
        qDebug() << "查询执行失败：" << query.lastError().text();
        throw runtime_error("SELECT失败");
    } else {
        qDebug() << "查询执行成功";

        auto last_query = query.next();
        unsigned st = 0;

        if(last_query){
            st = stoi(query.value(0).toString().toStdString());
        }

        if(st == 1 && s == true){
            QMessageBox message(QMessageBox::NoIcon, "Error", "请等待上一次预约结束\t");
            message.exec();
            throw runtime_error("重复预约提醒");
        }
        else{

            for(auto &i: books){
                query.prepare("INSERT INTO bookpurchase (x_pos, y_pos, isbn, status) VALUES (:x_pos, :y_pos, :isbn, :status)");

                query.bindValue(":x_pos", x);
                query.bindValue(":y_pos",y);
                query.bindValue(":isbn",i.getIsbn());
                query.bindValue(":status",s);

                if (!query.exec()) {
                    qDebug() << "查询执行失败：" << query.lastError().text();
                    throw runtime_error("INSERT失败");
                } else {
                    qDebug() << "查询执行成功";
                }
            }
        }
    }
}

void Data::deleteAll(std::string database){
    QSqlQuery query(db);
    query.prepare(("DELETE FROM " + database + " WHERE status > -1").c_str());

    if (!query.exec()) {
        qDebug() << "删除执行失败：" << query.lastError().text();
        throw runtime_error("SELECT失败");
    } else {
        qDebug() << "删除执行成功";
    }
}

int Data::statusSQL(string database){
    QSqlQuery query(db);
    query.prepare(("SELECT status FROM " + database).c_str());

    if (!query.exec()) {
        qDebug() << "查询执行失败：" << query.lastError().text();
        throw runtime_error("SELECT失败");
    } else {
        qDebug() << "查询执行成功";
    }

    if(query.next()){
        int result = stoi(query.value(0).toString().toStdString());
        return result;
    }
    else{
        qDebug() << "没有数据捏";
    }
    return 0;
}

void Data::getBookSQL(Book*** base, unsigned count, set<unsigned> own, unsigned type){
    QSqlQuery query(db);
    query.prepare("SELECT * FROM bookinfo AS t1 JOIN (SELECT ROUND(RAND()*(SELECT MAX(id) FROM bookinfo)) AS id) AS t2 WHERE t1.id>=t2.id && (type = :type) ORDER BY t1.id LIMIT 1;");

    query.bindValue(":type",type);

    Book **ba = *base;
    unsigned pos = 0,timeout = 0;

    while(pos<count && timeout < count*3){
        if (!query.exec()) {
            qDebug() << "随机选择执行失败：" << query.lastError().text();
            throw runtime_error("SELECT失败");
        } else {
            qDebug() << "随机选择执行成功";
        }

        ++timeout;

        if(query.next()){
            string bookname = query.value(0).toString().toStdString();

            Type tp;
            switch(stoi(query.value(1).toString().toStdString())){
            case 0:
                tp = Science_fiction;
                break;
            case 1:
                tp = Romance_fiction;
                break;
            case 2:
                tp = Technology;
                break;
            case 3:
                tp = Magazine;
                break;
            default:
                tp = None;
                break;
            }

            long long isbn = stoi(query.value(2).toString().toStdString());
            unsigned count = stoi(query.value(3).toString().toStdString());
            unsigned id = stoi(query.value(4).toString().toStdString());

            // 如果他没有在 own 中，那么我们就把它加入，并且将数据写入base
            if(find(own.begin(),own.end(),id) == own.end()){
                own.insert(id);
                (ba[pos++])->setConfig(bookname,isbn,count,tp, id);
            }
        }
        else
        {
            qDebug()<<"没有查询导数据\n";
        }
    }

    while(pos<count){
        string bookname = "查询失败";
        Type tp = None;
        long long isbn = 0;
        unsigned count = 0;
        unsigned id = (unsigned)-1;

        (ba[pos++])->setConfig(bookname,isbn,count,tp, id);
    }
}
