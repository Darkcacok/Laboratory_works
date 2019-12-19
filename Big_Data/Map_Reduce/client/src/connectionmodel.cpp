#include "connectionmodel.h"


ConnectionModel::ConnectionModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int ConnectionModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !m_Connections)
        return 0;

    return m_Connections->size();
}

QVariant ConnectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_Connections)
        return QVariant();

    switch (role) {
    case AddressRole:
        return QVariant((*m_Connections)[index.row()]->address());
    case PortRole:
    {
        uint16_t port = (*m_Connections)[index.row()]->port();
        if(port == 0)
            return QVariant(QString());
        return QVariant(QString::number(port));
    }
    case StatusRole:
        return QVariant( (*m_Connections)[index.row()]->status());
    case StateConnectionRole:
        return QVariant((*m_Connections)[index.row()]->stateConnection());
    case WorkStateRole:
        return QVariant((*m_Connections)[index.row()]->workState());
    case PrecentRole:
        return QVariant((*m_Connections)[index.row()]->precent());

    }

    // FIXME: Implement me!
    return QVariant();
}

bool ConnectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
//    if (data(index, role) != value) {
//        // FIXME: Implement me!
//        emit dataChanged(index, index, QVector<int>() << role);
//        return true;
//    }

    if(!m_Connections)
        return false;

    bool flag = false;
    switch (role) {
    case AddressRole:
        (*m_Connections)[index.row()]->setAddress(value.toString());
        flag = true;
        break;
    case PortRole:
        (*m_Connections)[index.row()]->setPort(value.toInt());
        flag = true;
        break;
    case StateConnectionRole:
        (*m_Connections)[index.row()]->setStateConnection(static_cast<Connection::SocketState>(value.toInt()));
        flag = true;
        break;
    }

    if(flag)
    {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }

    return false;
}

Qt::ItemFlags ConnectionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> ConnectionModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[AddressRole] = "address";
    names[PortRole] = "port";
    names[StatusRole] = "status";
    names[StateConnectionRole] = "stateConnection";
    names[WorkStateRole] = "workState";
    names[PrecentRole] = "precent";

    return names;
}

void ConnectionModel::setConnections(ConnectionList *conList)
{
    m_Connections = conList;

    connect(m_Connections, QOverload<int>::of(&ConnectionList::stateConnectionChanged), this, &ConnectionModel::stateConnectionChanged);
    connect(m_Connections, QOverload<int>::of(&ConnectionList::workStateChanged), this, &ConnectionModel::workStateChanged);
    connect(m_Connections, QOverload<int>::of(&ConnectionList::precentChanged), this, &ConnectionModel::precentChanged);
}

ConnectionList *ConnectionModel::connections() const
{
    return m_Connections;
}

void ConnectionModel::appendConnectionItem()
{
    emit beginInsertRows(QModelIndex(), m_Connections->size(), m_Connections->size());

    (*m_Connections).push_back(new Connection());

    emit endInsertRows();
}

void ConnectionModel::onConnectClicked(int index, QString address, int port)
{
    for(int i = 0; i < m_Connections->size(); ++i)
    {
        if(index == i)
            continue;

        if((*m_Connections)[i]->address() == address && (*m_Connections)[i]->port() == port)
        {
            (*m_Connections)[index]->setStatus("Error connect to the same port");
            return;
        }
    }

    (*m_Connections)[index]->connectToHost(address, port);
}

void ConnectionModel::stateConnectionChanged(int i)
{
    QModelIndex index = this->index(i);

    dataChanged(index,index, QVector<int>{StatusRole});

}

void ConnectionModel::workStateChanged(int i)
{
    QModelIndex index = this->index(i);

    dataChanged(index,index, QVector<int>{WorkStateRole});
}

void ConnectionModel::precentChanged(int i)
{
    QModelIndex index = this->index(i);

    dataChanged(index,index, QVector<int>{PrecentRole});
}
