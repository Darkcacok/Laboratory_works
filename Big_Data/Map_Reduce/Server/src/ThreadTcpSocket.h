#ifndef THREADTCPSOCKET_H
#define THREADTCPSOCKET_H

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

#include "worker.h"
#include "support.h"

class ThreadTcpSocket : public QObject
{
    Q_OBJECT
    
public:
    ThreadTcpSocket(qintptr socketDescriptor, QObject *parent = nullptr);
    ~ThreadTcpSocket();

    void stop();
    void waitForStop();

    //bool sendMessage(int type, QByteArray const &body);

    /**
     * @brief Get the State object
     * 
     * @return true - busy
     * @return false  - free
     */

signals:
    void setState(bool flag);
    void getState();
    void processMessage(int type, QByteArray data);
    //void deliveryMessage(int type, QByteArray const &body);

public slots:
    void receiveState(bool state);
    void init();

private:

    QTcpSocket *socket;
    qintptr descriptor;

    //state
    bool state;
    QMutex mutexState;

    //message
    QMutex mutexSend;

    Worker *worker;

    bool m_receiveFlag = false;
    int m_type;
    int m_size;
    QByteArray m_message;
    

    QByteArray *createMessage(int type, QByteArray const &body);

private slots:
    void socketReadyRead();
    void messageProcessed(QByteArray data);
    void sendMessage(int type, QByteArray const &body);

};

#endif //THREADTCPSOCKET_H
