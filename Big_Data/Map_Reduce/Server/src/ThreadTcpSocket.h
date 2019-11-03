#ifndef THREADTCPSOCKET_H
#define THREADTCPSOCKET_H

#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>

class ThreadTcpSocket : public QThread
{
    Q_OBJECT
    
public:
    ThreadTcpSocket(qintptr socketDescriptor);
    ~ThreadTcpSocket();

    void run() override;
    void stop();
    void waitForStop();

    bool sendMessage(int type, QByteArray const &body);

    /**
     * @brief Get the State object
     * 
     * @return true - busy
     * @return false  - free
     */
    bool getState();

private:

    QTcpSocket *socket;
    qintptr descriptor;

    //state
    bool state;
    QMutex mutexState;

    //message
    QMutex mutexSend;
    

    QByteArray *createMessage(int type, QByteArray const &body);
    char intToChar(int src);

private slots:
    void socketReadyRead();

};

#endif //THREADTCPSOCKET_H