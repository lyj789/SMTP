#ifndef QUOTED_H
#define QUOTED_H
#include"mainwindow.h"

class quoted
{
public:
    static QByteArray quoted_printable_decode(QByteArray inStr);
};

#endif // QUOTED_H
