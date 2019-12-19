#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>

#include <QtWidgets/QApplication>

#include "connectionmodel.h"
#include "connectionlist.h"
#include "worker.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    qmlRegisterType<ConnectionModel>("Connection", 1, 0, "ConnectionModel");
    qmlRegisterUncreatableType<ConnectionList>("Connection", 1, 0, "ConnetionList",
                                         QStringLiteral("ConnectionList should not be created in QML"));

    ConnectionList conList;
    QThread thread;
    Worker worker;
    worker.setConnections(&conList);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("connectionList"), &conList);
    engine.rootContext()->setContextProperty(QStringLiteral("worker"), &worker);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if(engine.rootObjects().isEmpty())
        return -1;

    return a.exec();
}
