#ifndef CONNECTIONMODEL_H
#define CONNECTIONMODEL_H

#include <QAbstractListModel>

#include "connectionlist.h"

class ConnectionModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ConnectionList *connections READ connections WRITE setConnections)
public:
    explicit ConnectionModel(QObject *parent = nullptr);

    enum{
        AddressRole = Qt::UserRole,
        PortRole,
        StatusRole,
        StateConnectionRole,
        WorkStateRole,
        PrecentRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    void setConnections(ConnectionList *conList);
    ConnectionList *connections() const;


public slots:
    void appendConnectionItem();
    void onConnectClicked(int index, QString address, int port);

    void stateConnectionChanged(int i);
    void workStateChanged(int i);
    void precentChanged(int i);

private:

    ConnectionList *m_Connections = nullptr;


};

#endif // CONNECTIONMODEL_H
