#include "worker.h"

#include <iostream>

Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::setConnections(ConnectionList *Connections)
{
    m_Connections = Connections;

    stateTimer = new QTimer();
    connect(stateTimer, &QTimer::timeout, this, &Worker::getState);
    connect(m_Connections, &ConnectionList::workerFinished, this, &Worker::workerFinished);
    connect(this, &Worker::sendData, m_Connections, &ConnectionList::sendData, Qt::QueuedConnection);

    stateTimer->start(1000);
}

void Worker::openFile(QUrl url)
{
    if(!Logs.empty())
    {
        Logs.clear();
    }

    test.clear();

    //QFile file(url.path());
    //QTextStream stream(&file);

//    if(!file.open(QFile::ReadOnly))
//    {
//        qDebug() << "Eror open file";
//        return;
//    }

    QFuture<void> th = QtConcurrent::run(this, &Worker::processFile, url.toLocalFile());
    //th.waitForFinished();
    //processFile(file);

//    file.close();
}

void Worker::sendWorkToServer()
{
    QFuture<void> th = QtConcurrent::run(this, &Worker::_sendDataToServer);

}

uint8_t Worker::getProgressLoad() const
{
    return m_progressLoad;
}

void Worker::setProgressLoad(const int progressLoad)
{
    m_progressLoad = progressLoad;
    progressLoadChanged();
}

bool Worker::getVisibleFileLoadDialog() const
{
    return m_visibleFileLoadDialog;
}

bool Worker::getSendingData() const
{
    return m_sendingData;
}

void Worker::setVisibleFileLoadDialog(bool visibleFileLoadDialog)
{
    m_visibleFileLoadDialog = visibleFileLoadDialog;
    visibleFileDialogChanged();
}

void Worker::workerFinished(QHash<QString, int> map)
{
    QHashIterator<QString, int> i(map);

    while (i.hasNext()) {
        i.next();
        result[i.key()] += i.value();
    }

    --workers;

    if(workers == 0)
    {

        QHashIterator<QString, int> i(result);
        while (i.hasNext()) {
            i.next();
            qDebug() << i.key() << "   " << i.value();
        }

        emit workersFinished();
    }
}

void Worker::saveResultToFile(QUrl url)
{
    QFile file(url.toLocalFile());

    if(!file.open(QFile::WriteOnly))
    {
        qDebug() << "ERROR: open file: \"" << url.toString() << "\"";
        return;
    }

    QTextStream textWriter(&file);

    if(workers == 0)
    {
        QHashIterator<QString, int> i(result);
        while (i.hasNext()) {
            i.next();
            textWriter << i.key() << "\t" << i.value() << endl;
        }
    }

    file.close();
}

void Worker::setSendingData(bool sendingData)
{
    m_sendingData = sendingData;
    sendingDataChanged();
}

void Worker::_sendDataToServer()
{
    setSendingData(true);
    result.clear();
    workers = 0;

    std::vector<Connection*> connectios;

    for(int i = 0; i < m_Connections->size(); ++i)
    {
        if((*m_Connections)[i]->stateConnection() == Connection::Connected && (*m_Connections)[i]->workState() == Free)
        {
            connectios.push_back((*m_Connections)[i]);
        }
    }

    if(!connectios.size())
        return;

//    int parts = Logs.size() / connectios.size();
    int parts = test.size() / connectios.size();

    std::vector<QByteArray> messages;

//    for(int i = 0; i < connectios.size() - 1; ++i)
//    {
//        nlohmann::json array;
//        for(int j = i*parts; j < i*parts + parts; ++j)
//        {
//            nlohmann::json item;
//            item["Date"] = Logs[j].date.toString(QString("dd.MM.yyyy")).toStdString();
//            item["User"] = Logs[j].user.toStdString();
//            item["Url"] = Logs[j].url.toStdString();

//            array["data"].push_back(item);
//        }

//        std::vector<uint8_t> buf = nlohmann::json::to_bson(array);
//        messages.push_back(QByteArray(reinterpret_cast<const char*>(buf.data()), buf.size()));
//    }

    for(int i = 0; i < connectios.size() - 1 && i < parts - 1; ++i)
    {
        nlohmann::json array;
        for(int j = i*parts; j < i*parts + parts; ++j)
        {
            array["data"].push_back(test[j]);
        }

        ++workers;
        std::vector<uint8_t> buf = nlohmann::json::to_bson(array);
        messages.push_back(QByteArray(reinterpret_cast<const char*>(buf.data()), buf.size()));
    }


    nlohmann::json array;
//    for(int i = parts*(connectios.size() - 1); i < Logs.size(); ++i)
//    {
//        nlohmann::json item;
//        item["Date"] = Logs[i].date.toString(QString("dd.MM.yyyy")).toStdString();
//        item["User"] = Logs[i].user.toStdString();
//        item["Url"] = Logs[i].url.toStdString();

//        array["data"].push_back(item);

//    }

    for(int i = parts*(connectios.size() - 1); i < test.size(); ++i)
    {
        array["data"].push_back(test[i]);
    }

    //std::cout << array;
    //nlohmann::json object;
    //object["data"] = array;
    std::vector<uint8_t> buf = nlohmann::json::to_bson(array);
    messages.push_back(QByteArray(reinterpret_cast<const char*>(buf.data()), buf.size()));
    ++workers;

    for(int i = 0; i < messages.size(); ++i)
    {
        QByteArray m = createMessage(3, messages.at(i));

        emit sendData(i, m);

    }

    setSendingData(false);
}


bool Worker::workExist() const
{
    return m_workExist;
}

void Worker::processFile(QString filePath)
{
    QFile file(filePath);
    QTextStream stream(&file);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << "Eror open file";
        return;
    }

    uint64_t fileSize = file.size();
    uint64_t read = 0;

    setVisibleFileLoadDialog(true);

    while(!file.atEnd())
    {
        char line[1000];

        int64_t length = file.readLine(line, sizeof (line));
        if(length <= 0)
        {
            qDebug() << "Error Read File";
            return ;
        }

        if(!processLine(line))
            return;

        read += length;

        if((read * 100) / fileSize - getProgressLoad() > 0)
        {
            setProgressLoad((read * 100) / fileSize);
            qDebug() << (read * 100) / fileSize;
        }
    }

    file.close();

    setVisibleFileLoadDialog(false);
    m_workExist = true;
    emit workExistChanged();

    return;
}

bool Worker::processLine(const QByteArray line)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QByteArray tempLine = codec->toUnicode(line).toUtf8();
    if(tempLine.at(0) == '#')
    {
        return true;
    }
//    QStringList stringList = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

//    if(stringList.size() < LOG_SIZE)
//        return false;

//    Logs.push_back({QDate::fromString(stringList.at(0), "dd.MM.yyyy"),
//                    stringList.at(1),
//                    stringList.at(2)});

    tempLine = tempLine.trimmed();

    if(tempLine.isEmpty())
    {
        return true;
    }

    test.push_back(tempLine);

    return true;
}


QByteArray Worker::createMessage(int type, const QByteArray body)
{
    QByteArray message;

    message.append(intToByte(type));
    message.append(intToByte(body.size()));
    message.append(body);

    return message;
}

void Worker::getState()
{
    QByteArray message = createMessage(2, QByteArray());

    for(int i = 0; i < m_Connections->size(); ++i)
    {
        (*m_Connections)[i]->send(message);
    }
}
