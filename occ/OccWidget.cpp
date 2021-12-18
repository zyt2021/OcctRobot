#include "OccWidget.h"

#include <QApplication>
#include <QMouseEvent>
#include <QWheelEvent>

#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <WNT_Window.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_RubberBand.hxx>

OccWidget::OccWidget(QWidget *parent) :QWidget(parent)
{
    // 1.create the viewer
    Handle(Aspect_DisplayConnection) m_display_donnection = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) aGraphicDriver = new OpenGl_GraphicDriver(m_display_donnection);
    WId window_handle =  winId();
    Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle) window_handle);
    if (!wind->IsMapped())
    {
        wind->Map();
    }
    Handle(V3d_Viewer) aViewer = new V3d_Viewer(aGraphicDriver);
    aViewer->SetDefaultLights();
    aViewer->SetLightOn();

    // 2.create the view
    myView = aViewer->CreateView();
    myView->SetWindow(wind);
    myView->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;
    myView->ChangeRenderingParams().IsAntialiasingEnabled = Standard_False;
    myView->SetBgGradientColors(Quantity_NOC_GRAY90,Quantity_NOC_GRAY75,Aspect_GFM_VER);
    myView->MustBeResized();

    // 3.create the context
    myContext = new AIS_InteractiveContext(aViewer);
    myContext->SetDisplayMode(AIS_Shaded, Standard_True);

    myContext->HighlightStyle()->SetColor(Quantity_NOC_CYAN1);
    myContext->SelectionStyle()->SetColor(Quantity_NOC_CYAN1);
    myContext->HighlightStyle(Prs3d_TypeOfHighlight_LocalDynamic)->SetColor(Quantity_NOC_CYAN1);
    myContext->HighlightStyle(Prs3d_TypeOfHighlight_LocalSelected)->SetColor(Quantity_NOC_CYAN1);

    myContext->HighlightStyle()->SetDisplayMode(1);
    myContext->SelectionStyle()->SetDisplayMode(1);
    myContext->SelectionStyle()->SetTransparency(0.2f);
    myContext->HighlightStyle()->SetTransparency(0.2f);

    myContext->MainSelector()->SetPickClosest(Standard_False);

    // 4.create the rubberBand
    myRubberBand = new AIS_RubberBand(Quantity_NOC_SKYBLUE4,Aspect_TOL_SOLID);

    // 5.create the manipulator
    myManipulator = new AIS_Manipulator();
    myManipulator->SetPart (0, AIS_MM_Scaling, Standard_True);
    myManipulator->SetPart (1, AIS_MM_Rotation, Standard_True);
    myManipulator->SetPart (2, AIS_MM_Translation, Standard_True);
    myManipulator->EnableMode (AIS_MM_Translation);
    myManipulator->EnableMode (AIS_MM_Rotation);
    myManipulator->EnableMode (AIS_MM_Scaling);

    // 6.create the view cube
    Handle(AIS_ViewCube) aCube = new AIS_ViewCube();
    aCube->SetTransformPersistence(
                new Graphic3d_TransformPers(
                    Graphic3d_TMF_TriedronPers,
                    Aspect_TOTP_RIGHT_LOWER,
                    Graphic3d_Vec2i(85, 85)));
    aCube->SetAutoStartAnimation(Standard_True);
    aCube->SetSize(60);
    aCube->SetFontHeight(13);
    myContext->Display(aCube,Standard_False);

    // 7.set the attribute of QWidget
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setBackgroundRole( QPalette::NoRole );
    setFocusPolicy( Qt::StrongFocus );
    setMouseTracking(true);
}

OccWidget::~OccWidget()
{
    myContext.Nullify();
    myView.Nullify();
    myManipulator.Nullify();
    myRubberBand.Nullify();
}

void OccWidget::paintEvent(QPaintEvent *)
{
    myView->Redraw();
}

void OccWidget::resizeEvent(QResizeEvent *)
{
    if( !myView.IsNull() )
    {
        myView->MustBeResized();
    }
}

void OccWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton)
    {
        setCursor(Qt::ClosedHandCursor);
        myView->StartRotation(event->x(),event->y());
    }
    else if(event->button()==Qt::LeftButton)
    {
        myContext->MoveTo(event->pos().x(),event->pos().y(),myView,Standard_True);
        myRubberStartPoint = event->pos();

        if(myManipulator->IsAttached())
            myManipulator->StartTransform(event->pos().x(),event->pos().y(),myView);
    }
    else if(event->button() == Qt::MidButton)
    {
        setCursor(Qt::SizeAllCursor);
        myPanStartPoint = event->pos();
    }
}

void OccWidget::mouseReleaseEvent(QMouseEvent *event)
{
    unsetCursor();
    myContext->MoveTo(event->pos().x(),event->pos().y(),myView,Standard_True);

    if(event->button()==Qt::LeftButton)
    {
        if(myContext->IsDisplayed(myRubberBand)&&!myManipulator->IsAttached())
            drawRubberBand(myRubberStartPoint.x(),myRubberStartPoint.y(),event->x(),event->y(),false);

        if(myManipulator->IsAttached())
            myManipulator->StopTransform();

        if(event->pos()==myRubberStartPoint)
        {
            AIS_StatusOfPick t_pick_status = AIS_SOP_NothingSelected;
            if(qApp->keyboardModifiers()==Qt::ControlModifier)
            {
                t_pick_status = myContext->ShiftSelect(true);
            }
            else
            {
                t_pick_status = myContext->Select(true);
            }

            if(t_pick_status == AIS_SOP_OneSelected || t_pick_status == AIS_SOP_SeveralSelected)
            {
                emit selectShapeChanged();
            }

            emit pickPixel(event->x(),event->y());
        }
        else
        {
            myContext->Select(myRubberStartPoint.x(),myRubberStartPoint.y(),event->x(),event->y(),myView,Standard_True);
        }
    }
}

void OccWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::RightButton)
    {
        myView->Rotation(event->x(),event->y());
    }
    else if(event->buttons()&Qt::MidButton)
    {
        myView->Pan(event->pos().x()-myPanStartPoint.x()
                    ,myPanStartPoint.y()-event->pos().y());
        myPanStartPoint = event->pos();
    }
    else if(event->buttons()&Qt::LeftButton)
    {
        if(!myManipulator->IsAttached())
        {
            drawRubberBand(myRubberStartPoint.x(),myRubberStartPoint.y(),event->x(),event->y(),true);
        }
        else
        {
            myManipulator->Transform(event->pos().x(),event->pos().y(),myView);
            myView->Update();
        }
    }
    else
    {
        myContext->MoveTo(event->pos().x(),event->pos().y(),myView,Standard_True);
    }
}

void OccWidget::wheelEvent(QWheelEvent *event)
{
    myView->StartZoomAtPoint(event->pos().x(),event->pos().y());
    myView->ZoomAtPoint(0, 0, event->angleDelta().y(), 0);
}

QPaintEngine *OccWidget::paintEngine() const
{
    return nullptr;
}

void OccWidget::drawRubberBand(Standard_Integer theMinX,
                               Standard_Integer theMinY,
                               Standard_Integer theMaxX,
                               Standard_Integer theMaxY,
                               Standard_Boolean theToDraw,
                               Aspect_TypeOfLine theLineType)
{
    if (!theToDraw)
    {
        myContext->Remove (myRubberBand, false);
        myContext->CurrentViewer()->RedrawImmediate();
        return;
    }

    myRubberBand->SetLineType (theLineType);
    myRubberBand->SetRectangle (theMinX, height() - theMinY, theMaxX, height() - theMaxY);
    if (!myContext->IsDisplayed (myRubberBand))
    {
        myContext->Display (myRubberBand, false);
    }
    else
    {
        myContext->Redisplay (myRubberBand, false);
    }
    myContext->CurrentViewer()->RedrawImmediate();
}
