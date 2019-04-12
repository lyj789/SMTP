#include "mymainwindow.h"
#include "ui_mymainwindow.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMainWindow)
{
    ui->setupUi(this);
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("另存为"),curFile);                     //如果文件是第一次保存，调用另存为                                                                      //文件名不为空，则保存文件
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly|QFile::Text))
        //以只写方式打开文件，如果打开失败则返回，弹出对话框
    {
        QMessageBox::warning(this,tr("保存文件"),tr("无法保存文件 %1:\n %2").arg(fileName).arg(file.errorString()));
    }
    QTextStream out(&file);
    out<<ui->textEdit->toPlainText();//将文本编辑器中的内容以纯文本的形式输出到流对象中
    curFile = QFileInfo(fileName).canonicalFilePath();//获取文件的标准路径
    setWindowTitle(QFileInfo(fileName).fileName());
    QMessageBox::about(this,tr("提示"),tr("保存成功"));
}
