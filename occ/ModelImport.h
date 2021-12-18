#ifndef MODELIMPORT_H
#define MODELIMPORT_H

#include <QString>
#include <QObject>

#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XSControl_WorkSession.hxx>
#include <TDocStd_Document.hxx>
#include <AIS_ConnectedInteractive.hxx>

#include "CafShapePrs.h"

class Poly_Triangulation;
class RobotCreator;

class ModelImport : public QObject
{
    friend class RobotCreator;
    Q_OBJECT

public:
    explicit ModelImport(QObject *parent = nullptr);

public slots:
    void ReadAModelFile(const QString &aModelFileName);

private:
    static Standard_Boolean importSTEP(const QString &theFilename,
                                       QList<Handle(AIS_ConnectedInteractive)> &result);
    static Standard_Boolean importIGES(const QString &theFilename,
                                       QList<Handle(AIS_ConnectedInteractive)> &result);
    static Standard_Boolean importBREP(const QString &theFilename, Handle(AIS_Shape) &result);
    static Standard_Boolean importSTL(const QString &theFilename, Handle(AIS_Shape) &result);

    static Standard_Boolean getPresentations(const Handle(TDocStd_Document) &theDoc, QList<Handle(AIS_ConnectedInteractive)> &result);
    static void displayWithChildren (XCAFDoc_ShapeTool&                       theShapeTool,
                                     XCAFDoc_ColorTool&                       theColorTool,
                                     const TDF_Label&                         theLabel,
                                     const TopLoc_Location&                   theParentTrsf,
                                     const XCAFPrs_Style&                     theParentStyle,
                                     const TCollection_AsciiString&           theParentId,
                                     MapOfPrsForShapes&                       theMapOfShapes,
                                     QList<Handle(AIS_ConnectedInteractive)>& result);
    static void clearSession (const Handle(XSControl_WorkSession)& theSession);

signals:
    void importFinish(const QList<Handle(AIS_ConnectedInteractive)>& result);
    void importFinish(const Handle(AIS_Shape)& result);
};

#endif // MODELIMPORT_H
