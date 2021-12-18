#include "RobotCtrlCenter.h"
#include "ui_RobotCtrlCenter.h"

#include <QDebug>

#include <gp_Ax1.hxx>

#include "robotics/Robot.h"

RobotCtrlCenter::RobotCtrlCenter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RobotCtrlCenter)
{
    ui->setupUi(this);
}

RobotCtrlCenter::~RobotCtrlCenter()
{
    delete ui;
}

void RobotCtrlCenter::InitValues(const Robot *robot)
{
    QList<double> minpos = robot->GetAxisMin();
    QList<double> maxpos = robot->GetAxisMax();
    if(minpos.size() != 6) {
        qWarning()<<tr("The number of controler doesn't match robot's dof! ");;
    }
    ui->horizontalSlider_baseValue->setRange(minpos[0],maxpos[0]);
    ui->horizontalSlider_shoulderValue->setRange(minpos[1],maxpos[1]);
    ui->horizontalSlider_elbowValue->setRange(minpos[2],maxpos[2]);
    ui->horizontalSlider_wristValue->setRange(minpos[3],maxpos[3]);
    ui->horizontalSlider_wristMidValue->setRange(minpos[4],maxpos[4]);
    ui->horizontalSlider_wristLastValue->setRange(minpos[5],maxpos[5]);
}

void RobotCtrlCenter::on_horizontalSlider_baseValue_valueChanged(int value)
{
    int base = value;
    int shoulder = ui->horizontalSlider_shoulderValue->value();
    int elbow = ui->horizontalSlider_elbowValue->value();
    int wrist = ui->horizontalSlider_wristValue->value();
    int wristMid = ui->horizontalSlider_wristMidValue->value();
    int wristLast = ui->horizontalSlider_wristLastValue->value();
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}

void RobotCtrlCenter::on_horizontalSlider_shoulderValue_valueChanged(int value)
{
    int base = ui->horizontalSlider_baseValue->value();
    int shoulder = value;
    int elbow = ui->horizontalSlider_elbowValue->value();
    int wrist = ui->horizontalSlider_wristValue->value();
    int wristMid = ui->horizontalSlider_wristMidValue->value();
    int wristLast = ui->horizontalSlider_wristLastValue->value();
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}

void RobotCtrlCenter::on_horizontalSlider_elbowValue_valueChanged(int value)
{
    int base = ui->horizontalSlider_baseValue->value();
    int shoulder = ui->horizontalSlider_shoulderValue->value();
    int elbow = value;
    int wrist = ui->horizontalSlider_wristValue->value();
    int wristMid = ui->horizontalSlider_wristMidValue->value();
    int wristLast = ui->horizontalSlider_wristLastValue->value();
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}

void RobotCtrlCenter::on_horizontalSlider_wristValue_valueChanged(int value)
{
    int base = ui->horizontalSlider_baseValue->value();
    int shoulder = ui->horizontalSlider_shoulderValue->value();
    int elbow = ui->horizontalSlider_elbowValue->value();
    int wrist = value;
    int wristMid = ui->horizontalSlider_wristMidValue->value();
    int wristLast = ui->horizontalSlider_wristLastValue->value();
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}

void RobotCtrlCenter::on_horizontalSlider_wristMidValue_valueChanged(int value)
{
    int base = ui->horizontalSlider_baseValue->value();
    int shoulder = ui->horizontalSlider_shoulderValue->value();
    int elbow = ui->horizontalSlider_elbowValue->value();
    int wrist = ui->horizontalSlider_wristValue->value();
    int wristMid = value;
    int wristLast = ui->horizontalSlider_wristLastValue->value();
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}

void RobotCtrlCenter::on_horizontalSlider_wristLastValue_valueChanged(int value)
{
    int base = ui->horizontalSlider_baseValue->value();
    int shoulder = ui->horizontalSlider_shoulderValue->value();
    int elbow = ui->horizontalSlider_elbowValue->value();
    int wrist = ui->horizontalSlider_wristValue->value();
    int wristMid = ui->horizontalSlider_wristMidValue->value();
    int wristLast = value;
    emit requestForwardKinematic({base,shoulder,elbow,wrist,wristMid,wristLast});
}
