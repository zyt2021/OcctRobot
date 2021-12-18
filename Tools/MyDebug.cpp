#include "MyDebug.h"

#include <QMutex>
#include <QDateTime>
#include <QColor>

void messageHandler(QtMsgType type, const QMessageLogContext& context,
                    const QString &msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    QString str(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    QColor strColor;

    switch (type) {
    case QtDebugMsg:    str.append(" Debug   :"); strColor = Qt::black; break;
    case QtInfoMsg:     str.append(" Info    :"); strColor = Qt::blue; break;
    case QtWarningMsg:  str.append(" Warning :"); strColor = Qt::darkYellow; break;
    case QtCriticalMsg: str.append(" Critical:"); strColor = Qt::darkRed; break;
    case QtFatalMsg:    str.append(" Fatal   :"); strColor = Qt::red; break;
    }

    str.append(" (" + QString(context.category) + "): ");

    MyDebug *instance = MyDebug::Instance();
    instance->debugInfo(str+msg, strColor);
}

MyDebug* MyDebug::ptrSelf = nullptr;
MyDebug* MyDebug::Instance()
{
    if(!ptrSelf) {
        static QMutex muter;
        QMutexLocker clocker(&muter);

        if(!ptrSelf) {
            ptrSelf = new MyDebug();
        }
    }
    return ptrSelf;
}

void MyDebug::installMessageHandler()
{
    qInstallMessageHandler(messageHandler);
}

void MyDebug::uninstallMessageHandler()
{
    qInstallMessageHandler(0);
}

MyDebug::MyDebug(QObject *parent) : QObject(parent)
{
}
