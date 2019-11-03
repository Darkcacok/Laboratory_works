#include "ThreadTcpSocket.h"

ThreadTcpSocket::ThreadTcpSocket(qintptr socketDescriptor)
    :QThread()
{
    descriptor = socketDescriptor;
    state = false;
}

ThreadTcpSocket::~ThreadTcpSocket()
{
}

void ThreadTcpSocket::run()
{
    //Sokcet
    socket = new QTcpSocket();
    socket->setSocketDescriptor(descriptor);

    connect(socket, &QTcpSocket::readyRead, this, &ThreadTcpSocket::socketReadyRead);

    exec();
}

bool ThreadTcpSocket::getState()
{
    mutexState.lock();
    bool state = this->state;
    mutexState.unlock();

    return state;
}

bool ThreadTcpSocket::sendMessage(int type, QByteArray const &body)
{
    QByteArray *message = createMessage(type, body);

    if(message = nullptr)
        return false;

    
    mutexSend.lock();

    uint64_t written = 0;
    while(written < message->size())
    {
        uint64_t w;
        w = socket->write(message->data() + written, message->length());

        if(w == -1)
        {
            delete[] message;
            return false;
        }

        written += w;
    }

    mutexSend.unlock();

    return true;
}

QByteArray *ThreadTcpSocket::createMessage(int type, QByteArray const &body)
{
    QByteArray *message;

    try
    {
        message = new QByteArray();
    }
    catch(const std::bad_alloc &e)
    {
        qDebug() << e.what();

        return NULL;
    }
    
    message->append(type);
    message->append(body);
    message->append({"0xFF"});

    return message;
}

char ThreadTcpSocket::intToChar(int src)
{
    char dest;

    dest = (src >> 24) & 0xFF;
}

//slots
void ThreadTcpSocket::socketReadyRead()
{

}