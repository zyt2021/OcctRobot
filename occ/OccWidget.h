#ifndef OCCWIDGET_H
#define OCCWIDGET_H

#include <QWidget>

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <AIS_Manipulator.hxx>

class AIS_InteractiveContext;
class V3d_View;
class AIS_Manipulator;
class AIS_RubberBand;

class OccWidget:public QWidget
{
    Q_OBJECT

public:
    explicit OccWidget(QWidget *parent=nullptr);//set the parent arg to avoid error
    ~OccWidget();

    Handle(V3d_View) GetView()
    {
        return myView;
    }

    Handle(AIS_InteractiveContext) GetContext()
    {
        return myContext;
    }

    Handle(AIS_Manipulator) GetManipulator()
    {
        return myManipulator;
    }

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    QPaintEngine *paintEngine() const;

private:
    Handle(AIS_InteractiveContext) myContext;
    Handle(V3d_View) myView;
    Handle(AIS_Manipulator) myManipulator;
    Handle(AIS_RubberBand) myRubberBand;

    QPoint myPanStartPoint;
    QPoint myRubberStartPoint;

    void drawRubberBand(Standard_Integer theMinX,
                        Standard_Integer theMinY,
                        Standard_Integer theMaxX,
                        Standard_Integer theMaxY,
                        Standard_Boolean theToDraw,
                        Aspect_TypeOfLine theLineType = Aspect_TOL_SOLID);

signals:
    void pickPixel(int x ,int y);
    void selectShapeChanged();
};

#endif // OCCWIDGET_H
