#include "purchase.h"
#include "ui_purchase.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace std;

Purchase::Purchase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Purchase),
    skanning(nullptr),
    port(new Data())
{
    skanning = new Book*[4];
    for(int i = 0; i < 4; ++i) {
        skanning[i] = new Book();
    }

    ui->setupUi(this);

    // 复选框配置
    connect(ui->checkBox,&QCheckBox::stateChanged, this, &Purchase::checkAll4State);
    connect(ui->checkBox_2,&QCheckBox::stateChanged, this, &Purchase::checkAll4State);
    connect(ui->checkBox_3,&QCheckBox::stateChanged, this, &Purchase::checkAll4State);
    connect(ui->checkBox_4,&QCheckBox::stateChanged, this, &Purchase::checkAll4State);
    connect(ui->checkBox_5,&QCheckBox::stateChanged, this, &Purchase::allCheck);
    connect(ui->comboBox,&QComboBox::currentIndexChanged, this, &Purchase::reload);

    // 删除按钮
    connect(ui->pushButton, &QPushButton::clicked, this, &Purchase::delBook);
    connect(ui->pushButton_2, &QPushButton::clicked, this, &Purchase::delAllBook);

    // 功能按钮
    connect(ui->pushButton_3, &QPushButton::clicked, this, &Purchase::removeAction);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &Purchase::returnAction);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &Purchase::reload);
    connect(ui->pushButton_6, &QPushButton::clicked, this, &Purchase::checkChcekBox);
    connect(ui->pushButton_7, &QPushButton::clicked, this, &Purchase::purchase);
    connect(ui->pushButton_8, &QPushButton::clicked, this, &Purchase::flush);

    this->reload();

    QStringList list;

    list << "暂无数据" ;

    QStringListModel* model = new QStringListModel(list);

    ui->listView->setModel(model);
    ui->listView->show();

    this->flush();
}

Purchase::~Purchase()
{
    delete ui->listView->model();
    for(int i = 0; i < 4; ++i) {
        delete skanning[i];
    }
    delete [] skanning;
    delete port;
    delete ui;
}

// 返回函数
void Purchase::returnAction(void){
    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    this->close();
}

// 全选函数
void Purchase::allCheck(int state){
    if(state == Qt::Checked){
        ui->checkBox->setCheckState(Qt::Checked);
        ui->checkBox_2->setCheckState(Qt::Checked);
        ui->checkBox_3->setCheckState(Qt::Checked);
        ui->checkBox_4->setCheckState(Qt::Checked);
    }
    else if(state == Qt::Unchecked){
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_3->setCheckState(Qt::Unchecked);
        ui->checkBox_4->setCheckState(Qt::Unchecked);
    }
}

// 添加到候选单
void Purchase::checkChcekBox(void){
    unsigned count = 0;
    if(ui->checkBox->isChecked() && skanning[0]->getID() != (unsigned)-1){
        ++count;
        shopping.push_back(*(skanning[0]));
        ids.insert(skanning[0]->getID());
    }
    if(ui->checkBox_2->isChecked() && skanning[1]->getID() != (unsigned)-1){
        ++count;
        shopping.push_back(*(skanning[1]));
        ids.insert(skanning[1]->getID());
    }
    if(ui->checkBox_3->isChecked() && skanning[2]->getID() != (unsigned)-1){
        ++count;
        shopping.push_back(*(skanning[2]));
        ids.insert(skanning[2]->getID());
    }
    if(ui->checkBox_4->isChecked() && skanning[3]->getID() != (unsigned)-1){
        ++count;
        shopping.push_back(*(skanning[3]));
        ids.insert(skanning[3]->getID());
    }

    ui->checkBox->setCheckState(Qt::Unchecked);
    ui->checkBox_2->setCheckState(Qt::Unchecked);
    ui->checkBox_3->setCheckState(Qt::Unchecked);
    ui->checkBox_4->setCheckState(Qt::Unchecked);

    if(count == 0){
        QMessageBox message(QMessageBox::NoIcon, "Tips", "您还没有要添加进购物车的书哦\t");
        message.exec();
    }

    this->reload();

    // 务必删除旧的
    delete ui->listView->model();

    QStringList list;

    for(auto &i:shopping){
        list << i.toString().c_str();
    }

    QStringListModel* model = new QStringListModel(list);

    ui->listView->setModel(model);
    ui->listView->show();
}

