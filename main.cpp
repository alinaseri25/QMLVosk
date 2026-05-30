#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

//----- this part for debug
#include <QFile>
#include <QTextStream>
#include <QDateTime>
//-----

#include "backend.h"
#include "GrammerModel.h"

QFile file;

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    file.setFileName(QString("log %1.txt").arg(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    qInstallMessageHandler(myMessageHandler);

    QGuiApplication app(argc, argv);

    GrammerModel *grammerModel = new GrammerModel(&app);
    Backend *myBackend = new Backend(grammerModel, &app);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty(QString("myBackend"),myBackend);
    engine.rootContext()->setContextProperty(QString("grammerModel"),grammerModel);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("QMLVosk", "Main");

    return app.exec();
}

void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    //QFile file(QString("log %1.txt").arg(QDateTime::currentDateTime().toMSecsSinceEpoch()));
    if(!file.isOpen())
    {
        if(!file.open(QIODevice::Append))
        {
            return;
        }
    }

    QTextStream out(&file);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");

    switch (type) {
    case QtDebugMsg:
        out << "[DEBUG] ";
        break;
    case QtWarningMsg:
        out << "[WARN ] ";
        break;
    case QtCriticalMsg:
        out << "[ERROR] ";
        break;
    case QtFatalMsg:
        out << "[FATAL] ";
        break;
    case QtInfoMsg:
        break;
    }

    out << msg << "\n";

    file.close();
}

