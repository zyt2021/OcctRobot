#ifndef ROBOT_H
#define ROBOT_H

#include <QList>

#include <gp_Trsf.hxx>

class RobotLink;

class Robot
{
public:
    Robot();
    explicit Robot(int dof);
    ~Robot();

    QList<RobotLink*> GetLinks() const {
        return RLinks;
    }

    QString GetName() const {
        return RName;
    }
    int GetDOF() const {
        return RDOF;
    }
    gp_Pnt GetTCP() const {
        return RTCP;
    }
    QList<double> GetAxisMax() const {
        return RMaxPos;
    }
    QList<double> GetAxisMin() const {
        return RMinPos;
    }
    QList<bool> GetAxisType() const {
        return RAxisRotate;
    }

    void SetRobotName(const QString &str) {
        RName = str;
    }
    void SetDOF(int count) {
        RDOF = count;
    }
    void SetLinkShapes(const QList<RobotLink*> &shapes) {
        Q_ASSERT(shapes.size() == RDOF+1);

        RLinks = shapes;
    }
    void SetMotionAxis(const QList<gp_Ax1> &axis) {
        Q_ASSERT(axis.size() == RDOF);

        RMotionAxis = axis;
    }
    void SetAxisRotate(const QList<bool> &types) {
        Q_ASSERT(types.size() == RDOF);

        RAxisRotate = types;
    }
    void SetAxisMaxPos(const QList<double> &values) {
        Q_ASSERT(values.size() == RDOF);

        RMaxPos = values;
    }
    void SetAxisMinPos(const QList<double> &values) {
        Q_ASSERT(values.size() == RDOF);

        RMinPos = values;
    }
    void SetAssembel(const QList<gp_Trsf> &trsfs) {
        Q_ASSERT(trsfs.size() == RDOF);

        RAssembelTrsfs = trsfs;
    }
    void SetTCPTrsf(const gp_Trsf &trsf) {
        RTCPTrsf = trsf;
    }

    void PerformFK(const QList<double> &angs);

private:
    int RDOF;
    QString RName;
    QList<RobotLink*> RLinks;
    QList<gp_Ax1> RMotionAxis;
    QList<bool> RAxisRotate;
    QList<double> RMaxPos;
    QList<double> RMinPos;
    QList<gp_Trsf> RAssembelTrsfs;
    gp_Trsf RTCPTrsf;

    gp_Pnt RTCP;
};

#endif // ROBOT_H
