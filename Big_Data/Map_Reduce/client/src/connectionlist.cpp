#include "connectionlist.h"

ConnectionList::ConnectionList(QObject *parent)
    :QObject(parent)
{
}

size_t ConnectionList::size()
{
    return m_Connections.size();
}

bool ConnectionList::empty()
{
    return m_Connections.empty();
}

Connection* ConnectionList::operator[](size_t index)
{
    if(m_Connections.size() < index)
        return nullptr;

    return m_Connections[index];
}

void ConnectionList::push_back(Connection *connection)
{
    m_Connections.push_back(connection);

    m_map[m_Connections.size() - 1] = connection;

    connect(connection, &Connection::stateConnectionChanged, this, QOverload<>::of(&ConnectionList::stateConnectionChanged));
    connect(connection, &Connection::workStateChanged, this, QOverload<>::of(&ConnectionList::workStateChanged));
    connect(connection, &Connection::precentChanged, this, QOverload<>::of(&ConnectionList::precentChanged));
    connect(connection, &Connection::workerFinished, this, &ConnectionList::workerFinished);
    //connect(connection, &Connection::send, this, &ConnectionList::workerFinished);
}

void ConnectionList::sendData(size_t i, QByteArray message)
{
    Connection *con = m_map.value(i);
    con->send(message);
}

void ConnectionList::stateConnectionChanged()
{
    Connection *con = static_cast<Connection*>(QObject::sender());

    int index = m_map.key(con);

    emit stateConnectionChanged(index);
}

void ConnectionList::workStateChanged()
{
    Connection *con = static_cast<Connection*>(QObject::sender());

    int index = m_map.key(con);

    emit workStateChanged(index);
}

void ConnectionList::precentChanged()
{
    Connection *con = static_cast<Connection*>(QObject::sender());

    int index = m_map.key(con);

    emit precentChanged(index);
}
