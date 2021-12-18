#include "ModelImport.h"

#include <STEPCAFControl_Reader.hxx>
#include <IGESCAFControl_Reader.hxx>
#include <StlAPI_Reader.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFApp_Application.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <XSControl_TransferReader.hxx>
#include <Transfer_TransientProcess.hxx>
#include <Prs3d_LineAspect.hxx>

#include <QDebug>
#include <QFileInfo>

#include "XCAFBroswer.h"

ModelImport::ModelImport(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<QList<opencascade::handle<AIS_ConnectedInteractive> >>("QList<opencascade::handle<AIS_ConnectedInteractive> >");
    qRegisterMetaType<opencascade::handle<AIS_Shape>>("opencascade::handle<AIS_Shape>");
}

void ModelImport::ReadAModelFile(const QString &aModelFileName)
{
    QList<Handle(AIS_ConnectedInteractive)> list;
    Handle(AIS_Shape) anAIS;
    bool ret = false;
    QFileInfo info(aModelFileName);

    if(info.suffix()=="step"||info.suffix()=="stp"||info.suffix()=="STEP"||info.suffix()=="STP")
    {
        ret = importSTEP(aModelFileName,list);
    }
    else if(info.suffix()=="iges"||info.suffix()=="igs"||info.suffix()=="IGES"||info.suffix()=="IGS")
    {
        ret = importIGES(aModelFileName,list);
    }
    else if(info.suffix()=="stl"||info.suffix()=="STL")
    {
        ret = importSTL(aModelFileName,anAIS);
    }
    else if(info.suffix()=="brep"||info.suffix()=="brp")
    {
        ret = importBREP(aModelFileName,anAIS);
        if(ret) {
            anAIS->Attributes()->SetFaceBoundaryDraw(true);
            anAIS->Attributes()->SetFaceBoundaryAspect(
                        new Prs3d_LineAspect(Quantity_NOC_BLACK, Aspect_TOL_SOLID, 1.));
            anAIS->Attributes()->SetIsoOnTriangulation(true);
        }
    }
    else {
        qCritical()<< tr("The format of shape file is unsupported!");
        return;
    }

    if(!ret) {
        qCritical()<< tr("File %1 import failed!").arg(info.suffix());
    }
    else {
        if(!list.isEmpty()) {
            emit importFinish(list);
        }

        if(!anAIS.IsNull()) {
            emit importFinish(anAIS);
        }
        qInfo()<< tr("Import finish.");
    }
}

Standard_Boolean ModelImport::importSTEP(const QString &theFilename,
                                         QList<Handle(AIS_ConnectedInteractive) > &result)
{
    // create a new document
    TCollection_AsciiString theAscii(theFilename.toUtf8().data());
    Handle(TDocStd_Document) aDoc;
    Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
    anApp->NewDocument("MDTV-XCAF", aDoc);

    STEPCAFControl_Reader aReader;
    Handle(XSControl_WorkSession) aSession = aReader.Reader().WS();

    try {
        qInfo()<< QObject::tr("Reading step file...");
        if (!aReader.ReadFile (theAscii.ToCString()))
        {
            clearSession (aSession);
            return Standard_False;
        }

        qInfo()<< QObject::tr("Transfering step file...");
        if (!aReader.Transfer (aDoc))
        {
            clearSession (aSession);
            return Standard_False;
        }

        clearSession(aSession);
    } catch (Standard_Failure theFailure) {
        qCritical()<< QObject::tr("Exception raised during STEP import\n[")
                      + QString(theFailure.GetMessageString()) + "]\n"
                      + theAscii.ToCString();
        return Standard_False;
    }

    return getPresentations(aDoc,result);
}

Standard_Boolean ModelImport::importIGES(const QString &theFilename,
                                         QList<Handle(AIS_ConnectedInteractive)> &result)
{
    // create a new document
    TCollection_AsciiString theAscii(theFilename.toUtf8().data());
    Handle(TDocStd_Document) aDoc;
    Handle(XCAFApp_Application) anApp = XCAFApp_Application::GetApplication();
    anApp->NewDocument("MDTV-XCAF", aDoc);

    IGESCAFControl_Reader aReader;
    Handle(XSControl_WorkSession) aSession = aReader.WS();

    try {
        qInfo()<< QObject::tr("Reading iges file...");
        if (!aReader.ReadFile (theAscii.ToCString()))
        {
            clearSession (aSession);
            return Standard_False;
        }

        qInfo()<< QObject::tr("Transfering iges file...");
        if (!aReader.Transfer (aDoc))
        {
            clearSession (aSession);
            return Standard_False;
        }

        clearSession(aSession);
    } catch (Standard_Failure theFailure) {
        qCritical()<< QObject::tr("Exception raised during IGES import\n[")
                      + QString(theFailure.GetMessageString()) + "]\n"
                      + theAscii.ToCString();
        return Standard_False;
    }

    return  getPresentations(aDoc,result);
}

Standard_Boolean ModelImport::importBREP(const QString &theFilename, Handle(AIS_Shape) &result)
{
    TopoDS_Shape aShape;
    BRep_Builder aBuilder;
    TCollection_AsciiString theAscii(theFilename.toUtf8().data());
    try {
        qInfo()<< QObject::tr("Reading brep file...");
        bool ret = BRepTools::Read(aShape,theAscii.ToCString(),aBuilder);
        if(!ret) {
            return Standard_False;
        }
    } catch (Standard_Failure theFailure) {
        qCritical()<< QObject::tr("Exception raised during BREP import\n[")
                      + QString(theFailure.GetMessageString()) + "]\n"
                      + theAscii.ToCString();
    }

    if(!result) {
        result = new AIS_Shape(aShape);
    }
    else {
        result->SetShape(aShape);
    }

    return Standard_True;
}

