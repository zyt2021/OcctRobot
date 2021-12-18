#include "CafShapePrs.h"

IMPLEMENT_STANDARD_RTTIEXT(CafShapePrs, XCAFPrs_AISObject)

// =======================================================================
// function : CafShapePrs
// purpose  :
// =======================================================================
CafShapePrs::CafShapePrs(const TDF_Label&                theLabel,
                         const XCAFPrs_Style&            theStyle,
                         const Graphic3d_MaterialAspect& theMaterial)
: XCAFPrs_AISObject (theLabel),
  myDefStyle(theStyle)
{
  SetMaterial(theMaterial);
}
