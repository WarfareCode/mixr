
#include "mixr/graphics/readouts/NumericReadout.hpp"

#include "mixr/base/numeric/Boolean.hpp"
#include "mixr/base/numeric/Float.hpp"
#include "mixr/base/numeric/Number.hpp"
#include "mixr/base/numeric/Integer.hpp"

#include "mixr/base/String.hpp"

#include "mixr/base/util/str_utils.hpp"
#include "mixr/base/util/constants.hpp"

#include "ReformatScanner.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mixr {
namespace graphics {

IMPLEMENT_SUBCLASS(NumericReadout, "NumericReadout")

ReformatScanner* NumericReadout::reformatter = new ReformatScanner();

BEGIN_SLOTTABLE(NumericReadout)
   "value",            //  1: Value to be displayed
   "maxValue",         //  2: Maximum value that can be displayed
   "format",           //  3: Example format text
   "plusChar",         //  4: Positive value character
   "minusChar",        //  5: Negative value character
   "dpChar",           //  6: Decimal Point character
   "undefinedChar",    //  7: Undefined value character
   "overflowChar",     //  8: Overflow character
   "maxValid",         //  9: Maximum valid input
   "minValid",         // 10: Minimum valid input
   "blankIfZero",      // 11: Display blanks if value is zero
END_SLOTTABLE(NumericReadout)

BEGIN_SLOT_MAP(NumericReadout)
   ON_SLOT(1, setSlotFloatToBeDisplayed,   base::Float)
   ON_SLOT(1, setSlotIntegerToBeDisplayed, base::Integer)
   ON_SLOT(2, setSlotFloatMaxValue,        base::Float)
   ON_SLOT(2, setSlotIntegerMaxValue,      base::Integer)
   ON_SLOT(3, setSlotExampleFormatText,    base::String)
   ON_SLOT(4, setSlotPlusChar,             base::String)
   ON_SLOT(5, setSlotMinusChar,            base::String)
   ON_SLOT(6, setSlotDecimalPointChar,     base::String)
   ON_SLOT(7, setSlotUndefinedChar,        base::String)
   ON_SLOT(8, setSlotOverflowChar,         base::String)
   ON_SLOT(9, setSlotMaxValid,             base::Number)
   ON_SLOT(10, setSlotMinValid,            base::Number)
   ON_SLOT(11, setSlotBlankZero,           base::Boolean)
END_SLOT_MAP()

BEGIN_EVENT_HANDLER(NumericReadout)
   ON_EVENT_OBJ(UPDATE_VALUE, onUpdateValue, base::Float)
   ON_EVENT_OBJ(UPDATE_VALUE, onUpdateValue, base::Integer)
   ON_EVENT_OBJ(UPDATE_VALUE, onUpdateValue, base::Number)
END_EVENT_HANDLER()

NumericReadout::NumericReadout()
{
   STANDARD_CONSTRUCTOR()

   maxNum = base::UNDEFINED_VALUE;
   base::utStrcpy(format, FORMAT_LENGTH, "%.0f");
   justification(base::Justify::Right);
}

void NumericReadout::copyData(const NumericReadout& org, const bool)
{
   BaseClass::copyData(org);

   // copy the display buffer, example format, and the sprintf format
   base::utStrcpy(cbuf,CBUF_LENGTH,org.cbuf);
   base::utStrcpy(format,FORMAT_LENGTH,org.format);

   // copy other member variables
   plusChar  = org.plusChar;
   minusChar = org.minusChar;
   dpChar    = org.dpChar;
   undefinedChar = org.undefinedChar;
   overflowChar  = org.overflowChar;
   postSign  = org.postSign;
   num = org.num;
   maxNum = org.maxNum;
   maxValid = org.maxValid;
   minValid = org.minValid;
   blankZero = org.blankZero;
}

void NumericReadout::deleteData()
{
   cbuf[0]   = '\0';
   base::utStrcpy(format,FORMAT_LENGTH,"%.0f");
   plusChar  = '\0';
   minusChar = '\0';
   dpChar    = '\0';
   undefinedChar = '-';
   overflowChar  = '*';
   postSign = false;
   num  = 0.0;
   blankZero = false;
}

void NumericReadout::updateData(const double dt)
{
   BaseClass::updateData(dt);
}

//------------------------------------------------------------------------------
// isInputValueValid() -- returns true if the input value is valid.
//------------------------------------------------------------------------------
bool NumericReadout::isInputValueValid() const
{
   bool ok {true};
   const double val {getInputValue()};
   if ( (minValid != base::UNDEFINED_VALUE && val < minValid) ||
        (maxValid != base::UNDEFINED_VALUE && val > maxValid) ) {
      ok = false;
   }
   return ok;
}

//------------------------------------------------------------------------------
// input mode function --
//   filterInputEvent() -- Filter input events using a template character (x)
//------------------------------------------------------------------------------
char NumericReadout::filterInputEvent(const int event, const char x)
{
   const char tc{static_cast<int>(x)};
   if (tc == '+') {
      // sign keys
      if (event == '7') {
         if (minusChar != '\0')
            return minusChar;
         else
            return '-';
      }
      else if (event == '9') {
         if (plusChar != '\0')
            return plusChar;
         else
            return '+';
      }
      else {
         return '\0';
      }
   }
   else {
      return BaseClass::filterInputEvent(event,tc);
   }
}

//------------------------------------------------------------------------------
// getInputValue() -- returns the readout as a numeric value
//------------------------------------------------------------------------------
double NumericReadout::getInputValue() const
{
   // copy string to buffer with correct sign character
   const std::size_t CBUFLOCAL_LEN {100};
   char cbuf[CBUFLOCAL_LEN] {};
   const char* p {this->str.c_str()};
   base::utStrcpy(cbuf, CBUFLOCAL_LEN, p);
   if (cbuf[0] == plusChar)  cbuf[0] = '+';
   if (cbuf[0] == minusChar) cbuf[0] = '-';

   // Check decimal point character
   if (dpChar != '\0') {
      int i {};
      for (i = 0; cbuf[i] != '\0'; i++) {
         if (cbuf[i] == dpChar)  cbuf[i] = '.';
      }
   }

   // Remove spaces
   {
      int i{};
      while (cbuf[i] != '\0') {
         if (cbuf[i] == ' ') {
            int j{i + 1};
            while (cbuf[j] != '\0') { cbuf[j-1] = cbuf[j]; j++; }
            cbuf[j-1] = '\0';
         }
         else i++;
      }
   }

   return std::atof(cbuf);
}

//------------------------------------------------------------------------------
// redisplay() -- redisplay the value
//------------------------------------------------------------------------------
void NumericReadout::redisplay()
{
   // Check if we are displaying blank for zero
   if ((num == 0) && blankZero) {
      for (std::size_t i = 0; i < width(); i++) {
         cbuf[i] = ' ';
      }
      cbuf[width()] = '\0';
      setText(cbuf);
      return;
   }

   // Check if we have an undefined value and should place dashes in the display
   if (num == base::UNDEFINED_VALUE) {
      for (std::size_t i = 0; i < width(); i++) {
         cbuf[i] = undefinedChar;
      }
      cbuf[width()] = '\0';
      setText(cbuf);
      return;
   }

   // Check if we have an undefined value and should place dashes in the display
   if (maxNum != base::UNDEFINED_VALUE && num > maxNum) {
      for (std::size_t i = 0; i < width(); i++) {
         cbuf[i] = overflowChar;
      }
      cbuf[width()] = '\0';
      setText(cbuf);
      return;
   }

   // Create the readout text string
   makeText();

   std::size_t len{std::strlen(cbuf)};

   // When we have a replacement char for the sign character
   if (plusChar != '\0' || minusChar != '\0' || postSign) {

      // Find the sign character (if any)
      std::size_t i {};
      while (i < len) {
         if (cbuf[i] == '+' || cbuf[i] == '-') break;
         i++;
      }

      // When we have a sign character
      if (i < len) {
         if (cbuf[i] == '+' && plusChar  != '\0')  cbuf[i] = plusChar;
         if (cbuf[i] == '-' && minusChar != '\0')  cbuf[i] = minusChar;
         if (postSign) {
            // move sign character to the end of the string
            char cc {cbuf[i]};
            while (i < (len-1)) {
               cbuf[i] = cbuf[i+1];
               i++;
            }
            cbuf[i] = cc;
         }
      }
   }

   // When we have a replacement char for the decimal point
   if (dpChar != '\0') {

      // Find the decimal point (if any)
      std::size_t i {};
      while (i < len) {
         if (cbuf[i] == '.') break;
         i++;
      }

      // when we have a decimal point
      if (i < len) cbuf[i] = dpChar;
   }

   // Have Field display this text
   setText(cbuf);
}

//------------------------------------------------------------------------------
//  makeText() -- make the text string use the current value and formats
//------------------------------------------------------------------------------
void NumericReadout::makeText()
{
   std::sprintf(cbuf, format, getFloat());
}

//------------------------------------------------------------------------------
// reformat() -- convert the numerical value into an ascii character string
//------------------------------------------------------------------------------
void NumericReadout::reformat(const char* const example)
{
   if (reformatter->convertNumber(example) != ReformatScanner::DataType::invalid) {
      setExample(example);
      base::utStrcpy(format, FORMAT_LENGTH, reformatter->getFormat());
      postSign = reformatter->isPostSign();
      redisplay();
   }
}

//------------------------------------------------------------------------------
// Event functions
//------------------------------------------------------------------------------

// onUpdateValue() - for base::Float
bool NumericReadout::onUpdateValue(const base::Float* const ouvobj)
{
   if (ouvobj != nullptr) {
      setValue(ouvobj->asDouble());
   }
   return true;
}

// onUpdateValue() - for base::Integer
bool NumericReadout::onUpdateValue(const base::Integer* const ouvobj)
{
   if (ouvobj != nullptr) {
      setValue(ouvobj->asInt());
   }
   return true;
}

// onUpdateValue() - for base::Number
bool NumericReadout::onUpdateValue(const base::Number* const ouvobj)
{
   if (ouvobj != nullptr) {
      setValue(ouvobj->asDouble());
   }
   return true;
}

bool NumericReadout::setSlotFloatToBeDisplayed(const base::Float* const x)
{
   bool ok{true};
   if (x != nullptr) {
      setValue(x->asDouble());
      ok = true;
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setFloatToBeDisplayed: \"value\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

bool NumericReadout::setSlotIntegerToBeDisplayed(const base::Integer* const x)
{
   bool ok{true};
   if (x != nullptr) {
      setValue(x->asInt());
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setNumberToBeDisplayed: \"value\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

bool NumericReadout::setSlotFloatMaxValue(const base::Float* const x)
{
   bool ok{true};
   if (x != nullptr) {
      setMaxValue(x->asDouble());
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setFloatMaxValue: \"maxValue\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

bool NumericReadout::setSlotIntegerMaxValue(const base::Integer* const x)
{
   bool ok{true};
   if (x != nullptr) {
      setMaxValue(x->asInt());
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setNumberMaxValue: \"maxValue\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

bool NumericReadout::setSlotExampleFormatText(const base::String* const seftobj)
{
   bool ok{true};
   if (seftobj != nullptr) {
      if (width() == 0) width(seftobj->len());
      reformat((*seftobj).c_str());
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setExampleFormatText: \"format\" must be a string!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotPlusChar() -- positive value character
bool NumericReadout::setSlotPlusChar(const base::String* const spcobj)
{
   bool ok{true};
   if (spcobj != nullptr) {
      const char* str{(*spcobj).c_str()};
      plusChar = str[0];
      redisplay();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setPlusChar \"plusChar\" must be a character!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotMinusChar() -- negative value character
bool NumericReadout::setSlotMinusChar(const base::String* const smcobj)
{
   bool ok{true};
   if (smcobj != nullptr) {
      const char* str{(*smcobj).c_str()};
      minusChar = str[0];
      redisplay();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setMinusChar \"minusChar\" must be a character!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotDecimalPointChar() -- decimal point character
bool NumericReadout::setSlotDecimalPointChar(const base::String* const sdpcobj)
{
   bool ok{true};
   if (sdpcobj != nullptr) {
      const char* str{(*sdpcobj).c_str()};
      dpChar = str[0];
      redisplay();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setDecimalPointChar \"dpChar\" must be a character!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotUndefinedChar() -- undefined value character
bool NumericReadout::setSlotUndefinedChar(const base::String* const sucobj)
{
   bool ok{true};
   if (sucobj != nullptr) {
      const char* str{(*sucobj).c_str()};
      undefinedChar = str[0];
      redisplay();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setUndefinedChar \"undefinedChar\" must be a character!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotMaxValid() -- Maximum valid value
bool NumericReadout::setSlotMaxValid(const base::Number* const msg)
{
   bool ok{true};
   if (msg != nullptr) {
      maxValid = msg->asDouble();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setSlotMaxValid: \"maxValid\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotMinValid() -- Minimum valid value
bool NumericReadout::setSlotMinValid(const base::Number* const msg)
{
   bool ok{true};
   if (msg != nullptr) {
      minValid = msg->asDouble();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setSlotMinValid: \"minValid\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotBlankZero() -- Display blank if value is zero
bool NumericReadout::setSlotBlankZero(const base::Boolean* const msg)
{
   bool ok{true};
   if (msg != nullptr) {
      blankZero = msg->asBool();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setSlotMinValid: \"blankZero\" must be a number!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

//  setSlotOverflowChar() --overflow character
bool NumericReadout::setSlotOverflowChar(const base::String* const socobj)
{
   bool ok{true};
   if (socobj != nullptr) {
      const char* str{(*socobj).c_str()};
      overflowChar = str[0];
      redisplay();
   } else {
      if (isMessageEnabled(MSG_ERROR)) {
         std::cerr << "NumericReadout::setOverflowChar \"overflowChar\" must be a character!" << std::endl;
      }
      ok = false;
   }
   return ok;
}

}
}
