#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtConcurrent/QtConcurrentRun>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);

signals:
    void messageProcessed(QByteArray data);
    void sendState();
    void setState(bool flag);
    void sendProgress(int type, QByteArray json);

public slots:
    void process(int type, QByteArray data);

private:
    void processThirdMessage(QByteArray data);
    QByteArray progressJSON(int progress);
};

#endif // WORKER_H
