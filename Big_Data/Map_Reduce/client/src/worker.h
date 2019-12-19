#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QDate>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include <QTextCodec>

#include "connectionlist.h"
#include "QDataStream"
#include "vector"

#include "support.h"
#include "constants.h"

#include "json.hpp"

class Worker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool workExist READ workExist NOTIFY workExistChanged)
    Q_PROPERTY(bool visibleFileLoadDialog READ getVisibleFileLoadDialog NOTIFY visibleFileDialogChanged)
    Q_PROPERTY(int progressLoad READ getProgressLoad NOTIFY progressLoadChanged)
    Q_PROPERTY(bool sendingData READ getSendingData NOTIFY sendingDataChanged)

public:
    explicit Worker(QObject *parent = nullptr);

    void setConnections(ConnectionList *Connections);

    struct Log{
        QDate date;
        QString user;
        QString url;
    };

    bool workExist() const;
    uint8_t getProgressLoad() const;
    bool getVisibleFileLoadDialog() const;
    bool getSendingData() const;


signals:
    void workExistChanged();
    void visibleFileDialogChanged();
    void progressLoadChanged();
    void workersFinished();
    void sendData(size_t i, QByteArray message);

    void sendingDataChanged();

public slots:
    void openFile(QUrl url);
    void sendWorkToServer();
    void setProgressLoad(const int progressLoad);
    void setVisibleFileLoadDialog(bool visibleFileLoadDialog);
    void workerFinished(QHash<QString, int> map);
    void saveResultToFile(QUrl url);


private:
    ConnectionList *m_Connections = nullptr;
    std::vector<Log> Logs;
    std::vector<QByteArray> test;
    bool m_workExist = false;

    QHash<QString, int> result;
    int workers;

    QTimer *stateTimer;

    static const int LOG_SIZE = 3;

    bool m_visibleFileLoadDialog = false;
    int m_progressLoad = 0;
    bool m_sendingData = false;

    void processFile(QString filePath);
    bool processLine(const QByteArray line);
    QByteArray createMessage(int type, const QByteArray body);
    void setSendingData(bool sendingData);

    void _sendDataToServer();

private slots:
    void getState();
};

#endif // WORKER_H
