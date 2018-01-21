
#include "mixr/graphics/readouts/Rotary2.hpp"

#include "mixr/base/numeric/Boolean.hpp"
#include "mixr/base/numeric/Integer.hpp"

namespace mixr {
namespace graphics {

IMPLEMENT_SUBCLASS(Rotary2, "Rotary2")
EMPTY_SLOTTABLE(Rotary2)
EMPTY_COPYDATA(Rotary2)
EMPTY_DELETEDATA(Rotary2)

BEGIN_EVENT_HANDLER(Rotary2)
   ON_EVENT_OBJ(SELECT, onSelect, base::Boolean)
   ON_EVENT_OBJ(SELECT, onSelect, base::Integer)
END_EVENT_HANDLER()

Rotary2::Rotary2()
{
   STANDARD_CONSTRUCTOR()

   const auto p = new base::Integer(1);    // default rotary item
   setSelectionName(p);
   p->unref();
}

// onSelect() - Macro function for Rotary2
bool Rotary2::onSelect(const base::Boolean* const x)
{
   if (x != nullptr) {
      if (x->getBoolean()){
         //if true, select the second component
         base::Integer two(2);
         select(&two);
      } else {
         //if false, select the first component
         base::Integer one(1);
         select(&one);
      }
   }
   return true;
}

// onSelect() - Macro function for Rotary2
bool Rotary2::onSelect(const base::Integer* const x)
{
   select(x);
   return true;
}

}
}
