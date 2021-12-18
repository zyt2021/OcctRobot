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
void Robot::PerformFK(const QList<double> &angs)
{
    Q_ASSERT(angs.size() == RDOF);

    RTCP = gp_Pnt(0,0,0);
    gp_Trsf CumulativeTrsf;//set an Cumulative Trsf to change the axis after the current one
    for(int i=0;i<RMotionAxis.size();++i)
    {
        gp_Trsf aTrsf;

        double motionVal = qBound(RMinPos[i],angs[i],RMaxPos[i]);

        if(RAxisRotate[i]) {
            aTrsf.SetRotation(RMotionAxis[i],motionVal*M_PI/180);
        }
        else {
            aTrsf.SetTranslation(gp_Vec(RMotionAxis[i].Direction())*motionVal);
        }

        CumulativeTrsf.Multiply(aTrsf);

        gp_Trsf workTrsf;//the real work Trsf ,E * Cumulative * Assemble
        workTrsf.Multiply(CumulativeTrsf);
        workTrsf.Multiply(RAssembelTrsfs[i]);

        //when u change here ,u needn't to display them again in the main function, update the occWidget is enough
        RLinks[i+1]->ApplyTrsf(workTrsf);
    }

    gp_Trsf aTrsf;
    aTrsf.SetRotation(gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1)),0);

    CumulativeTrsf.Multiply(aTrsf);

    gp_Trsf workTrsf;
    workTrsf.Multiply(CumulativeTrsf);
    workTrsf.Multiply(RTCPTrsf);

    RTCP.Transform(workTrsf);
}
