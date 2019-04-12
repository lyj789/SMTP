#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H

#include <QMainWindow>
#include<QFileDialog>
#include<QFile>
#include<QTextDocument>
#include<QMessageBox>
#include<QTextStream>
namespace Ui {
class MyMainWindow;
}

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();
    Ui::MyMainWindow *ui;

private slots:
    void on_pushButton_clicked();

private:
       QString curFile;

};

#endif // MYMAINWINDOW_H
