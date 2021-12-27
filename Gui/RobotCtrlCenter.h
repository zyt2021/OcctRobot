#ifndef ROBOTCTRLCENTER_H
#define ROBOTCTRLCENTER_H

#include <QWidget>

class Robot;

namespace Ui {
class RobotCtrlCenter;
}

class RobotCtrlCenter : public QWidget
{
    Q_OBJECT

public:
    explicit RobotCtrlCenter(QWidget *parent = nullptr);
    ~RobotCtrlCenter();

    void InitValues(const Robot *robot);

private slots:
    void on_horizontalSlider_baseValue_valueChanged(int value);
    void on_horizontalSlider_shoulderValue_valueChanged(int value);
    void on_horizontalSlider_elbowValue_valueChanged(int value);
    void on_horizontalSlider_wristValue_valueChanged(int value);
    void on_horizontalSlider_wristMidValue_valueChanged(int value);
    void on_horizontalSlider_wristLastValue_valueChanged(int value);

private:
    Ui::RobotCtrlCenter *ui;

    void anglesChanged();

signals:
    void requestForwardKinematic(const QVector<int> &vals);
};

#endif // ROBOTCTRLCENTER_H
