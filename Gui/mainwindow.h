#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

class OccWidget;
class Robot;
class ModelImport;
class RobotCreator;
class RobotCtrlCenter;
class QThread;
class QTextEdit;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitDock();
    void InitView();
    void InitFuncs();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onImportCAD(const QString &str);
    void onImportRobot(const QString &str);
    void onRobotCreateFinish();
    void onSaveOutput();

private:
    QStackedWidget *myCenterStack;
    QTextEdit *myGlobalInfo;
    QThread *myImportThread;

    ModelImport *myImportWork;
    OccWidget *myOccWidget;
    RobotCreator *myRobotCreator;
    Robot *myWorkRobot;
    RobotCtrlCenter *myRobotCtrl;

signals:
    void requestImportModel(const QString &str);
};
#endif // MAINWINDOW_H
