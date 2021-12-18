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
    QList<RobotLink*> list;
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

    // 4.transformation of each link
    QList<gp_Trsf> assembly;
    if (jsonObject.contains("Transforms")) {
        QJsonValue arrayValue = jsonObject.value("Transforms");
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 0; i < array.size(); i++) {
                QJsonValue TrsfArray = array.at(i);
                QJsonObject trsf = TrsfArray.toObject();
                qInfo()<<"x:"<<trsf["x"].toDouble();
                qInfo()<<"y:"<<trsf["y"].toDouble();
                qInfo()<<"z:"<<trsf["z"].toDouble();

                gp_Trsf aTrsf;
                aTrsf.SetTranslationPart({trsf["x"].toDouble(),
                                          trsf["y"].toDouble(),
                                          trsf["z"].toDouble()});
                assembly.append(aTrsf);
            }
        }
    }
    myInputRobot->SetAssembel(assembly);

    // 5.TCP transformation
    gp_Trsf tcpTrsf;
    if (jsonObject.contains("TCPTransform")) {
        QJsonValue tcpValue = jsonObject.value("TCPTransform");
        QJsonObject tcp = tcpValue.toObject();
        qInfo()<<"x:"<<tcp["x"].toDouble();
        qInfo()<<"y:"<<tcp["y"].toDouble();
        qInfo()<<"z:"<<tcp["z"].toDouble();

        tcpTrsf.SetTranslationPart({tcp["x"].toDouble(),
                                    tcp["y"].toDouble(),
                                    tcp["z"].toDouble()});
    }
    myInputRobot->SetTCPTrsf(tcpTrsf);

    // 6.motion axis of each joint
    QList<gp_Ax1> axes;
    QList<bool> types;
    QList<double> max;
    QList<double> min;
    if (jsonObject.contains("Axes")) {
        QJsonValue arrayValue = jsonObject.value("Axes");
        if (arrayValue.isArray()) {
            QJsonArray array = arrayValue.toArray();
            for (int i = 0; i < array.size(); i++) {
                QJsonValue axisArray = array.at(i);
                QJsonObject axis = axisArray.toObject();
                qInfo()<<"x:"<<axis["x"].toDouble();
                qInfo()<<"y:"<<axis["y"].toDouble();
                qInfo()<<"z:"<<axis["z"].toDouble();
                qInfo()<<"type:"<<axis["type"].toString();
                qInfo()<<"max:"<<axis["max"].toDouble();
                qInfo()<<"min:"<<axis["min"].toDouble();

                // axis
                gp_Ax1 ax1(gp_Pnt(assembly[i].TranslationPart()),{axis["x"].toDouble(),
                                                                  axis["y"].toDouble(),
                                                                  axis["z"].toDouble()});
                axes.append(ax1);

                // type
                if(axis["type"].toString() == "rotate") {
                    types.append(true);
                }
                else if(axis["type"].toString() == "translate") {
                    types.append(false);
                }
                else {
                    qCritical()<< QObject::tr("Wrong description of axis type!");
                    return;
                }

                // position limitation
                double maxV = axis["max"].toDouble();
                double minV = axis["min"].toDouble();
                if(maxV<minV) {
                    qCritical()<< QObject::tr("Wrong limitation of axis position!");
                    return;
                }
                else {
                    max.append(maxV);
                    min.append(minV);
                }
            }
        }
    }
    myInputRobot->SetMotionAxis(axes);
    myInputRobot->SetAxisRotate(types);
    myInputRobot->SetAxisMaxPos(max);
    myInputRobot->SetAxisMinPos(min);

    emit createFinish();
}
