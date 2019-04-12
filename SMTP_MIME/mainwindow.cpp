#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"base64.h"
#include"quoted.h"
#include"mymainwindow.h"
#include"ui_mymainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    server =new QTcpServer();   //初始化服务器对象

    //当服务器被客户端访问时，会触发newconnection信号，可以利用信号作为触发信号，绑定槽函数server_new_Connect
    connect(server,&QTcpServer::newConnection,this,&MainWindow::server_New_Connect);
    ui->lineEdit_IP->setText("127.0.0.1");
    ui->lineEdit_Port->setText("123");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QHostAddress IP(ui->lineEdit_IP->text());
    quint16 port=ui->lineEdit_Port->text().toUInt();
    if(server->listen(IP,port))
    {
        //监听成功
        QMessageBox::warning(this,"warning",tr("监听成功"),QMessageBox::Yes,QMessageBox::No);
    }
    else
    {
        //监听失败
        QMessageBox::warning(this,"warning",tr("监听失败"),QMessageBox::Yes,QMessageBox::No);

    }
}
void MainWindow::server_New_Connect()
{
    QTcpSocket *socket;  //每次连接都有一个新的socket
    times+=1;  //连接次数
    socket =server->nextPendingConnection(); //获取客户端连接
    QString IP=socket->peerAddress().toString();
    QString port=QString::number(socket->peerPort());
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");
    ui->textEdit_2->append(IP+"     "+port+"    "+current_date);
    MyMainWindow *a=new MyMainWindow(this);
    New[socket]=a;
    QPushButton *button=new QPushButton(this);
    Button[socket]=button;
    button->setGeometry(QRect(400,130+(times-1)*20,60,30));
    button->setText("查看");
    button->show();
    connect(Button[socket],&QPushButton::pressed,New[socket],&MyMainWindow::show);
    //服务器接收到客户端数据后，自动发送readyRead信号
    QObject::connect(socket,&QTcpSocket::readyRead,this,[=](){socket_Read_Data(socket);});
    QObject::connect(socket,&QTcpSocket::disconnected,this,&MainWindow::socket_Disconnect);
    writeREsult=socket->write("220\r\n");      //建立连接 ：服务器回传220
   // BoolFlush=socket->flush();   //从缓冲区读写数据
    state=1;
}

