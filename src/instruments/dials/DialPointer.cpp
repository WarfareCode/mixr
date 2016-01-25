#include "openeaagles/instruments/dials/DialPointer.h"
#include "openeaagles/basic/units/Angles.h"

namespace oe {
namespace instruments {

IMPLEMENT_EMPTY_SLOTTABLE_SUBCLASS(DialPointer, "DialPointer")
EMPTY_SERIALIZER(DialPointer)

//------------------------------------------------------------------------------
// Constructor(s)
//------------------------------------------------------------------------------
DialPointer::DialPointer()
{
    STANDARD_CONSTRUCTOR()
}

//------------------------------------------------------------------------------
// copyData() -- copy member data
//------------------------------------------------------------------------------
EMPTY_COPYDATA(DialPointer)

//------------------------------------------------------------------------------
// deleteData() -- delete member data
//------------------------------------------------------------------------------
EMPTY_DELETEDATA(DialPointer)

//------------------------------------------------------------------------------
// draw() -- draws the object(s)
//------------------------------------------------------------------------------
void DialPointer::draw()
{
    LCreal pointerPos = getInstValue();
    LCreal startAngle = getStartAngle();
    lcSaveMatrix();
        lcRotate((-pointerPos - startAngle) * static_cast<LCreal>(basic::Angle::D2RCC));
        BasicGL::Graphic::draw();
    lcRestoreMatrix();
}

}  // end of Instruments namespace
}  // end of oe namespace