// 识别类型
unsigned Purchase::checkComboBox(void){
    unsigned ret = 0;
    // 科幻，言情，科技，杂志
    if(ui->comboBox->currentText() == "科幻"){
        ret |= 1U<< 0;
    }
    else if(ui->comboBox->currentText() == "言情"){
        ret |= 1U<< 1;
    }
    else if(ui->comboBox->currentText() == "科技"){
        ret |= 1U<< 2;
    }
    else if(ui->comboBox->currentText() == "杂志"){
        ret |= 1U<< 3;
    }
    else{
        qDebug() << "ComboBox类型错误\n";
    }

    return ret;
}

// 配送函数
void Purchase::purchase(void){
    try{
        if(this->shopping.size() == 0)
            throw int(1);

        auto sx = ui->lineEdit_2->text().toStdString();
        auto sy = ui->lineEdit_3->text().toStdString();

        int x_pos = stoi(sx);
        int y_pos = stoi(sy);

        port->loginSQL();

        port->setConfig(x_pos, y_pos);

        if(port->getStatus()){
            try{
                port->updateSQL2(x_pos, y_pos, shopping, true);

                QMessageBox message(QMessageBox::NoIcon, "Tips", "预约成功\t");
                message.exec();
            }
            catch(exception e){
                qDebug() << e.what();
            }
            catch(...){
                qDebug() << "Fatal error: Unknown error.";
            }
        }
        else{
            qDebug() << "数据库未连接\n";

            QMessageBox message(QMessageBox::NoIcon, "Error", "数据库未连接\t");
            message.exec();
        }
    }
    catch(exception e){
        qDebug() << "Fatal error: " << e.what() << "\n";

        QMessageBox message(QMessageBox::NoIcon, "Tips", "地址必须是两个整数哦\t");
        message.exec();
    }
    catch(int){
        qDebug() << "序列为空";

        QMessageBox message(QMessageBox::NoIcon, "Error", "您还没有选书哦");
        message.exec();
    }
    catch(...){
        qDebug() << "Fatal error: Unknown error\n";

        QMessageBox message(QMessageBox::NoIcon, "Error", "Unknown error\t");
        message.exec();
    }

    this->flush();
    port->logoutSQL();
}

// 取消上次预约函数
void Purchase::removeAction(void){
    try{
        int x_pos = 0;
        int y_pos = 0;

        port->loginSQL();

        port->setConfig(x_pos, y_pos);

        if(port->getStatus()){
            try{
                port->deleteAll("bookpurchase");

                QMessageBox message(QMessageBox::NoIcon, "Tips", "取消预约成功\t");
                message.exec();
            }
            catch(exception e){
                qDebug() << e.what();
            }
            catch(...){
                qDebug() << "Fatal error: Unknown error.";
            }
        }
        else{
            qDebug() << "数据库未连接\n";

            QMessageBox message(QMessageBox::NoIcon, "Error", "数据库未连接\t");
            message.exec();
        }
    }
    catch(exception e){
        qDebug() << "Fatal error: " << e.what() << "\n";
    }
    catch(...){
        qDebug() << "Fatal error: Unknown error\n";

        QMessageBox message(QMessageBox::NoIcon, "Error", "Unknown error\t");
        message.exec();
    }

    this->flush();
    port->logoutSQL();
}

// 检测全选框状态函数
void Purchase::checkAll4State(int state){
    int status = -1;
    // 增加了新的勾选
    if(state == Qt::Checked){
        // 如果四个都已经完全勾选，那么status赋值为2表示全选
        if(ui->checkBox->isChecked() && ui->checkBox_2->isChecked() &&
            ui->checkBox_3->isChecked() && ui->checkBox_4->isChecked())
            status = 2;
        else
            status = 1;
    }
    else if(state == Qt::Unchecked){
        // 如果四个都被取消
        if(!(ui->checkBox->isChecked() || ui->checkBox_2->isChecked() ||
             ui->checkBox_3->isChecked() || ui->checkBox_4->isChecked()))
            status = 0;
        else
            status = 1;
    }

    if(status == 0){
        ui->checkBox_5->setCheckState(Qt::Unchecked);
    }
    else if(status == 1){
        ui->checkBox_5->setCheckState(Qt::PartiallyChecked);
    }
    else if(status == 2){
        ui->checkBox_5->setCheckState(Qt::Checked);
    }
    else{
        qDebug() << "勾选框出现逻辑错误\n";

        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_3->setCheckState(Qt::Unchecked);
        ui->checkBox_4->setCheckState(Qt::Unchecked);
        ui->checkBox_5->setCheckState(Qt::Unchecked);
    }
}