void MainWindow::socket_Disconnect()
{

}
void MainWindow::socket_Read_Data(QTcpSocket *socket)   //服务器端读取客户端传来的数据
{
    auto buffer=socket->readAll();   //读入缓冲区数据
    if(state==1&&buffer.startsWith("EHLO"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("250-mail\r\n");
        BoolFlush=socket->flush();
        writeREsult=socket->write("250-AUTH=LOGIN\r\n");
        BoolFlush=socket->flush();
        writeREsult=socket->write("250 8BITMIME\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"250";
        state=2;
        return;
    }
    if(state==2&&buffer.startsWith("AUTH LOGIN"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("334 VXNlcm5hbWU6\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"334";
        state=3;
        return;
    }
    if(state==3&&buffer.startsWith("MTIzQDEyMy5jb20="))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("334 UGFzc3dvcmQ6\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"334";
        state=4;
        return;
    }
    if(state==4&&buffer.startsWith("MTIzNDU2Nzg5"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("235 auth successfully\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"235";
        state=5;
        return;
    }
    if(state==5&&buffer.startsWith("MAIL FROM"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("250\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"250";
        state=6;
        return;

    }
    if(state==6&&buffer.startsWith("RCPT TO"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("250\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"250";
        state=7;
        return;

    }
    if(state==7&&buffer.startsWith("DATA"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("354 Enter mail\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"354";
        state=8;
        return;

    }
    if(state==8)
    {

        this->buffer+=buffer;

        if(this->buffer.endsWith("\r\n.\r\n"))
        {
            this->buffer.chop(5);
            qDebug()<<this->buffer;
            New[socket]->ui->textEdit->setText(this->buffer);
            writeREsult=socket->write("250 Message sent\r\n");
            BoolFlush=socket->flush();
            qDebug()<<"250";
            state=9;
            this->buffer=0;
            return;

        }
    }

    if(state==9&&buffer.startsWith("RSET"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("250\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"250";
        state=5;
        return;

    }
    if((state==9)&&buffer.startsWith("QUIT"))
    {
        qDebug()<<buffer;
        writeREsult=socket->write("221 Bye\r\n");
        BoolFlush=socket->flush();
        qDebug()<<"221";
        state=10;
        QByteArray a=New[socket]->ui->textEdit->toPlainText().toUtf8();
        MIME_read(a,socket);
        return;

    }
}


void MainWindow::MIME_read(QByteArray a,QTcpSocket *socket)
{
    qDebug()<<"调用开始";

    QRegularExpression FromRe("From: (.+)");
    QRegularExpressionMatch fromMatch = FromRe.match(a);
    QString from = fromMatch.captured(1);
    from.chop(1);
    int c=from.indexOf("<");
    if(fromMatch.hasMatch())
    {
         New[socket]->ui->lineEdit_3->setText(from.mid(c+1));
    }
    //获取发件人

    QRegularExpression ToRe("To: (.+)");
    QRegularExpressionMatch toMatch = ToRe.match(a);
    QString to = toMatch.captured(1);
    to.chop(1);
    int b=to.indexOf("<");
    if(toMatch.hasMatch())
    {
         New[socket]->ui->lineEdit->setText(to.mid(b+1));
    }
   // qDebug()<<to;
    //获取收件人

    QRegularExpression SubRe("Subject: (.+)");
    QRegularExpressionMatch subMatch = SubRe.match(a);
    QString subject = subMatch.captured(1);
    if(subMatch.hasMatch())
    {
        if(subject.startsWith("="))
        {
            subject.chop(2);
            QByteArray temp=subject.mid(10).toLatin1();
            QByteArray Subject= quoted::quoted_printable_decode(temp);
            New[socket]->ui->lineEdit_2->setText(Subject);
        }
        else
        {
         New[socket]->ui->lineEdit_2->setText(subject);
        }
    }
    //qDebug()<<subject;
    //获取邮件主题

    QRegularExpression contentRe("Content-Type: (.+);");
    QRegularExpressionMatch contentMatch = contentRe.match(a);
    QString contentType = contentMatch.captured(1);
    //获取了contentType的内容

    QRegularExpression boundaryRe("boundary=\"(.+)\"");
    QRegularExpressionMatch boundaryMatch = boundaryRe.match(a);
    QString boundary;
    QString boundaryend;
    if(boundaryMatch.hasMatch())
    {
        boundary = "--"+boundaryMatch.captured(1)+"\n";
        boundaryend="--"+boundaryMatch.captured(1)+"--";
    }
    else{boundary="\n\n";}
    //获取boundary边界

    QRegularExpression bodyRe(boundary);
    QRegularExpression bodyendRe(boundaryend);
    QRegularExpressionMatchIterator bodies = bodyRe.globalMatch(a);
    QRegularExpressionMatch bodyend=bodyendRe.match(a);
    //全局匹配所有的boundary
    int bodyStarts[100], bodyEnds[100];//存所有出现的boundary的开头结尾的位置
    QString bodyList[100];
    int count=0;
    while(bodies.hasNext()){
        QRegularExpressionMatch bodyMatch = bodies.next();
        bodyStarts[count] = bodyMatch.capturedStart(0);
        bodyEnds[count] = bodyMatch.capturedEnd(0);
        count++;
    }

    bodyStarts[count]=bodyend.capturedStart(0);
    bodyEnds[count]=bodyend.capturedEnd(0);

    for(int i=0; i<count;i++){
        bodyList[i]=a.mid(bodyEnds[i], bodyStarts[i+1]-bodyEnds[i]);
        qDebug()<<bodyList[i];
    }
    //利用boundary划分出body


    qDebug()<<boundary;
    qDebug()<<contentType;
    if(contentType.contains("multipart/"))
    {
        for(int i=0;i<count;i++)
        {
            MIME_read(bodyList[i].toLatin1(),socket);
            qDebug()<<"又调用一次";
        }
    }
    else if(contentType.contains("text/html"))
    {
        //首先获取编码方式，然后解码
        QRegularExpression encodeingRe("Content-Transfer-Encoding: (.+)");
        QRegularExpressionMatch encodingMatch=encodeingRe.match(a);
        QString encoding = encodingMatch.captured(1);
        if(encoding == "quoted-printable")
        {
                if(bodyList[0]!=NULL)
                {
                   QByteArray html= quoted::quoted_printable_decode(bodyList[0].toLatin1());
                    New[socket]->ui->textBrowser->insertHtml(html);

                }
        }
        qDebug()<<"html解码";
    }
    else if(contentType.contains("image/"))
    {
        //首先获取编码方式，然后解码
        QRegularExpression encodeingRe("Content-Transfer-Encoding: (.+)");
        QRegularExpressionMatch encodingMatch=encodeingRe.match(a);
        QString encoding = encodingMatch.captured(1);
        if(encoding == "base64")
        {
            for(int i=0;i<count;i++)
            {
                if(bodyList[i]!=NULL)
                {
                    QRegularExpression cidRe("Content-ID: <(.+)>");
                    QRegularExpressionMatch contentMatch=cidRe.match(a);
                    QString contentId = contentMatch.captured(1);
                    qDebug()<<contentId;
                    QByteArray result=base64::base64_decode(bodyList[i]);

                    QPixmap dest = base64::Base64_To_Image(bodyList[i],"e:/mail/"+contentId+".png");
                    //自动保存图片
                    QTextDocument *src=New[socket]->ui->textBrowser->document();
                    src->addResource(QTextDocument::ImageResource,QUrl("cid:"+contentId),result);
                }
            }
        }
        qDebug()<<encoding;
        qDebug()<<"图片解码";

    }



}



