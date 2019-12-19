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
#include <QObject>

#include <QNetworkInterface>

#include <vector>
#include <tuple>

#include "ThreadTcpSocket.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    struct Config
    {
        std::vector<std::tuple<QString, QString, QString>> address;
        quint16 port;
    };

    TcpServer();
    ~TcpServer();

    void incomingConnection(qintptr socketDescriptor) override;

    Config getConfig();

signals:
    void state(bool flag);

private:

    /**
     * @brief Get the State object
     *
     * @return true - busy
     * @return false  - free
     */
    std::vector<std::tuple<QThread*, ThreadTcpSocket*, bool>> connections;

    //state
    QTimer *stateTimer;


    //bool getState();
    QByteArray createJsonState(bool state);

    QString toString(QNetworkInterface::InterfaceType type);

private slots:
    void setState(bool flag);

};

#endif //SERVER_H
