#include "Robot.h"

#include "RobotLink.h"

Robot::Robot() : RDOF(0)
{
}

Robot::Robot(int dof) : RDOF(dof)
{
}

Robot::~Robot()
{
    for(int i=0;i<RLinks.size();++i) {
        delete RLinks[i];
    }
}

// robot forward kinematic
void Robot::PerformFK(const QVector<double> &angs)
{
    Q_ASSERT(angs.size() == RDOF);

    if(RDHData.isEmpty())
        return;

    RTCP = gp_Pnt(0,0,0);
    gp_Trsf T0,T1,T2,T3,T4,T5,T6;
    T0 = dhToTrsf(RDHData[0],0);
    T1 = dhToTrsf(RDHData[1],angs[0]*M_PI/180.0);
    T2 = dhToTrsf(RDHData[2],angs[1]*M_PI/180.0);
    T3 = dhToTrsf(RDHData[3],angs[2]*M_PI/180.0);
    T4 = dhToTrsf(RDHData[4],angs[3]*M_PI/180.0);
    T5 = dhToTrsf(RDHData[5],angs[4]*M_PI/180.0);
    T6 = dhToTrsf(RDHData[6],angs[5]*M_PI/180.0);

    gp_Trsf temp = T0;
    temp = T0.Multiplied(T1);
    RLinks[1]->ApplyTrsf(temp);
    temp = temp.Multiplied(T2);
    RLinks[2]->ApplyTrsf(temp);
    temp = temp.Multiplied(T3);
    RLinks[3]->ApplyTrsf(temp);
    temp = temp.Multiplied(T4);
    RLinks[4]->ApplyTrsf(temp);
    temp = temp.Multiplied(T5);
    RLinks[5]->ApplyTrsf(temp);
    temp = temp.Multiplied(T6);
    RLinks[6]->ApplyTrsf(temp);
}

gp_Trsf Robot::dhToTrsf(const DHArg &arg, double rad)
{
    gp_Trsf cvt;
    double theta = arg.theta+rad; double d = arg.d; double alpha = arg.alpha; double a = arg.a;
    cvt.SetValues(cos(theta), -sin(theta)*cos(alpha), sin(theta)*sin(alpha), a*cos(theta),
                  sin(theta), cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta),
                  0, sin(alpha), cos(alpha), d);

    return cvt;
}
