#ifndef PURCHASE_H
#define PURCHASE_H

#include <QWidget>
#include "data.h"
#include <QMessageBox>
#include <QListView>
#include <QStringListModel>

namespace Ui {
class Purchase;
}

class Purchase : public QWidget
{
    Q_OBJECT

public:
    explicit Purchase(QWidget *parent = nullptr);
    ~Purchase();
    void getBook(Book*** Books, unsigned count);

private:
    Ui::Purchase *ui;
    Data* port;
    Book** skanning;
    std::vector<Book> shopping;
    std::set<unsigned> ids;

private slots:
    // 全选函数
    void allCheck(int state);
    // 添加到候选单
    void checkChcekBox(void);
    // 识别类型
    unsigned checkComboBox(void);
    // 配送函数
    void purchase(void);
    // 换一批推荐
    void reload(void);
    // 删除所有书目
    void delAllBook(void);
    // 删除指定书目
    void delBook(void);
    // 检测全选框状态函数
    void checkAll4State(int state);
    // 取消上次预约
    void removeAction(void);
    // 返回卖书界面
    void returnAction(void);
    // 刷新状态函数
    void flush(void);
};

#endif // PURCHASE_H
