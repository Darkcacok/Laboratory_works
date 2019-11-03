#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QTimer>

#include <vector>

#include "ThreadTcpSocket.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    struct Config
    {
        QString address;
        quint16 port;
    };

    TcpServer();
    ~TcpServer();

    void incomingConnection(qintptr socketDescriptor) override;

    Config getConfig();

private:
    std::vector<ThreadTcpSocket*> connections;

    //state
    QTimer *stateTimer;


    bool getState();
    QByteArray createJsonState(bool state);

private slots:
    void sendState();

};

#endif //SERVER_H