#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>
#include<QMessageBox>
#include<QDebug>
#include<QFileDialog>
#include<QFile>
#include<QMimeDatabase>
#include<QUrl>
#include<QTextDocument>
#include<Qurl>
#include<QBuffer>
#include<QDateTime>
#include<QMap>
#include<QTextEdit>
#include<QTextBrowser>
#include"mymainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void server_New_Connect();
    void socket_Read_Data(QTcpSocket *socket);
    void socket_Disconnect();

    void MIME_read(QByteArray a,QTcpSocket *socket);

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    //QTcpSocket *socket;
    QByteArray buffer;
    qint64 writeREsult;
    bool BoolFlush;
    int state=0;
    int times=0;
    QMap <QTcpSocket *,MyMainWindow *> New;
    QMap <QTcpSocket *,QPushButton *> Button;
};

#endif // MAINWINDOW_H
