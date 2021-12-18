#include "DockWidgetBar.h"

#include <QMouseEvent>
#include <QCoreApplication>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QMenu>

DockWidgetBar::DockWidgetBar() : QFrame(nullptr)
{
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);

    layout->setContentsMargins(3, 1, 3, 1);

    myTextLabel = new QLabel(this);

    layout->addWidget(myTextLabel);

    myClearButton = new QToolButton(this);
    myClearButton->setToolTip(tr("Clear"));
    myClearButton->setIcon(QIcon(":/icons/clear.png"));
    layout->addWidget(myClearButton);
    connect(myClearButton,&QToolButton::clicked,this,[=]() {
        emit requestClear();
    });

    mySaveButton = new QToolButton(this);
    mySaveButton->setToolTip(tr("Save"));
    mySaveButton->setIcon(QIcon(":/icons/save.png"));
    layout->addWidget(mySaveButton);
    connect(mySaveButton,&QToolButton::clicked,this,[=]() {
        emit requestSave();
    });

    layout->addStretch(1);
}

QString DockWidgetBar::getText() const
{
    return myTextLabel->text();
}

void DockWidgetBar::setText(const QString &text)
{
    myTextLabel->setText(text+"   ");
}

void DockWidgetBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        myLeftPressed = true;
    }

    QFrame::mousePressEvent(event);
}

void DockWidgetBar::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        myLeftPressed = false;
    }

    QFrame::mouseReleaseEvent(event);
}

void DockWidgetBar::mouseMoveEvent(QMouseEvent *event)
{
    if(myLeftPressed)
    {
        QDockWidget* dockWidget = static_cast<QDockWidget*>(parentWidget());
        if(dockWidget != nullptr)
        {
            myLeftPressed = false;

            QPoint pos = event->pos();

            QRect rect = this ->rect();

            if (dockWidget->features() & QDockWidget::DockWidgetVerticalTitleBar) {
                QPoint p = pos;
                pos.setX(rect.left() + rect.bottom() - p.y());
                pos.setY(rect.top() + p.x() - rect.left());

                QSize s = rect.size();
                s.transpose();
                rect.setSize(s);
            }

            const int buttonRight = 10;
            const int buttonWidth = 20;
            int right = rect.right() - pos.x();
            int button = (right - buttonRight)/buttonWidth;


            switch (button) {
            case 0:
                event->accept();
                dockWidget->close();
                break ;
            case 1:
                event->accept();
                dockWidget->setFloating(!dockWidget->isFloating());
                break ;
            case 2: {
                event->accept();
                QDockWidget::DockWidgetFeatures features = dockWidget->features();
                if (features & QDockWidget::DockWidgetVerticalTitleBar)
                    features &= ~QDockWidget::DockWidgetVerticalTitleBar;
                else
                    features |= QDockWidget::DockWidgetVerticalTitleBar;
                dockWidget->setFeatures(features);
                break ;
            }
            default :
                event->ignore();
                break ;
            }
        }
    }

    QFrame::mouseMoveEvent(event);
}
