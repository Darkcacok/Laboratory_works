#ifndef SUPPORT_H
#define SUPPORT_H

#include <QByteArray>
#include <QDataStream>


QByteArray intToByte(int32_t src);
int32_t byteToInt(QByteArray src);


#endif // SUPPORT_H
