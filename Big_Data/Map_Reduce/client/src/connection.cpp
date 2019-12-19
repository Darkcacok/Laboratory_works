#include "connection.h"

#include <chrono>

Connection::Connection(QObject *parent)
    : QObject(parent),
      m_stateConnection(Disconnected),
      m_status("Disconnected")
{
    qRegisterMetaType<WorkState>("WorkState");

    m_socket = new QTcpSocket(this);

    m_mp = new MessageProcessing();
    m_threadMP = new QThread();
    m_mp->moveToThread(m_threadMP);

    m_threadMP->start();

    connectSignalSlots();
}

Connection::Connection(const QString &address, uint16_t port, QObject *parent)
    : QObject(parent),
      m_address(address),
      m_port(port)
{
    m_socket = new QTcpSocket(this);

    connectSignalSlots();

    m_socket->connectToHost(m_address, port);
}

void Connection::connectToHost(const QString &address, const uint16_t port)
{
    m_stateConnection = Connecting;
    m_status = "Connecting";
    m_socket->connectToHost(address, port);
    stateConnectionChanged();
}

QString Connection::address() const
{
    return m_address;
}

uint16_t Connection::port() const
{
    return m_port;
}

Connection::SocketState Connection::stateConnection() const
{
    return m_stateConnection;
}

void Connection::setAddress(const QString &address)
{
    m_address = address;
}

void Connection::setPort(const uint16_t &port)
{
    m_port = port;
}

void Connection::setStateConnection(const SocketState &stateConnection)
{
    m_stateConnection = stateConnection;
    stateConnectionChanged();
}

WorkState Connection::workState() const
{
    return m_workState;
}

QString Connection::status() const
{
    return m_status;
}

void Connection::setStatus(const QString &status)
{
    m_status = status;
    stateConnectionChanged();
}

bool Connection::connectSignalSlots()
{
    if(!m_socket)
        return false;

    connect(m_socket, &QTcpSocket::connected, this, &Connection::socketConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &Connection::socketDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error), this, &Connection::socketError);

    connect(m_socket, &QTcpSocket::readyRead, this, &Connection::socketReadyRead);


    connect(this, &Connection::messageProcess, m_mp, &MessageProcessing::process, Qt::QueuedConnection);
    connect(m_mp, &MessageProcessing::setWorkState, this, &Connection::setWorkState, Qt::QueuedConnection);
    connect(m_mp, &MessageProcessing::setPrecent, this, &Connection::setPrecent, Qt::QueuedConnection);
    connect(m_mp, &MessageProcessing::workFinished, this, &Connection::workerFinished, Qt::QueuedConnection);

    return true;
}

void Connection::processMessage(int type, QByteArray data)
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


        for(int i = 0; i < array.size(); ++i)
        {
            qDebug() << array.at(i).toObject().value("Url").toString() << "     "
                     << array.at(i).toObject().value("Number").toInt();
        }

        break;
    }
    case 5:
    {
        auto begin = std::chrono::steady_clock::now();
        QJsonDocument doc;
        doc = QJsonDocument::fromBinaryData(data);

        QVariantMap dataMap = doc.object().toVariantMap();

        qDebug() << dataMap["Progress"].toInt();


        auto end = std::chrono::steady_clock::now();

        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        qDebug() << "The time: " << elapsed_ms.count() << " ms " << "PRocess";

        break;

    }
    }
}

void Connection::socketConnected()
{
    m_stateConnection = Connected;
    m_status = "Connected";
    stateConnectionChanged();
}

void Connection::socketDisconnected()
{

}

void Connection::socketError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        m_status =  "Connection Refused Error";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        m_status =  "Remote Host Closed Error";
        break;
    case QAbstractSocket::HostNotFoundError:
        m_status =  "Host Not Found Error";
        break;
    case QAbstractSocket::SocketAccessError:
        m_status =  "Socket Access Error";
        break;
    case QAbstractSocket::SocketResourceError:
        m_status =  "Socket Resource Error";
        break;
    case QAbstractSocket::SocketTimeoutError:
        m_status =  "Socket Timeout Error";
        break;
    case QAbstractSocket::DatagramTooLargeError:
        m_status =  "Datagram Too Large Error";
        break;
    case QAbstractSocket::NetworkError:
        m_status =  "Network Error";
        break;
    case QAbstractSocket::AddressInUseError:
        m_status =  "Address In Use Error";
        break;
    case QAbstractSocket::SocketAddressNotAvailableError:
        m_status =  "Socket Address Not Available Error";
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        m_status =  "Unsupported Socket Operation Error";
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        m_status =  "Proxy Authentication Required Error";
        break;
    case QAbstractSocket::SslHandshakeFailedError:
        m_status =  "Ssl Handshake Failed Error";
        break;
    case QAbstractSocket::UnfinishedSocketOperationError:
        m_status =  "Unfinished Socket Operation Error";
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        m_status =  "Proxy Connection Refused Error";
        break;
    case QAbstractSocket::ProxyConnectionClosedError:
        m_status =  "Proxy Connection Closed Error";
        break;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        m_status =  "Proxy Connection Timeout Error";
        break;
    case QAbstractSocket::ProxyNotFoundError:
        m_status =  "Proxy Not Found Error";
        break;
    case QAbstractSocket::ProxyProtocolError:
        m_status =  "Proxy Protocol Error";
        break;
    case QAbstractSocket::OperationError:
        m_status =  "Operation Error";
        break;
    case QAbstractSocket::SslInternalError:
        m_status =  "SslInternal Error";
        break;
    case QAbstractSocket::SslInvalidUserDataError:
        m_status =  "Ssl Invalid User Data Error";
        break;
    case QAbstractSocket::TemporaryError:
        m_status =  "Temporary Error";
        break;
    case QAbstractSocket::UnknownSocketError:
        m_status =  "Unknown Socket Error";
        break;
    }

    m_stateConnection = Disconnected;
    emit stateConnectionChanged();
}

void Connection::socketReadyRead()
{
    auto begin = std::chrono::steady_clock::now();
    if(m_receiveFlag == false)
    {
        m_receiveFlag = true;
        m_type = byteToInt(m_socket->read(4));
        m_size = byteToInt(m_socket->read(4));
    }

    while (m_socket->bytesAvailable() != 0)
    {
        char buffer[m_size];
        int64_t reads;

        reads = m_socket->read(buffer, m_size - m_message.size());

        if(reads == -1)
            qDebug() << "Error read bytes from socket";

        m_message.append(buffer, reads);

        if(m_message.size() == m_size)
        {
            auto end = std::chrono::steady_clock::now();

            auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
            qDebug() << "The time: " << elapsed_ms.count() << " ms " << "Read";

            emit messageProcess(m_type, m_message);

            m_receiveFlag = false;
            m_message.clear();

            break;
        }
    }

    if(m_socket->bytesAvailable() > 0)
        socketReadyRead();
}

void Connection::send(const QByteArray message)
{
    if(this->stateConnection() == Connected/* && this->workState() == Free*/)
    {
        uint64_t written = 0;
        while(written < message.size())
        {
            uint64_t w;
            w = m_socket->write(message.data() + written, message.size());

            if(w == -1)
            {
                return;
            }

            written += w;
        }
    }

    //m_socket->waitForBytesWritten();
}

void Connection::setWorkState(const WorkState workState)
{
    if(WorkState::None == workState || workState == WorkState::Free)
        setPrecent(0);

    m_workState = workState;
    emit workStateChanged();
}

uint8_t Connection::precent() const
{
    return m_precent;
}

void Connection::setPrecent(const uint8_t precent)
{
    m_precent = precent;
    precentChanged();
}
