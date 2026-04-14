#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

#include "backend.h"
#include "GrammerModel.h"

int main(int argc, char *argv[])
{
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
