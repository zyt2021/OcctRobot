#ifndef ROBOTLINK_H
#define ROBOTLINK_H

#include <QList>

#include <gp_Trsf.hxx>
#include <gp_Ax1.hxx>

class AIS_ConnectedInteractive;
class RobotCreator;

class RobotLink
{
    friend class RobotCreator;

public:
    RobotLink();
    ~RobotLink();

    QList<Handle(AIS_ConnectedInteractive)> Shapes() const {
        return RShapes;
    }

    void ApplyTrsf(const gp_Trsf &trsf);

private:
    QList<Handle(AIS_ConnectedInteractive)> RShapes;
};

#endif // ROBOTLINK_H
