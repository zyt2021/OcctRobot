#include "RobotLink.h"

#include <AIS_ConnectedInteractive.hxx>

RobotLink::RobotLink()
{
}

RobotLink::~RobotLink()
{
    for(int i=0;i<RShapes.size();++i) {
        RShapes[i].Nullify();
    }
}

void RobotLink::ApplyTrsf(const gp_Trsf &trsf)
{
    for(int i=0;i<RShapes.size();++i) {
        RShapes[i]->SetLocalTransformation(trsf);
    }
}
