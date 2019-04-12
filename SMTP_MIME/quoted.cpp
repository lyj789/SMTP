#include "quoted.h"

QByteArray quoted::quoted_printable_decode(QByteArray inStr)
{
    QByteArray dst;
    QByteArray INSTR;

    for(int i=0;i<inStr.size();i++)
    {
        if(inStr[i]=='=')
        {
            if(inStr.mid(i+1,2)=="\r\n")
            {
                //qDebug()<<"忽略";
            }
            if(inStr.mid(i+1,1)=="\n")
            {
                i-=1;
            }
            else
            {
                dst=inStr.mid(i+1,2);
                if(dst.length()==2)

                { INSTR+=char(dst.toInt(nullptr,16));}

            }

            i+=2;
        }
        else
        {
            INSTR+=inStr[i];
        }
    }
    return INSTR;
}
