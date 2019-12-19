#include "support.h"

QByteArray intToByte(int32_t src)
{
    unsigned char dest[4];
    QByteArray byteArray;

    dest[0] = (src) & 0xFF;
    dest[1] = (src >> 8) & 0xFF;
    dest[2] = (src >> 16) & 0xFF;
    dest[3] = (src >> 24) & 0xFF;

    byteArray.append(reinterpret_cast<char*>(dest), 4);

    return byteArray;
}

int32_t byteToInt(QByteArray src)
{
    if(src.size() != 4)
        return 0;

    int32_t dest;
    QDataStream stream(src);
    stream.setByteOrder(QDataStream::LittleEndian);

    stream >> dest;

    return dest;
}
