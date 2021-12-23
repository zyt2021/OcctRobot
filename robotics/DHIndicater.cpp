#include "DHIndicater.h"
#include <QDebug>
#include <QMutex>

DHIndicater* DHIndicater::ptrSelf = nullptr;
QVector<DHARG> DHIndicater::myDHData = {};
QVector<Handle(AIS_Coordinate)> DHIndicater::myDHCoord = {};

DHIndicater::DHIndicater()
{
}

DHIndicater *DHIndicater::Instance()
{
    if(!ptrSelf) {
        static QMutex muter;
        QMutexLocker clocker(&muter);

        if(!ptrSelf) {
            ptrSelf = new DHIndicater();
        }
    }
    return ptrSelf;
}

void DHIndicater::ComputeFK()
{
    if(myDHData.isEmpty())
        return;

    gp_Trsf apply;
    for(int i=0;i<myDHData.size();++i) {
        DHARG dh = myDHData[i];
        gp_Trsf cvt;
        double theta = dh.theta; double d = dh.d; double alpha = dh.alpha; double a = dh.a;
        cvt.SetValues(cos(theta), -sin(theta)*cos(alpha), sin(theta)*sin(alpha), a*cos(theta),
                      sin(theta), cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta),
                      0, sin(alpha), cos(alpha), d);

        apply.Multiply(cvt);
        myDHCoord[i+1]->SetLocalTransformation(apply);
    }
}

void DHIndicater::SetDHData(const QVector<DHARG> &data)
{
    myDHData = data;
    if(myDHCoord.size() != myDHData.size()+1) {
        myDHCoord.clear();
        for(int i=0;i<myDHData.size()+1;++i) {
            Handle(AIS_Coordinate) aCoord = new AIS_Coordinate;
            aCoord->SetNumber(i);
            myDHCoord.append(aCoord);
        }
    }
}
