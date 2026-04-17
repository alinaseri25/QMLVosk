#include "backend.h"

Backend::Backend(GrammerModel *_grammerModel, QObject *parent)
    : QObject{parent}
{
    grammerModel = _grammerModel;
}

void Backend::testSendMessage(const QString &messsage)
{
    qDebug() << "message : " << messsage;
}

void Backend::onQmlLoaded()
{
    if(voice != nullptr)
    {
        voice->stop();
        voice->deleteLater();
        voice = nullptr;
    }
    voice = new VoiceEngine(nullptr);
    connect(voice,&VoiceEngine::finalResult,this,&Backend::resultFound);
    QString modelOnDisk;

#ifdef ANDROID
    modelOnDisk = ensureModelAvailable(QString("assets:/models/vosk-model-small-fa-0.5"),QString("vosk-model-small-fa-0.5"));
    QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
        return;

    QJniObject::callStaticMethod<void>(
        "org/verya/QMLVosk/MainActivity",
        "manageScreenAndWakeLock",
        "(Landroid/content/Context;ZZ)V",
        context.object(),
        (jboolean)true,  // screenAlwaysOn
        (jboolean)true    // wakeLock
        );

    QJniObject::callStaticMethod<void>(
        "org/verya/QMLVosk/MainActivity",
        "setDimTimeoutFromQt",
        "(J)V",
        0
        );
#else
    modelOnDisk = ensureModelAvailable(QString("./vosk-model-small-fa-0.5"),QString("vosk-model-small-fa-0.5"));
#endif

    if(modelOnDisk.isEmpty())
    {
        emit showMessage(false,QString("cannot find vosk model\nPlease Reopen App!"));
        return;
    }

    qDebug() << QString("modelOnDisk : %1").arg(modelOnDisk);
    voiceEngineInit = voice->initialize(modelOnDisk);
    qDebug() << "VOICE INIT:" << voiceEngineInit;
    if(voiceEngineInit)
    {
        emit showMessage(true,QString("Voice Engine Ready To start"));
    }
    else
    {
        emit showMessage(false,QString("Voice Engine Not Ready\nPlease Reopen App!"));
    }

    emit voiceEngineChangeState(voiceEngineInit,voiceEngineStarted);

}

void Backend::onStartStop()
{
    if(voiceEngineInit)
    {
        if(voiceEngineStarted)
        {
            voice->disableGrammar();
            voice->stop();
            voiceEngineStarted = false;
        }
        else
        {
            voiceEngineStarted = voice->start();
            if(grammerModel->rowCount() > 0)
            {
                voice->enableGrammar(grammerModel->toList());
            }
        }
    }
    else
    {
        onQmlLoaded();
    }
    emit voiceEngineChangeState(voiceEngineInit,voiceEngineStarted);
}

QString Backend::ensureModelAvailable(const QString &resourcePath, const QString &fileName)
{
    // مسیر قابل نوشتن
    QString targetRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (targetRoot.isEmpty()) {
        qWarning() << "No writable location available!";
        return QString();
    }

    QDir().mkpath(targetRoot);

    // تشخیص اینکه resourcePath فایل است یا فولدر
    QFileInfo info(resourcePath);

    if (!info.exists()) {
        qWarning() << "Resource path does NOT exist in QRC:" << resourcePath;
        return QString();
    }

    // ============================================================
    // حالت 1: منبع یک فایل است
    // ============================================================
    if (info.isFile()) {
        QString targetFile = QDir(targetRoot).filePath(fileName);

        if (QFile::exists(targetFile)) {
            qDebug() << "File already exists:" << targetFile;
            return targetFile;
        }

        if (!copyFileChunked(resourcePath, targetFile)) {
            qWarning() << "Failed copying file:" << resourcePath;
            return QString();
        }

        qDebug() << "File copied:" << targetFile;
        return targetFile;
    }

    // ============================================================
    // حالت 2: منبع فولدر است → کپی بازگشتی
    // ============================================================
    if (info.isDir()) {
        QString targetDir = QDir(targetRoot).filePath(info.fileName());
        if (!QDir().mkpath(targetDir)) {
            qWarning() << "Cannot create target directory:" << targetDir;
            return QString();
        }

        if (!copyDirectoryRecursive(resourcePath, targetDir)) {
            qWarning() << "Error copying directory recursively";
            return QString();
        }

        qDebug() << "Folder copied to:" << targetDir;
        return targetDir;
    }

    qWarning() << "Invalid resource path (not file/dir):" << resourcePath;
    return QString();
}

bool Backend::copyFileChunked(const QString &srcPath, const QString &dstPath)
{
    QFile src(srcPath);
    if (!src.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open src:" << srcPath;
        return false;
    }

    QFile dst(dstPath);
    if (!dst.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot open dst:" << dstPath;
        return false;
    }

    const qint64 chunkSize = 256 * 1024;
    QByteArray buffer;
    buffer.resize(chunkSize);

    while (true) {
        qint64 r = src.read(buffer.data(), chunkSize);
        if (r < 0) return false;
        if (r == 0) break;

        if (dst.write(buffer.constData(), r) != r)
            return false;
    }

    dst.close();
    src.close();
    return true;
}

bool Backend::copyDirectoryRecursive(const QString &srcDirPath, const QString &dstDirPath)
{
    QDir srcDir(srcDirPath);

    if (!srcDir.exists()) {
        qWarning() << "Source directory does not exist:" << srcDirPath;
        return false;
    }

    QDir dstDir(dstDirPath);
    if (!dstDir.exists()) {
        if (!QDir().mkpath(dstDirPath))
            return false;
    }

    // فایل‌های داخل فولدر
    for (const QFileInfo &fi : srcDir.entryInfoList(QDir::Files)) {
        QString srcFile = fi.filePath();
        QString dstFile = dstDir.filePath(fi.fileName());

        if (!copyFileChunked(srcFile, dstFile))
            return false;
    }

    // ساب‌فولدرها
    for (const QFileInfo &fi : srcDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString subSrc = fi.filePath();
        QString subDst = dstDir.filePath(fi.fileName());

        if (!copyDirectoryRecursive(subSrc, subDst))
            return false;
    }

    return true;
}

void Backend::resultFound(QString text)
{
    emit newResult(text);
}
