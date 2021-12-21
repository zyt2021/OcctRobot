#include "DHIndicater.h"
#include <QDebug>

DHIndicater::DHIndicater()
    : myDHData({
                {0, 214, 0, 0},
                {0, 116, M_PI_2, -40},
                {M_PI_2, 0, 0, 345},
                {0, 0, -M_PI_2, 40},
                {0, 340, M_PI_2, 0},
                {M_PI, 0, M_PI_2, 0},
                {0, 221, 0, 61}
               })
{
    for(int i=0;i<myDHData.size()+1;++i) {
        Handle(AIS_Coordinate) aCoord = new AIS_Coordinate;
        aCoord->SetNumber(i);
        myDHCoord.append(aCoord);
    }
}

void DHIndicater::ComputeFK()
{
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
