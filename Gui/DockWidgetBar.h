#ifndef DOCKWIDGETBAR_H
#define DOCKWIDGETBAR_H

#include <QFrame>

class QToolButton;
class QLabel;

class DockWidgetBar : public QFrame
{
    Q_OBJECT
public:
    DockWidgetBar();

    QString getText() const;
    void setText(const QString& text);

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;

signals:
    void requestClear();
    void requestSave();

private:
    QToolButton* myClearButton;
    QToolButton* mySaveButton;

    bool myLeftPressed;
    QLabel* myTextLabel;

};

#endif // DOCKWIDGETBAR_H
