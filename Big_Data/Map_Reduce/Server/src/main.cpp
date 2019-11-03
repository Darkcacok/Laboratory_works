#include <QCoreApplication>
#include "QDebug"

#include "server.h"

int main(int argc, char **argv)
{
    QCoreApplication a(argc,argv);

    TcpServer *server = new TcpServer();

    if(server->listen())
    {
        qDebug() << "Start listening";
    }
    else
    {
        qDebug() << "Failed to start listening";
        exit(-1);
    }

    TcpServer::Config conf = server->getConfig();

    qDebug() << "Address: " << conf.address;
    qDebug() << "Port:    " << conf.port;


    return a.exec();
}