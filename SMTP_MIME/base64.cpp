#include "base64.h"
static const quint8 DATA_ASCII2BIN[128] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xE0,0xF0,0xFF,0xFF,0xF1,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xE0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x3E,0xFF,0xF2,0xFF,0x3F,
    0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
    0xFF,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,
    0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0xFF,0xFF,0xFF,0xFF,0xFF
};
QByteArray base64::base64_decode(QString inStr)
{
        int length = inStr.length(); // 字符串长度
        QByteArray ba;  // 字节数组
        QDataStream out(&ba, QIODevice::WriteOnly); // 写入数据流
        int index = 0;
        int previous = 0;
        for(int i = 0; i < length; i++)
        {
            int charValue = inStr[i].unicode();  // 字符的Unicode编码
            int orderValue = DATA_ASCII2BIN[charValue];
            if((orderValue | 0x13) == 0xF3)
                continue;
            if(index == 0)
            {
                previous = orderValue;
                index++;
            }
            else if(index == 1)
            {
                out << static_cast<quint8>((previous << 2) | (orderValue >> 4));
                previous = orderValue;
                index++;
            }
            else if(index == 2)
            {
                if(charValue == '=') // 简化处理，不再判断第二个'='
                {
                    index = 0;
                    break;
                }
                out << static_cast<quint8>((previous << 4) | (orderValue >> 2));
                previous = orderValue;
                index++;
            }
            else
            {
                index = 0;
                if(charValue == '=')break;
                out << static_cast<quint8>((previous << 6) | orderValue);
            }
        }

        if(index == 0)  // 有效字符个数必须是4的倍数
        {
             return ba;
        }
}


QPixmap base64::Base64_To_Image(QString bytearray,QString SavePath)
{
    QByteArray Ret_bytearray=base64_decode(bytearray);
    QBuffer buffer(&Ret_bytearray);
    buffer.open(QIODevice::WriteOnly);
    QPixmap imageresult;
    imageresult.loadFromData(Ret_bytearray);
    if(SavePath != "")
    {
        qDebug() <<"save" ;
        imageresult.save(SavePath);
    }
    //ui->textEdit_2->setText(Ret_bytearray.toHex());
    return imageresult;
}