// 换一批推荐
void Purchase::reload(void){
    getBook(&skanning, 4);

    // 书名设置
    ui->plainTextEdit->setPlainText(skanning[0]->getBookName());
    ui->plainTextEdit_2->setPlainText(skanning[1]->getBookName());
    ui->plainTextEdit_3->setPlainText(skanning[2]->getBookName());
    ui->plainTextEdit_4->setPlainText(skanning[3]->getBookName());

    if(skanning[0]->getID() == (unsigned)-1){
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox->setEnabled(false);
    }
    else
        ui->checkBox->setEnabled(true);

    if(skanning[1]->getID() == (unsigned)-1){
        ui->checkBox_2->setCheckState(Qt::Unchecked);
        ui->checkBox_2->setEnabled(false);
    }
    else
        ui->checkBox_2->setEnabled(true);

    if(skanning[2]->getID() == (unsigned)-1){
        ui->checkBox_3->setCheckState(Qt::Unchecked);
        ui->checkBox_3->setEnabled(false);
    }
    else
        ui->checkBox_3->setEnabled(true);

    if(skanning[3]->getID() == (unsigned)-1){
        ui->checkBox_4->setCheckState(Qt::Unchecked);
        ui->checkBox_4->setEnabled(false);
    }
    else
        ui->checkBox_4->setEnabled(true);
}

// 获得几本新的书
void Purchase::getBook(Book*** Books,unsigned count){
    Book** bTmp = *Books;

    port->loginSQL();

    int status = port->getStatus();

    if(status != -1){
        unsigned type = checkComboBox();
        // getBookSQL 中没有对数据库的操作
        port->getBookSQL(&bTmp, count, ids, type);
    }
    else{
        qDebug() << "查询失败，请刷新";
    }

    port->logoutSQL();
}

// 删除所有书目
void Purchase::delAllBook(void){
    this->shopping.clear();
    this->ids.clear();

    reload();

    // 务必删除旧的
    delete ui->listView->model();

    QStringList list;
    list << "暂无数据";

    QStringListModel* model = new QStringListModel(list);

    ui->listView->setModel(model);
    ui->listView->show();
}

// 删除指定书目
void Purchase::delBook(void){
    string str = ui->lineEdit->text().toStdString();
    auto bk = find_if(shopping.begin(),shopping.end(),[&](const Book& x){return x.getBookName(0) == str;});

    ids.erase(bk->getID());
    shopping.erase(bk);

    reload();

    // 务必删除旧的
    delete ui->listView->model();

    QStringList list;

    if(shopping.size() == 0)
        list << "暂无数据";
    else
        for(auto &i:shopping){
            list << i.toString().c_str();
        }

    QStringListModel* model = new QStringListModel(list);

    ui->listView->setModel(model);
    ui->listView->show();
}

// 刷新状态函数
void Purchase::flush(void){
    int status = port->getStatus();

    if(status == 0){
        qDebug() << "查询失败，自动刷新一次";
        port->loginSQL();
    }

    status = port->getStatus();

    if(status != 0){
        int status2 = port->statusSQL("bookpurchase");
        if(status2 == 0)
            ui->label_9->setText("无预约");
        else if(status2 == 1)
            ui->label_9->setText("有预约");
        else
            ui->label_9->setText("出错了");
    }
    else{
        qDebug() << "刷新后查询失败";
    }
}


// 更换单个书籍（暂时未启用）
/*
// 换一本推荐1
void Purchase::reloadOnePos(void){
    Book** bTmp = new Book*[1];
    bTmp[0] = new Book();

    getBook(&bTmp, 1);

    delete skanning[0];
    skanning[0] = bTmp[0];

    ui->plainTextEdit->setPlainText(skanning[0]->getBookName());
    delete [] bTmp;

    if(skanning[0]->getID() == (unsigned)-1){
        ui->checkBox->setCheckState(Qt::Unchecked);
        ui->checkBox->setEnabled(false);
    }
    else
        ui->checkBox->setEnabled(true);
}
*/
