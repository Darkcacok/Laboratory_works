#include "messageprocessing.h"

MessageProcessing::MessageProcessing(QObject *parent)
    :QObject(parent)
{

}

void MessageProcessing::process(int type, const QByteArray data)
{
    switch (type) {
    case 1:
    {
        QJsonDocument doc;
        doc = QJsonDocument::fromBinaryData(data);

        QVariantMap dataMap = doc.object().toVariantMap();

        if(!dataMap["State"].toBool())
        {
            setWorkState(Free);
        }
        else
        {
            setWorkState(Busy);
        }

        break;
    }
    case 4:
    {

        QJsonDocument doc;
        doc = QJsonDocument::fromBinaryData(data);
        QJsonArray array = doc.array();

        QHash<QString, int> map;


        for(int i = 0; i < array.size(); ++i)
        {
            map[array.at(i).toObject().value("Url").toString()] = array.at(i).toObject().value("Number").toInt();
            qDebug() << array.at(i).toObject().value("Url").toString() << "     "
                     << array.at(i).toObject().value("Number").toInt();
        }

        emit workFinished(map);

        break;
    }
    case 5:
    {
        auto begin = std::chrono::steady_clock::now();
        QJsonDocument doc;
        doc = QJsonDocument::fromBinaryData(data);

        QVariantMap dataMap = doc.object().toVariantMap();

        setPrecent(dataMap["Progress"].toInt());

        qDebug() << dataMap["Progress"].toInt();


        auto end = std::chrono::steady_clock::now();

        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        qDebug() << "The time: " << elapsed_ms.count() << " ms " << "PRocess";

        break;

    }
    }
}
