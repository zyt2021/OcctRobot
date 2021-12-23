#ifndef ROBOT_H
#define ROBOT_H

#include <QList>

#include <gp_Trsf.hxx>

class RobotLink;

struct DHArg
{
    double theta;
    double d;
    double alpha;
    double a;
};

class Robot
{
public:
    Robot();
    explicit Robot(int dof);
    ~Robot();

    QVector<RobotLink*> GetLinks() const {
        return RLinks;
    }
    void SetLinkShapes(const QVector<RobotLink*> &shapes) {
        Q_ASSERT(shapes.size() == RDOF+1);

        RLinks = shapes;
    }

    QString GetName() const {
        return RName;
    }
    void SetRobotName(const QString &str) {
        RName = str;
    }

    int GetDOF() const {
        return RDOF;
    }
    void SetDOF(int count) {
        RDOF = count;
    }

    gp_Pnt GetTCP() const {
        return RTCP;
    }

    void SetDHData(const QVector<DHArg>& data) {
        RDHData = data;
    }
    QVector<DHArg> GetDHData() const {
        return RDHData;
    }

    void PerformFK(const QVector<double> &angs);

private:
    int RDOF;
    QString RName;
    QVector<RobotLink*> RLinks;
    QVector<double> RMaxPos;
    QVector<double> RMinPos;
    QVector<DHArg> RDHData;

    gp_Pnt RTCP;

    gp_Trsf dhToTrsf(const DHArg& arg, double rad);
};

#endif // ROBOT_H
