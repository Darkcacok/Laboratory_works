#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

#include "support.h"
#include "messageprocessing.h"
#include "constants.h"


class Connection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_address READ address WRITE setAddress /*NOTIFY addressChanged*/)
    Q_PROPERTY(uint16_t m_port READ port WRITE setPort /*NOTIFY portChanged*/)
    Q_PROPERTY(SocketState m_stateConnection READ stateConnection WRITE setStateConnection NOTIFY stateConnectionChanged)
    Q_PROPERTY(uint8_t m_precent READ precent WRITE setPrecent NOTIFY precentChanged)

public:

    enum SocketState{
        Disconnected = 0,
        Connecting,
        Connected,
    };

    Connection(QObject *parent = nullptr);
    Connection(const QString &address, uint16_t port, QObject *parent = nullptr);

    void connectToHost(const QString &address, const uint16_t port);

    QString address() const;
    uint16_t port() const;
    QString status() const;
    SocketState stateConnection() const;
    WorkState workState() const;
    uint8_t precent() const;

    void setAddress(const QString &address);
    void setPort(const uint16_t &port);
    void setStatus(const QString &status);
    void setStateConnection(const SocketState &stateConnection);



signals:
    void stateConnectionChanged();
    void workStateChanged();
    void messageProcess(int type, const QByteArray data);
    void precentChanged();
    void workerFinished(QHash<QString, int> map);

public slots:
        void send(const QByteArray message);
        void setWorkState(const WorkState workState);
        void setPrecent(const uint8_t precent);

private:

    QString m_address;
    uint16_t m_port = 0;
    QString m_status;

    SocketState m_stateConnection;
    WorkState m_workState = None;

    uint8_t m_precent = 0;

    QTcpSocket *m_socket;

    bool m_receiveFlag = false;
    int m_type;
    int m_size;
    QByteArray m_message;

    MessageProcessing *m_mp;
    QThread *m_threadMP;

    bool connectSignalSlots();
    void processMessage(int type, QByteArray data);

private slots:
    void socketConnected();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError socketError);

    void socketReadyRead();

};

#endif // CONNECTION_H
