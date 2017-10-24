//==============================================================================
// UsbJoystick -- Linux implementation
//==============================================================================
#include <errno.h>

#include <iostream>
#include <cstdio>

#include <malloc.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/joystick.h>

#include "UsbJoystick_linux.hpp"
#include "mixr/base/util/str_utils.hpp"
#include "mixr/base/util/system_utils.hpp"

namespace mixr {
namespace iodevice {

IMPLEMENT_SUBCLASS(UsbJoystick, "UsbJoystick")
EMPTY_SLOTTABLE(UsbJoystick)
EMPTY_DELETEDATA(UsbJoystick)

UsbJoystick::UsbJoystick()
{
   STANDARD_CONSTRUCTOR()
}

void UsbJoystick::copyData(const UsbJoystick& org, const bool)
{
   BaseClass::copyData(org);
}

//------------------------------------------------------------------------------
// reset() -- reset (open) device
//------------------------------------------------------------------------------
void UsbJoystick::reset()
{
   BaseClass::reset();

   if (deviceName.empty()) {

      // create the device name based on the device index
      {
         char cbuff[128]{};
         // search for device at "/dev/jsX" first
         std::sprintf(cbuff, "/dev/js%d", getDeviceIndex());
         if (base::doesFileExist(cbuff)) {
            deviceName = cbuff;
         } else {
            // if not found, search for device at "/dev/input/jsX"
            std::sprintf(cbuff, "/dev/input/js%d", getDeviceIndex());
            if (base::doesFileExist(cbuff)) {
               deviceName = cbuff;
            }
         }
      }

      // if no device found, print message and leave
      if (deviceName.empty()) {
         stream = -1;
         if (isMessageEnabled(MSG_ERROR)) {
            std::cerr << "UsbJoystick::reset(): Valid joystick device not found" << std::endl;
         }
         return;
      }

      // try opening device
      stream = open(deviceName.c_str(), O_RDONLY | O_NONBLOCK );
      if ((stream == -1) && isMessageEnabled(MSG_ERROR)) {
         std::cerr << "UsbJoystick::reset(): Error opening device at : " << deviceName << std::endl;
      }

      // determine characteristics and print out info message
      if (stream != -1) {

         int driverVersion {};
         ioctl(stream, JSIOCGVERSION, &driverVersion);

         char modelName[128] {};
         ioctl(stream, JSIOCGNAME(sizeof(modelName)), modelName);

         {
            unsigned char numOfAxes {};
            ioctl(stream, JSIOCGAXES, &numOfAxes);
            unsigned short tmp {static_cast<unsigned short>(numOfAxes)};
            if (tmp > MAX_AI)
                tmp = MAX_AI;
            numAI = tmp;
         }

         {
            unsigned char numOfBtns {};
            ioctl(stream, JSIOCGBUTTONS, &numOfBtns);
            unsigned short tmp {static_cast<unsigned short>(numOfBtns)};
            if (tmp > MAX_DI)
                tmp = MAX_DI;
            numDI = tmp;
         }

         const int major_version{driverVersion >> 16};
         const int minor_version{(driverVersion >> 8) & 0xff};
         const int bug_fix{driverVersion & 0xff};

         std::cout << std::endl;
         std::cout << "---------------------------" << std::endl;
         std::cout << "USB Joystick Configuration:" << std::endl;
         std::cout << "---------------------------" << std::endl;
         std::cout << "  Port          : " << deviceName << std::endl;
         std::cout << "  Name          : " << modelName  << std::endl;
         std::cout << "  NumAIs        : " << numAI      << std::endl;
         std::cout << "  NumDIs        : " << numDI      << std::endl;
         std::cout << "  Driver version: " << major_version << "." << minor_version << "." << bug_fix;
         std::cout  << std::endl << std::endl;
      }
   }
}

//------------------------------------------------------------------------------
// Go get our AIs and DIs here
//------------------------------------------------------------------------------
void UsbJoystick::processInputs(const double dt, base::IoData* const pInData)
{
   js_event js; // joystick event structure

   // Loop reading joystick events ..
   while (true) {

      // read the next joystick event (if any)
      int status {static_cast<int>(read(stream, &js, sizeof(js)))};

      // Break out of the loop when there are no more events
      if (status != sizeof(js)) break;

      // decode the event
      switch ( js.type & ~JS_EVENT_INIT ) {

         // button event
         case JS_EVENT_BUTTON:
            {
            int n {js.number};
            if (n < numDI)
                inBits[n] = static_cast<bool>(js.value);
            }
         break;

         // axis event
         case JS_EVENT_AXIS:
            {
            int n {js.number};
            if (n < numAI)
                inData[n] = (js.value / 32767.0);
            }
            break;

      }
   }

   // Update our base class, which will call our component DI handlers
   BaseClass::processInputs(dt, pInData);
}

}
}