Standard_Boolean ModelImport::importSTL(const QString &theFilename, Handle(AIS_Shape) &result)
{
    TCollection_AsciiString theAscii(theFilename.toUtf8().data());
    StlAPI_Reader aReader;
    TopoDS_Shape theShape;
    aReader.Read(theShape, theAscii.ToCString());
    if(result) {
        result->SetShape(theShape);
    }
    else
        result = new AIS_Shape(theShape);
    return Standard_True;
}

Standard_Boolean ModelImport::getPresentations(const Handle(TDocStd_Document) &theDoc, QList<Handle(AIS_ConnectedInteractive)> &result)
{
    Handle(XCAFDoc_ShapeTool) aShapeTool = XCAFDoc_DocumentTool::ShapeTool (theDoc->Main());
    Handle(XCAFDoc_ColorTool) aColorTool = XCAFDoc_DocumentTool::ColorTool (theDoc->Main());

    TDF_LabelSequence aLabels;
    aShapeTool->GetFreeShapes (aLabels);

    // create presentations
    MapOfPrsForShapes aMapOfShapes;
    XCAFPrs_Style aDefStyle;
    aDefStyle.SetColorSurf (Quantity_NOC_GRAY65);
    aDefStyle.SetColorCurv (Quantity_NOC_GRAY65);
    for (Standard_Integer aLabIter = 1; aLabIter <= aLabels.Length(); ++aLabIter)
    {
        const TDF_Label& aLabel = aLabels.Value (aLabIter);
        displayWithChildren (*aShapeTool, *aColorTool, aLabel, TopLoc_Location(), aDefStyle, "", aMapOfShapes, result);
    }

    return Standard_True;
}

void ModelImport::displayWithChildren(XCAFDoc_ShapeTool &theShapeTool,
                                      XCAFDoc_ColorTool &theColorTool,
                                      const TDF_Label &theLabel,
                                      const TopLoc_Location &theParentTrsf,
                                      const XCAFPrs_Style &theParentStyle,
                                      const TCollection_AsciiString &theParentId,
                                      MapOfPrsForShapes &theMapOfShapes,
                                      QList<Handle(AIS_ConnectedInteractive)> &result)
{
    // 1. if the label is referenced by other label, get the reference one
    TDF_Label aRefLabel = theLabel;
    if (theShapeTool.IsReference (theLabel))
    {
        theShapeTool.GetReferredShape (theLabel, aRefLabel);
    }

    // 2. get the id of the label
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry (theLabel, anEntry);
    if (!theParentId.IsEmpty())
    {
        anEntry = theParentId + "\n" + anEntry;
    }
    anEntry += ".";

    // 3. if the label is not an assembly, connect the shape and the transform, end the current iteration
    if (!theShapeTool.IsAssembly (aRefLabel))
    {
        Handle(AIS_InteractiveObject) anAis;
        if (!theMapOfShapes.Find (aRefLabel, anAis))
        {
            anAis = new CafShapePrs (aRefLabel, theParentStyle, Graphic3d_NameOfMaterial_ShinyPlastified);
            theMapOfShapes.Bind (aRefLabel, anAis);
        }

        Handle(TCollection_HAsciiString) anId      = new TCollection_HAsciiString (anEntry);
        Handle(AIS_ConnectedInteractive) aConnected = new AIS_ConnectedInteractive();
        aConnected->Connect (anAis, theParentTrsf.Transformation());
        aConnected->SetOwner (anId);
        aConnected->SetLocalTransformation (theParentTrsf.Transformation());

        result.append(aConnected);
        return;
    }

    // 4. if the label is an assembly, iterate it's children and apply default presentation
    XCAFPrs_Style aDefStyle = theParentStyle;
    Quantity_Color aColor;
    if (theColorTool.GetColor (aRefLabel, XCAFDoc_ColorGen, aColor))
    {
        aDefStyle.SetColorCurv (aColor);
        aDefStyle.SetColorSurf (aColor);
    }
    if (theColorTool.GetColor (aRefLabel, XCAFDoc_ColorSurf, aColor))
    {
        aDefStyle.SetColorSurf (aColor);
    }
    if (theColorTool.GetColor (aRefLabel, XCAFDoc_ColorCurv, aColor))
    {
        aDefStyle.SetColorCurv (aColor);
    }

    for (TDF_ChildIterator childIter (aRefLabel); childIter.More(); childIter.Next())
    {
        TDF_Label aLabel = childIter.Value();
        if (!aLabel.IsNull()
                && (aLabel.HasAttribute() || aLabel.HasChild()))
        {
            TopLoc_Location aTrsf = theParentTrsf * theShapeTool.GetLocation (aLabel);
            displayWithChildren (theShapeTool, theColorTool, aLabel, aTrsf, aDefStyle, anEntry, theMapOfShapes,result);
        }
    }
}

void ModelImport::clearSession (const Handle(XSControl_WorkSession)& theSession)
{
    if (theSession.IsNull())
    {
        return;
    }

    Handle(Transfer_TransientProcess) aMapReader = theSession->TransferReader()->TransientProcess();
    if (!aMapReader.IsNull())
    {
        aMapReader->Clear();
    }

    Handle(XSControl_TransferReader) aTransferReader = theSession->TransferReader();
    if (!aTransferReader.IsNull())
    {
        aTransferReader->Clear(1);
    }
}
