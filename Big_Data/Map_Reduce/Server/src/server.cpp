#include "server.h"

TcpServer::TcpServer()
    : QTcpServer()
{
    stateTimer = new QTimer();

    connect(stateTimer, &QTimer::timeout, this, &TcpServer::sendState);
    
    stateTimer->start(500);
}

TcpServer::~TcpServer()
{
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    ThreadTcpSocket *socket = new ThreadTcpSocket(socketDescriptor);
    socket->run();

    connections.push_back(socket);
}

TcpServer::Config TcpServer::getConfig()
{
    Config conf = {this->serverAddress().toString(),
                   this->serverPort()};

    return conf;
}

bool TcpServer::getState()
{
    bool state = false;

    for (uint i = 0; i < connections.size() || state; i++)
    {
        state = connections[i]->getState();
    }

    return state;
}

QByteArray TcpServer::createJsonState(bool state)
{
    QJsonDocument doc;
    QJsonObject stateJsObj;

    stateJsObj.insert("State", QJsonValue::fromVariant(state));

    doc.setObject(stateJsObj);

    return doc.toJson();
}

//slots
void TcpServer::sendState()
{
    if(connections.size() == 0)
        return;
        
    bool state = getState();
    QByteArray jsState = createJsonState(state);

    QString str_state = state? "Free": "Busy";
    qDebug() << "Sending state: " << str_state;

    for(uint i = 0; i < connections.size(); i++)
    {
        if(!connections[i]->sendMessage(1, jsState))
        {
            qDebug() << "Can't send state";
        }
    }
}