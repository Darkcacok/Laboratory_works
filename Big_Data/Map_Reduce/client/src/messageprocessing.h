#ifndef MESSAGEPROCESSING_H
#define MESSAGEPROCESSING_H

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QByteArray>


#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "constants.h"

class MessageProcessing : public QObject
{
    Q_OBJECT
public:
    MessageProcessing(QObject *parent = nullptr);


signals:
    void setWorkState(const WorkState state);
    void setPrecent(const int precent);
    void workFinished(QHash<QString, int> map);

public slots:
    void process(int type, const QByteArray data);

private:



};

#endif // MESSAGEPROCESSING_H
