
#include "mixr/iodevice/Ai2DiSwitch.hpp"

#include "mixr/base/io/IoData.hpp"
#include "mixr/base/io/IoDevice.hpp"
#include "mixr/base/io/IoHandler.hpp"

#include "mixr/base/numeric/Number.hpp"

#include <iostream>

namespace mixr {
namespace iodevice {

IMPLEMENT_SUBCLASS(Ai2DiSwitch, "Ai2DiSwitch")
EMPTY_DELETEDATA(Ai2DiSwitch)

BEGIN_SLOTTABLE(Ai2DiSwitch)
    "di",         // 1) Discrete Input location (IoData's DI channel)
    "channel",    // 2) Device's AI channel number
    "level",      // 3) Level to switch DI (default: 0)
    "inverted"    // 4) Inverted bit flag (default: false)
END_SLOTTABLE(Ai2DiSwitch)

BEGIN_SLOT_MAP(Ai2DiSwitch)
    ON_SLOT( 1, setSlotLocation, base::Number)
    ON_SLOT( 2, setSlotChannel,  base::Number)
    ON_SLOT( 3, setSlotLevel,    base::Number)
    ON_SLOT( 4, setSlotInverted, base::Number)
END_SLOT_MAP()

Ai2DiSwitch::Ai2DiSwitch()
{
   STANDARD_CONSTRUCTOR()
}

void Ai2DiSwitch::copyData(const Ai2DiSwitch& org, const bool)
{
   BaseClass::copyData(org);

   devEnb = org.devEnb;
   location = org.location;
   channel = org.channel;
   level = org.level;
   invert = org.invert;
}


//------------------------------------------------------------------------------
// Get functions
//------------------------------------------------------------------------------
unsigned int Ai2DiSwitch::getLocation() const
{
   return location;
}

unsigned int Ai2DiSwitch::getChannel() const
{
   return channel;
}

double Ai2DiSwitch::getLevel() const
{
   return level;
}

bool Ai2DiSwitch::getInvertFlag() const
{
   return invert;
}

//------------------------------------------------------------------------------
// Set functions
//------------------------------------------------------------------------------
bool Ai2DiSwitch::setLocation(const unsigned int v)
{
   location = v;
   return true;
}

bool Ai2DiSwitch::setChannel(const unsigned int v)
{
   channel = v;
   devEnb = true;
   return true;
}

bool Ai2DiSwitch::setLevel(const double v)
{
   level = v;
   return true;
}

bool Ai2DiSwitch::setInvertFlag(const bool f)
{
   invert = f;
   return true;
}

//------------------------------------------------------------------------------
// process inputs
//------------------------------------------------------------------------------
void Ai2DiSwitch::processInputs(const double, const base::IoDevice* const device, base::IoData* const inData)
{
   // Default is our initial value
   double vin(0.0);

   // Get data from the AI device
   if (device != nullptr && devEnb) {
      device->getAnalogInput(&vin, channel);
   }

   // Set the bit to the cockpit input handler
   if (inData != nullptr) {
      bool flag = (vin >= level);
      if (invert) flag = !flag;
      inData->setDiscreteInput(location,flag);
   }
}

//------------------------------------------------------------------------------
// process outputs
//------------------------------------------------------------------------------
void Ai2DiSwitch::processOutputs(const double, const base::IoData* const, base::IoDevice* const)
{
}

//------------------------------------------------------------------------------
// Slot Functions
//------------------------------------------------------------------------------

// location: Input array index (location)
bool Ai2DiSwitch::setSlotLocation(const base::Number* const msg)
{
   bool ok = false;
   if (msg != nullptr) {
      int v = msg->getInt();
      if (v >= 0) {
         ok = setLocation( static_cast<unsigned int>(v) );
      }
   }
   return ok;
}

// channel: Device's AI channel number
bool Ai2DiSwitch::setSlotChannel(const base::Number* const msg)
{
   bool ok = false;
   if (msg != nullptr) {
      int v = msg->getInt();
      if (v >= 0) {
         ok = setChannel( static_cast<unsigned int>(v) );
      }
   }
   return ok;
}

// level: Level to switch DI (default: 0)
bool Ai2DiSwitch::setSlotLevel(const base::Number* const msg)
{
   bool ok = false;
   if (msg != nullptr) {
      ok = setLevel( msg->getReal() );
   }
   return ok;
}

// invert: Inverted bit flag (default: false)
bool Ai2DiSwitch::setSlotInverted(const base::Number* const msg)
{
   bool ok = false;
   if (msg != nullptr) {
      ok = setInvertFlag( msg->getBoolean() );
   }
   return ok;
}

}
}
