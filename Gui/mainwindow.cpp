#include "mainwindow.h"

#include <QStackedWidget>
#include <QDockWidget>
#include <QDebug>
#include <QThread>
#include <QApplication>
#include <QTextEdit>

#include "occ/OccWidget.h"
#include "occ/ModelImport.h"
#include "robotics/Robot.h"
#include "robotics/RobotLink.h"
#include "robotics/RobotCreator.h"
#include "Tools/MyDebug.h"
#include "MenuToolBar.h"
#include "DockWidgetBar.h"
#include "FileViewer.h"
#include "RobotCtrlCenter.h"
#include "DHSettingWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      myCenterStack(nullptr),
      myGlobalInfo(nullptr),
      myImportThread(nullptr),
      myImportWork(nullptr),
      myOccWidget(nullptr),
      myRobotCreator(nullptr),
      myWorkRobot(nullptr),
      myRobotCtrl(nullptr),
      myDHSet(nullptr)
{
    InitDock();
    InitView();
    InitFuncs();
}

MainWindow::~MainWindow()
{
    if(myImportThread) {
        if(myImportThread->isRunning()) {
            myImportThread->quit();
            myImportThread->wait();
        }
        delete myImportThread;
    }

    if(myImportWork) {
        delete myImportWork;
    }

    if(myOccWidget) {
        delete myOccWidget;
    }

    if(myRobotCreator) {
        if(myRobotCreator->isRunning()) {
            myRobotCreator->quit();
            myRobotCreator->wait();
        }
        delete myRobotCreator;
    }

    if(myWorkRobot)
        delete myWorkRobot;
}

void MainWindow::InitDock()
{
    // set to disable the contextmenu at menubar
    this->setContextMenuPolicy(Qt::NoContextMenu);

    // 1.install the left main menu
    MenuToolBar::Install(this);
    connect(MenuToolBar::Instance(),&MenuToolBar::requestImport,this,[=]() {
        myCenterStack->setCurrentIndex(1);
    });
    connect(MenuToolBar::Instance(),&MenuToolBar::requestOccView,this,[=]() {
        myCenterStack->setCurrentIndex(0);
    });
    connect(MenuToolBar::Instance(),&MenuToolBar::requestCtrlCenter,this,[=](bool show) {
        myRobotCtrl->setVisible(show);
    });
    connect(MenuToolBar::Instance(),&MenuToolBar::requestTest,this,[=](bool show) {
        myDHSet->setVisible(show);
    });

    // 2.set the output info textedit
    myGlobalInfo = new QTextEdit(this);
    myGlobalInfo->setFontPointSize(10);

    // set a dockwidget for the textedit
    DockWidgetBar *infoDockBar = new DockWidgetBar;
    infoDockBar->setText(tr("Output Info"));
    QDockWidget *infoDock = new QDockWidget(tr("Output Info"),this);
    infoDock->setWidget(myGlobalInfo);
    infoDock->setTitleBarWidget(infoDockBar);
    infoDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    this->addDockWidget(Qt::BottomDockWidgetArea,infoDock);

    // connect the signals of dockwidget bar
    connect(infoDockBar,&DockWidgetBar::requestClear,myGlobalInfo,&QTextEdit::clear);
    connect(infoDockBar,&DockWidgetBar::requestSave,this,&MainWindow::onSaveOutput);

    MyDebug::Instance()->installMessageHandler();
    connect(MyDebug::Instance(),&MyDebug::debugInfo,this,[=](const QString &str, const QColor &color) {
        myGlobalInfo->setTextColor(color);
        myGlobalInfo->append(str);
    });
}

