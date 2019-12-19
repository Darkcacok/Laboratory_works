#ifndef CONNECTIONLIST_H
#define CONNECTIONLIST_H

#include <QObject>

#include <vector>

#include "connection.h"

class ConnectionList : public QObject
{
    Q_OBJECT
public:
    ConnectionList(QObject *parent = nullptr);


    size_t size();
    bool empty();
    Connection *operator[](size_t index);
    void push_back(Connection *connection);

signals:
    void stateConnectionChanged(int i);
    void workStateChanged(int i);
    void precentChanged(int i);
    void workerFinished(QHash<QString, int> map);

public slots:
    void sendData(size_t i, QByteArray message);

private:
    std::vector<Connection*> m_Connections;
    QHash<int, Connection*> m_map;

private slots:
    void stateConnectionChanged();
    void workStateChanged();
    void precentChanged();
};

#endif // CONNECTIONLIST_H
