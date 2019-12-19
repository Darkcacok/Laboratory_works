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

   qDebug() << "Address: ";
   for(auto &address: conf.address)
   {
       QString name, type, ip;
       std::tie(name, type, ip) = address;
       qDebug() << "          " << name<< ": " << type << ": " << ip;
   }

//    qDebug() << "Address: " << conf.address;
    qDebug() << "Port:    " << conf.port;


    return a.exec();
}
