#include "ThreadTcpSocket.h"

ThreadTcpSocket::ThreadTcpSocket(qintptr socketDescriptor, QObject *parent)
    :QObject(parent)
{
    descriptor = socketDescriptor;
    state = false;
}

ThreadTcpSocket::~ThreadTcpSocket()
{
}

void ThreadTcpSocket::init()
{


    //Sokcet
    socket = new QTcpSocket();
    socket->setSocketDescriptor(descriptor);
    worker = new Worker();
    QThread *thread = new QThread;

    worker->moveToThread(thread);
    thread->start();

    connect(worker, &Worker::messageProcessed, this, &ThreadTcpSocket::messageProcessed, Qt::QueuedConnection);
    connect(this, &ThreadTcpSocket::processMessage, worker, &Worker::process);
    connect(worker, &Worker::setState, this, [=](bool flag){
        setState(flag);
    }, Qt::QueuedConnection);
    //connect(this, &ThreadTcpSocket::deliveryMessage, this, &ThreadTcpSocket::sendMessage, Qt::QueuedConnection);

    connect(worker, &Worker::sendProgress, this, &ThreadTcpSocket::sendMessage);
    connect(socket, &QTcpSocket::readyRead, this, &ThreadTcpSocket::socketReadyRead, Qt::DirectConnection);
    connect(worker, &Worker::sendState, this, &ThreadTcpSocket::getState, Qt::QueuedConnection);

    //exec();
}

//void ThreadTcpSocket::getState()
//{
////    mutexState.lock();
////    bool state = this->state;
////    mutexState.unlock();

////    return state;


//}

void ThreadTcpSocket::sendMessage(int type, QByteArray const &body)
{
    QAbstractSocket::SocketState state = socket->state();

    if(state== QAbstractSocket::UnconnectedState)
    {
        return;
    }

    QByteArray *message = createMessage(type, body);

    if(message == nullptr)
        return;

    
    mutexSend.lock();

    uint64_t written = 0;
    while(written < message->size())
    {
        uint64_t w;
        w = socket->write(message->data() + written, message->length());

        if(w == -1)
        {
            delete[] message;
            return;
        }

        written += w;
    }

    mutexSend.unlock();

    return;
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

        return nullptr;
    }

    message->append(intToByte(type));
    message->append(intToByte(body.size()));
    message->append(body);

    return message;
}


//slots
void ThreadTcpSocket::socketReadyRead()
{
    if(m_receiveFlag == false)
    {
        m_receiveFlag = true;
        m_type = byteToInt(socket->read(4));
        m_size = byteToInt(socket->read(4));

        if(m_size == 0)
        {
            emit processMessage(m_type, {});
            m_receiveFlag = false;
            m_message.clear();
            return;
        }
    }

    while (socket->bytesAvailable() != 0)
    {
        char *buffer = new char[m_size - m_message.size()];
        int64_t reads;

        reads = socket->read(buffer, m_size - m_message.size());

        if(reads == -1)
            qDebug() << "Error read bytes from socket";

        m_message.append(buffer, reads);

        delete [] buffer;

        if(m_message.size() == m_size)
        {
            emit processMessage(m_type, m_message);

            m_receiveFlag = false;
            m_message.clear();

            return;
        }
    }
}

void ThreadTcpSocket::messageProcessed(QByteArray data)
{
    sendMessage(4, data);

    setState(false);
}

void ThreadTcpSocket::receiveState(bool state)
{
    QJsonDocument doc;
    QJsonObject stateJsObj;

    stateJsObj.insert("State", QJsonValue::fromVariant(state));

    doc.setObject(stateJsObj);

    emit sendMessage(1, doc.toBinaryData());
}
