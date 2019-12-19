#include "server.h"

TcpServer::TcpServer()
    : QTcpServer()
{
    stateTimer = new QTimer();

    //connect(stateTimer, &QTimer::timeout, this, &TcpServer::sendState);
    
    //stateTimer->start(500);
}

TcpServer::~TcpServer()
{
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new connection is comming";
    ThreadTcpSocket *socket = new ThreadTcpSocket(socketDescriptor);
    QThread *thread = new QThread();

    socket->moveToThread(thread);

    connect(thread, &QThread::started, socket, &ThreadTcpSocket::init);

    connect(socket, &ThreadTcpSocket::getState, this, [=]{

        bool state = false;

        for(const auto &item : connections)
        {
            if(std::get<2>(item) == true)
            {
                state = true;
                break;
            }
        }

        emit this->state(state);
    });
    connect(this, &TcpServer::state, socket, &ThreadTcpSocket::receiveState);
    connect(socket, &ThreadTcpSocket::setState, this, &TcpServer::setState);

    connections.push_back(std::make_tuple(thread, socket, false));

    thread->start();
}

TcpServer::Config TcpServer::getConfig()
{
    Config conf;
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QNetworkInterface &address: QNetworkInterface::allInterfaces())
    {
        for (const QNetworkAddressEntry &entry: address.addressEntries())
        {
            if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol && entry.ip() != localhost)
                conf.address.push_back(std::make_tuple(toString(address.type()), address.name(), entry.ip().toString()));
        }
//        //if (address.protocol() == QAbstractSocket::IPv4Protocol && address == localhost)
//             conf.address.push_back(std::make_tuple(toString(address.type()), address.name(), "test"));
    }

    conf.port = this->serverPort();

    return conf;
}

//bool TcpServer::getState()
//{
//    bool state = false;

//    for (uint i = 0; i < connections.size() || state; i++)
//    {
//        state = connections[i]->getState();
//    }

//    return state;
//}

QByteArray TcpServer::createJsonState(bool state)
{
    QJsonDocument doc;
    QJsonObject stateJsObj;

    stateJsObj.insert("State", QJsonValue::fromVariant(state));

    doc.setObject(stateJsObj);

    return doc.toJson();
}

QString TcpServer::toString(QNetworkInterface::InterfaceType type)
{
    switch (type) {
    case QNetworkInterface::Unknown:
        return QString("Unknown");
    case QNetworkInterface::Loopback:
        return QString("Loopback");
    case QNetworkInterface::Virtual:
        return QString("Virtual");
    case QNetworkInterface::Ethernet:
        return QString("Ethernet");
    case QNetworkInterface::Wifi:
        return QString("Wifi");
    case QNetworkInterface::Fddi:
        return QString("Fddi");
    case QNetworkInterface::Ppp:
        return QString("Ppp");
    case QNetworkInterface::Slip:
        return QString("Slip");
    case QNetworkInterface::Phonet:
        return QString("Phonet");
    case QNetworkInterface::Ieee802154:
        return QString("Ieee802154");
    case QNetworkInterface::Ieee80216:
        return QString("Ieee80216");
    case QNetworkInterface::Ieee1394:
        return QString("Ieee1394");
    }
}


void TcpServer::setState(bool flag)
{
    ThreadTcpSocket *socket = static_cast<ThreadTcpSocket*>(QObject::sender());

    std::tuple<QThread*, ThreadTcpSocket*, bool> item;

    for(auto &item : connections)
    {
        if(std::get<1>(item) == socket)
        {
            std::get<2>(item) = flag;
            break;
        }
    }
}
