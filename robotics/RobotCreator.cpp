#include "RobotCreator.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Robot.h"
#include "RobotLink.h"
#include "occ/ModelImport.h"

#include <QDebug>

RobotCreator::RobotCreator(QObject *parent)
    : QThread(parent),
      myInputRobot(nullptr)
{
}

RobotCreator::RobotCreator(const QString& jsonPath, QObject *parent)
    : QThread(parent),
      myJsonName(jsonPath),
      myInputRobot(nullptr)
{
}

void RobotCreator::run()
{
    LoadJson(myJsonName);
}

void RobotCreator::LoadJson(const QString &fileName)
{
    if(!myInputRobot)
        return;

    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString value = file.readAll();
    file.close();

    QJsonParseError parseJsonErr;
    QJsonDocument document = QJsonDocument::fromJson(value.toUtf8(), &parseJsonErr);
    if (parseJsonErr.error != QJsonParseError::NoError) {
        qCritical()<<parseJsonErr.errorString();
        return;
    }

    QJsonObject jsonObject = document.object();    

    // 1.dof of robot
    qInfo()<<"DOF:"<<jsonObject["DOF"].toInt();
    myInputRobot->SetDOF(jsonObject["DOF"].toInt());

    // 2.robot's name
    qInfo()<<"name:"<<jsonObject["Name"].toString();
    myInputRobot->SetRobotName(jsonObject["Name"].toString());

    // 3.shape files' paths
    QVector<RobotLink*> list;
    if (jsonObject.contains("Shapes")) {
        QJsonValue arrayValue = jsonObject.value("Shapes");
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 0; i < array.size(); i++) {
                QJsonValue pathArray = array.at(i);
                QJsonObject path = pathArray.toObject();
                RobotLink *aLink = new RobotLink;

                qInfo()<<"Path:"<<path["Path"].toString();
                QFileInfo aInfo(path["Path"].toString());

                bool ret = false;
                if(aInfo.isFile()) {
                    if(aInfo.suffix().toLower() == "step" || aInfo.suffix().toLower() == "stp") {
                        ret = ModelImport::importSTEP(path["Path"].toString(),aLink->RShapes);
                    }
                    else if(aInfo.suffix().toLower() == "iges" || aInfo.suffix().toLower() == "igs") {
                        ret = ModelImport::importIGES(path["Path"].toString(),aLink->RShapes);
                    }
                    else {
                        qCritical()<< QObject::tr("The format of robot shape file is unsupported!");
                        return;
                    }
                }
                else {
                    qCritical()<< QObject::tr("The element %1 is not a file!").arg(path["Path"].toString());
                    return;
                }

                if(!ret) {
                    qCritical()<< QObject::tr("Error while import robot shapes!");
                    return;
                }
                else {
                    list.append(aLink);
                }
            }
        }
    }
    myInputRobot->SetLinkShapes(list);

    // 4.DH arguments of each link
    QVector<DHArg> dhArgs;
    if (jsonObject.contains("DH")) {
        QJsonValue arrayValue = jsonObject.value("DH");
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 0; i < array.size(); i++) {
                QJsonValue TrsfArray = array.at(i);
                QJsonObject trsf = TrsfArray.toObject();
                qInfo()<<"theta:"<<trsf["theta"].toDouble()
                      <<"d:"<<trsf["d"].toDouble()
                     <<"alpha:"<<trsf["alpha"].toDouble()
                    <<"a:"<<trsf["a"].toDouble();

                dhArgs.append({trsf["theta"].toDouble()*M_PI/180.0,
                              trsf["d"].toDouble(),
                              trsf["alpha"].toDouble()*M_PI/180.0,
                              trsf["a"].toDouble()});
            }
        }
    }

    // 5.limits of each joint
    QVector<double> mins, maxs;
    if (jsonObject.contains("Limits")) {
        QJsonValue arrayValue = jsonObject.value("Limits");
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 0; i < array.size(); i++) {
                QJsonValue alimitArray = array.at(i);
                QJsonObject limit = alimitArray.toObject();
                qInfo()<<"min:"<<limit["min"].toDouble()
                      <<"max:"<<limit["max"].toDouble();

                mins.append(limit["min"].toDouble());
                maxs.append(limit["max"].toDouble());
            }
        }
    }
    myInputRobot->SetMaxPosition(maxs);
    myInputRobot->SetMinPosition(mins);

    // 6.TCP transformation
    if (jsonObject.contains("TCP")) {
        QJsonValue tcpValue = jsonObject.value("TCP");
        QJsonObject tcp = tcpValue.toObject();
        qInfo()<<"theta:"<<tcp["theta"].toDouble()
              <<"d:"<<tcp["d"].toDouble()
             <<"alpha:"<<tcp["alpha"].toDouble()
            <<"a:"<<tcp["a"].toDouble();

        dhArgs.append({tcp["theta"].toDouble()*M_PI/180.0,
                       tcp["d"].toDouble(),
                       tcp["alpha"].toDouble()*M_PI/180.0,
                       tcp["a"].toDouble()});
    }
    myInputRobot->SetDHData(dhArgs);

    emit createFinish();
}
