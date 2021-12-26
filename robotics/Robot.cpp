#include "Robot.h"

#include "RobotLink.h"
#include "occ/AIS_Coordinate.h"

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
        apply.Multiply(dhToTrsf(dh,angs[i]*M_PI/180.0));
        RLinks[i+1]->ApplyTrsf(apply);
    }

    DHArg dh = RDHData.last();
    apply.Multiply(dhToTrsf(dh,0));
    RTCP.Transform(apply);
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
