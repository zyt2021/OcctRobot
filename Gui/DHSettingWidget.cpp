#include "DHSettingWidget.h"
#include "ui_DHSettingWidget.h"

#include <QFileDialog>
#include <QTextStream>

DHSettingWidget::DHSettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DHSettingWidget)
{
    ui->setupUi(this);
}

DHSettingWidget::~DHSettingWidget()
{
    delete ui;
}

void DHSettingWidget::on_pushButton_importDHData_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"DH","","*.txt");
    if(fileName.isEmpty())
        return;

    QFile data(fileName);
    QTextStream stream(&data);
    data.open(QIODevice::ReadOnly);
    while(!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList list = line.split(',');
        myData.push_back({list[0].toDouble()*M_PI/180.0,list[1].toDouble(),list[2].toDouble()*M_PI/180.0,list[3].toDouble()});
    }

    ui->tableWidget_DHArgs->blockSignals(true);
    for(int i=0;i<myData.size();++i) {
        ui->tableWidget_DHArgs->setItem(i,0,new QTableWidgetItem(QString::number(myData[i].theta*180/M_PI)));
        ui->tableWidget_DHArgs->setItem(i,1,new QTableWidgetItem(QString::number(myData[i].d)));
        ui->tableWidget_DHArgs->setItem(i,2,new QTableWidgetItem(QString::number(myData[i].alpha*180/M_PI)));
        ui->tableWidget_DHArgs->setItem(i,3,new QTableWidgetItem(QString::number(myData[i].a)));
    }
    ui->tableWidget_DHArgs->blockSignals(false);

    DHIndicater::SetDHData(myData);
    emit requestShow();
}

void DHSettingWidget::on_pushButton_saveDHData_clicked()
{
}

void DHSettingWidget::on_tableWidget_DHArgs_cellChanged(int row, int column)
{
    switch(column) {
    case 0:myData[row].theta = ui->tableWidget_DHArgs->item(row,0)->text().toDouble()*M_PI/180.0;break;
    case 1:myData[row].d = ui->tableWidget_DHArgs->item(row,1)->text().toDouble();break;
    case 2:myData[row].alpha = ui->tableWidget_DHArgs->item(row,2)->text().toDouble()*M_PI/180.0;break;
    case 3:myData[row].a = ui->tableWidget_DHArgs->item(row,3)->text().toDouble();break;
    }

    DHIndicater::SetDHData(myData);
    emit requestShow();
}

