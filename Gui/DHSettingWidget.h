#ifndef DHSETTINGWIDGET_H
#define DHSETTINGWIDGET_H

#include <QWidget>

#include "robotics/DHIndicater.h"

namespace Ui {
class DHSettingWidget;
}

class DHSettingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DHSettingWidget(QWidget *parent = nullptr);
    ~DHSettingWidget();

private slots:
    void on_pushButton_importDHData_clicked();
    void on_pushButton_saveDHData_clicked();

    void on_tableWidget_DHArgs_cellChanged(int row, int column);

private:
    Ui::DHSettingWidget *ui;
    QVector<DHARG> myData;

signals:
    void requestShow();
};

#endif // DHSETTINGWIDGET_H
