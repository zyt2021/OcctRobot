#pragma once

#include <QObject>

class MyDebug : public QObject
{
    Q_OBJECT
public:
    static MyDebug* Instance();

    static void installMessageHandler();
    static void uninstallMessageHandler();

private:
    MyDebug(QObject* parent = nullptr);
    class CGabor
    {
    public:
        ~CGabor(){
            if (MyDebug::ptrSelf){
                delete MyDebug::ptrSelf;
                MyDebug::ptrSelf = nullptr;
            }
        }
    };

    static MyDebug* ptrSelf;

signals:
    void debugInfo(const QString &str,const QColor &color);
};
