#ifndef BASE64_H
#define BASE64_H
#include"mainwindow.h"

class base64
{
public:
  static QByteArray base64_decode(QString inStr);
  static QPixmap Base64_To_Image(QString bytearray,QString SavePath);
};

#endif // BASE64_H
