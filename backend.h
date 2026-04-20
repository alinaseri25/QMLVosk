#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QDebug>
#ifdef Q_OS_ANDROID
    #include <QJniObject>
    #include <QCoreApplication>
#endif
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "../../QtLibraries/vosk/voiceengine.h"
#include "GrammerModel.h"

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(GrammerModel *_grammerModel, QObject *parent = nullptr);
    Q_INVOKABLE void testSendMessage(const QString &messsage);

public slots:
    void onQmlLoaded(void);
    void onStartStop(void);

private:
    VoiceEngine *voice = nullptr;
    bool voiceEngineInit = false;
    bool voiceEngineStarted = false;
    GrammerModel *grammerModel;

    QString ensureModelAvailable(const QString &resourcePath, const QString &fileName);
    bool copyFileChunked(const QString &srcPath, const QString &dstPath);
    bool copyDirectoryRecursive(const QString &srcDirPath, const QString &dstDirPath);
    void askForPermission(const QStringList &permissions, int requestCode);

private slots:
    void resultFound(const QJsonDocument finalResultDoc);

signals:
    void voiceEngineChangeState(bool isInit,bool isStarted);
    void newResult(const QString &cmd);
    void showMessage(bool success,const QString &msg);
};

#endif // BACKEND_H
