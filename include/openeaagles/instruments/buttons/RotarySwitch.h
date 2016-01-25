//------------------------------------------------------------------------------
// Class:       RotarySwitch
// Base class:  basic::Object -> basic::Component -> BasicGL::Graphic -> Button -> RotarySwitch
// Description: Generic multi-position selector switch that knows when it is left or right
//                mouse clicked and moves to next or previous switch position.
//------------------------------------------------------------------------------
#ifndef __oe_instruments_RotarySwitch_H__
#define __oe_instruments_RotarySwitch_H__

#include "openeaagles/instruments/buttons/Button.h"

namespace oe {
namespace instruments {

class RotarySwitch : public Button
{
    DECLARE_SUBCLASS(RotarySwitch,Button)

public:
   RotarySwitch();

   // Get current switch position
   int getCurrentPosition() const { return currentPosition; };

   bool onSingleClick() override;

   void updateData(const LCreal dt = 0.0) override;

protected:
    bool setSlotAngles(const basic::PairStream* const stream);
    bool setSlotStartPosition(const basic::Number* const x);

private:
    static const int MAX_ANGLES = 500;

    int currentPosition;    //current switch position
    LCreal angles[MAX_ANGLES];
    int numAngs;
    int startPosition;
    SendData angleSD;       // angle to send to our rotator
};

}  // end instruments namespace
}  // end oe namespace

#endif
