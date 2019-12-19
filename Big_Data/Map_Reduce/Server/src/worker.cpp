#include "worker.h"

#include "json.hpp"

Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::process(int type, QByteArray data)
{
//    QJsonDocument doc;
//    doc = QJsonDocument::fromBinaryData(data);
//    QJsonArray array = doc.array();


    switch (type) {
    case 3:
    {
        emit setState(true);
        QtConcurrent::run(this, &Worker::processThirdMessage, data);
        break;
    }
    case 2:
    {
        emit sendState();
        break;
    }

    }
}

void Worker::processThirdMessage(QByteArray data)
{
    nlohmann::json js = nlohmann::json::from_bson(std::vector<uint8_t>{data.begin(), data.end()});

    auto array = js["data"];

    QHash<QString, int> map;
    int percent = 0;

    for(int i = 0; i < array.size(); ++i)
    {
//        std::string url;
//        array[i]["Url"].get_to(url);
//        ++map[QString(url.data())];

        auto buffer = array[i].get<std::string>();

        QByteArray test(reinterpret_cast<const char*>((buffer.data())), buffer.size());

        QTextStream stream(&test);

        QString url, id, date;

        stream >> date >> id >> url;

        ++map[url];

        if( ((((i + 1) * 100)) / array.size()) - percent > 0)
        {
            percent = (((i + 1) * 100)) / array.size();
            sendProgress(5, progressJSON(percent));
        }
    }

    QJsonDocument doc;
    QJsonArray jsonArrayData;
    QHashIterator<QString, int> i(map);

    while (i.hasNext()) {
        i.next();
        QJsonObject item_data;
        item_data.insert("Url", QJsonValue(i.key()));
        item_data.insert("Number", QJsonValue(i.value()));
        jsonArrayData.push_back(item_data);
    }

    js.clear();
    doc.setArray(jsonArrayData);

    emit messageProcessed(doc.toBinaryData());

    qDebug() << "finished";
}

QByteArray Worker::progressJSON(int progress)
{
    QJsonDocument doc;
    QJsonObject item;

    item.insert("Progress", QJsonValue(progress));
    doc.setObject(item);

    return doc.toBinaryData();
}
