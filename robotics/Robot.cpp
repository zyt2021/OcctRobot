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
    gp_Trsf apply;
    for(int i=0;i<RDHData.size()-1;++i) {
        DHArg dh = RDHData[i];
        gp_Trsf cvt;
        double theta = dh.theta+angs[i]*M_PI/180; double d = dh.d; double alpha = dh.alpha; double a = dh.a;
        cvt.SetValues(cos(theta), -sin(theta)*cos(alpha), sin(theta)*sin(alpha), a*cos(theta),
                      sin(theta), cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta),
                      0, sin(alpha), cos(alpha), d);

        apply.Multiply(cvt);
        RLinks[i+1]->ApplyTrsf(apply);
    }

    gp_Trsf tcp;
    DHArg dh = RDHData.last();
    double theta = dh.theta; double d = dh.d; double alpha = dh.alpha; double a = dh.a;
    tcp.SetValues(cos(theta), -sin(theta)*cos(alpha), sin(theta)*sin(alpha), a*cos(theta),
                  sin(theta), cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta),
                  0, sin(alpha), cos(alpha), d);
    apply.Multiply(tcp);
    RTCP.Transform(apply);
}
