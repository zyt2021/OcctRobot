#include "MenuToolBar.h"

#include <QMainWindow>
#include <QMutex>
#include <QToolButton>

MenuToolBar* MenuToolBar::ptrSelf = nullptr;

MenuToolBar::MenuToolBar(QWidget *parent) : QToolBar(tr("Menu ToolBar"),parent)
{
    this->setAllowedAreas(Qt::LeftToolBarArea);
    this->setMovable(false);
}

void MenuToolBar::initStyle()
{
    QMainWindow* window = static_cast<QMainWindow*>(Instance()->parent());
    if(!window) {
        return;
    }

    // 1.set the width
    int widP = window->width();
    Instance()->setFixedWidth(widP/10);

    // 2.add the button
    dummySpacing(); // add a dummy widget to avoid the button on the most top

    QToolButton* importBtn = new QToolButton(Instance());
    importBtn->setText(tr("Import"));
    importBtn->setIcon(QIcon(":/icons/import.png"));
    importBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    importBtn->setFixedSize(widP/10,widP/10);
    importBtn->setCheckable(true);
    Instance()->addWidget(importBtn);

    QToolButton* viewBtn = new QToolButton(Instance());
    viewBtn->setText(tr("View"));
    viewBtn->setIcon(QIcon(":/icons/occView.png"));
    viewBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    viewBtn->setFixedSize(widP/10,widP/10);
    viewBtn->setCheckable(true);
    viewBtn->setChecked(true);
    Instance()->addWidget(viewBtn);

    QToolButton* ctrlBtn = new QToolButton(Instance());
    ctrlBtn->setText(tr("Control"));
    ctrlBtn->setIcon(QIcon(":/icons/control.png"));
    ctrlBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ctrlBtn->setFixedSize(widP/10,widP/10);
    ctrlBtn->setCheckable(true);
    Instance()->addWidget(ctrlBtn);

    QToolButton* testBtn = new QToolButton(Instance());
    testBtn->setText(tr("Test"));
    testBtn->setIcon(QIcon(":/icons/plus.png"));
    testBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    testBtn->setFixedSize(widP/10,widP/10);
    testBtn->setCheckable(true);
    Instance()->addWidget(testBtn);

    // 3.connect the signals
    connect(importBtn,&QToolButton::clicked,Instance(),[=](bool checked) {
        if(checked) {
            viewBtn->setChecked(false);
            emit Instance()->requestImport();
        }
        else {
            importBtn->setChecked(true); // can't be unchecked by itself
        }
    });
    connect(viewBtn,&QToolButton::clicked,Instance(),[=](bool checked) {
        if(checked) {
            importBtn->setChecked(false);
            emit Instance()->requestOccView();
        }
        else {
            viewBtn->setChecked(true);
        }
    });
    connect(ctrlBtn,&QToolButton::clicked,Instance(),[=](bool checked) {
        emit Instance()->requestCtrlCenter(checked);
        if(checked) {
            viewBtn->click();
        }
    });
    connect(testBtn,&QToolButton::clicked,Instance(),[=](bool checked) {
        emit Instance()->requestTest();
    });
}

void MenuToolBar::dummySpacing()
{
    QWidget* empty = new QWidget(Instance());
    empty->setFixedSize(Instance()->width(),
                        Instance()->width()/4);
    Instance()->addWidget(empty);
}

void MenuToolBar::Install(QMainWindow *parent)
{
    if(parent) {
        parent->addToolBar(Qt::LeftToolBarArea,Instance());
        initStyle();
    }
}

MenuToolBar *MenuToolBar::Instance()
{
    if(!ptrSelf) {
        static QMutex muter;
        QMutexLocker clocker(&muter);

        if(!ptrSelf) {
            ptrSelf = new MenuToolBar();
        }
    }
    return ptrSelf;
}
