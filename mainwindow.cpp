#include "mainwindow.h"
#include "./ui_mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    port(new Data())
{
    ui->setupUi(this);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::purchaseBook);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::saleBook);
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::flush);
    connect(ui->pushButton_5, &QPushButton::clicked, this, &MainWindow::removeAction);

    flush();
}

MainWindow::~MainWindow()
{
    delete port;
    delete ui;
}

void MainWindow::showNewMainWindow()
{
    MainWindow* newMainWindow = new MainWindow();
    newMainWindow->show();
}

void MainWindow::saleBook(void){
    try{
        auto sx = ui->lineEdit->text().toStdString();
        auto sy = ui->lineEdit_2->text().toStdString();

        int x_pos = stoi(sx);
        int y_pos = stoi(sy);

        port->loginSQL();

        port->setConfig(x_pos, y_pos);

        if(port->getStatus()){
            try{
                port->updateSQL(x_pos,y_pos,true);

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
    catch(...){
        qDebug() << "Fatal error: Unknown error\n";

        QMessageBox message(QMessageBox::NoIcon, "Error", "Unknown error\t");
        message.exec();
    }

    this->flush();
    port->logoutSQL();
}

void MainWindow::purchaseBook(void){
    Purchase* win = new Purchase();
    win->show();

    this->close();
}

void MainWindow::removeAction(void){
    try{
        int x_pos = 0;
        int y_pos = 0;

        port->loginSQL();

        port->setConfig(x_pos, y_pos);

        if(port->getStatus()){
            try{
                port->updateSQL(x_pos,y_pos,false);

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

void MainWindow::flush(void){
    int status = port->getStatus();

    if(status == 0){
        qDebug() << "查询失败，自动刷新一次";
        port->loginSQL();
    }

    status = port->getStatus();

    if(status != 0){
        int status2 = port->statusSQL("booksale");
        if(status2 == 0)
            ui->label_5->setText("无预约");
        else if(status2 == 1)
            ui->label_5->setText("有预约");
        else
            ui->label_5->setText("出错了");

        status2 = -1;
        status2 = port->statusSQL("bookpurchase");
        if(status2 == 0)
            ui->label_8->setText("无预约");
        else if(status2 == 1)
            ui->label_8->setText("有预约");
        else
            ui->label_8->setText("出错了");
    }
    else{
        qDebug() << "刷新后查询失败";
    }
}
