#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "data.h"
#include "purchase.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showNewMainWindow();


private slots:
    void saleBook(void);
    void purchaseBook(void);
    void removeAction(void);
    void flush(void);

private:
    Data* port;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