void MainWindow::InitView()
{
    QFile styleFile(":/Gui.qss");
    styleFile.open(QIODevice::Text|QIODevice::ReadOnly);
    QString styleStr = styleFile.readAll();
    this->setStyleSheet(styleStr);

    // 1.the center window is stacked to be changable
    myCenterStack = new QStackedWidget(this);    

    // 2.the first stacked is occ view
    myOccWidget = new OccWidget();
    myCenterStack->addWidget(myOccWidget);

    // 3.the second stacked is system file viewer
    FileViewer *aViewer = new FileViewer(this);
    myCenterStack->addWidget(aViewer);
    connect(aViewer,&FileViewer::requestImportCAD,this,&MainWindow::onImportCAD);
    connect(aViewer,&FileViewer::requestImportRobot,this,&MainWindow::onImportRobot);

    // 4.init the control widget of robot
    myRobotCtrl = new RobotCtrlCenter(myOccWidget);
    myRobotCtrl->move(20,20);
    myRobotCtrl->hide();
    connect(myRobotCtrl,&RobotCtrlCenter::requestForwardKinematic,this,[=](const QVector<int> &vals) {
        if(myWorkRobot) {
            myWorkRobot->PerformFK({static_cast<double>(vals[0]),
                                    static_cast<double>(vals[1]),
                                    static_cast<double>(vals[2]),
                                    static_cast<double>(vals[3]),
                                    static_cast<double>(vals[4]),
                                    static_cast<double>(vals[5])});
            myOccWidget->GetView()->Update();
        }
    });

    // 5.DH setting widget
    myDHSet = new DHSettingWidget(myOccWidget);
    myDHSet->move(20,20);
    myDHSet->hide();
    connect(myDHSet,&DHSettingWidget::requestShow,this,[=]() {
        DHIndicater::ComputeFK();
        QVector<Handle(AIS_Coordinate)> coords = DHIndicater::GetIndicaters();
        for(int i=0;i<coords.size();++i) {
            myOccWidget->GetContext()->Display(coords[i],Standard_True);
        }
        myOccWidget->GetView()->FitAll();
    });

    // set the central widget and show
    this->setCentralWidget(myCenterStack);
    this->showMaximized();
}

void MainWindow::InitFuncs()
{
    // 1. put the CAD file import work into a thread
    myImportThread = new QThread;
    myImportWork = new ModelImport;
    connect(this,&MainWindow::requestImportModel,myImportWork,&ModelImport::ReadAModelFile);
    myImportWork->moveToThread(myImportThread);

    connect(myImportWork,QOverload<const QList<Handle(AIS_ConnectedInteractive)> &>::of(&ModelImport::importFinish)
            ,this,[=](const QList<Handle(AIS_ConnectedInteractive)> &list) {
        foreach(Handle(AIS_ConnectedInteractive) obj, list) {
            myOccWidget->GetContext()->Display(obj,Standard_False);
        }
        myOccWidget->GetView()->FitAll();
    });
    connect(myImportWork,QOverload<const Handle(AIS_Shape) &>::of(&ModelImport::importFinish)
            ,this,[=](const Handle(AIS_Shape) &result) {

        myOccWidget->GetContext()->Display(result,Standard_False);
        myOccWidget->GetView()->FitAll();
    });

    // 2. init the robot creating thread
    myRobotCreator = new RobotCreator;
    connect(myRobotCreator,&RobotCreator::createFinish,this,&MainWindow::onRobotCreateFinish);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete) {
        Handle(AIS_InteractiveContext) myContext = myOccWidget->GetContext();
        for (myContext->InitSelected(); myContext->MoreSelected(); myContext->InitSelected())
        {
            myContext->Remove(myContext->SelectedInteractive(), Standard_False);
        }
        myOccWidget->GetView()->Update();
        qInfo()<< tr("Delete selected shapes");
    }
}

void MainWindow::onImportCAD(const QString &str)
{
    if(!myImportThread) {
        return;
    }

    myImportThread->start();
    emit requestImportModel(str);
}

void MainWindow::onImportRobot(const QString &str)
{
    if(myWorkRobot) {
        QVector<RobotLink*> links = myWorkRobot->GetLinks();
        foreach(RobotLink* aLink, links) {
            foreach(Handle(AIS_ConnectedInteractive) obj, aLink->Shapes()) {
                myOccWidget->GetContext()->Remove(obj,Standard_False);
            }
        }

        delete myWorkRobot;
        myWorkRobot = nullptr;
    }

    myWorkRobot = new Robot;
    myRobotCreator->SetJsonPath(str);
    myRobotCreator->SetInput(myWorkRobot);
    myRobotCreator->start();
}

void MainWindow::onRobotCreateFinish()
{
    if(!myWorkRobot) {
        return;
    }

    if(myRobotCtrl) {
//        myRobotCtrl->InitValues(myWorkRobot);
    }

    myWorkRobot->PerformFK({30,30,30,30,30,30});

    QVector<RobotLink*> links = myWorkRobot->GetLinks();
    foreach(RobotLink* aLink, links) {
        foreach(Handle(AIS_ConnectedInteractive) obj, aLink->Shapes()) {
            myOccWidget->GetContext()->Display(obj,Standard_False);
            myOccWidget->GetContext()->Deactivate(obj);
            qApp->processEvents();
        }
    }
    myOccWidget->GetView()->FitAll();
}

void MainWindow::onSaveOutput()
{
    qDebug()<<"Save output function is a todo one......";
}
