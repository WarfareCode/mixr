
#ifndef __mixr_graphics_Page_HPP__
#define __mixr_graphics_Page_HPP__

#include "Graphic.hpp"
#include "mixr/base/Identifier.hpp"

#include <array>

namespace mixr {
namespace base { class Boolean; class Pair; class PairStream;}
namespace graphics {

//------------------------------------------------------------------------------
// Class: Page
// Description: Page format. A list of graphic objects and methods to manage
//              them as a page, also contains subpages and a background page.
//------------------------------------------------------------------------------
// EDL Interface:
//
// Factory name: Page
// Slots:
//  page                 <Identifier>   ! initial subpage (default: 0)
//  pages                <PairStream>   ! subpages (default: 0)
//  pages                <Page>
//  pagingEvent          <PairStream>
//  subpagesFirst        <Boolean>      ! draw subpages first (default: draw our page graphics first)
//  focusSlavedToSubpage <Boolean>      ! slave the focus to the subpage (default: true)
//------------------------------------------------------------------------------
// Events:
//  ON_ENTRY      <>        ! sets focus to our subpage, if we have one
//  ON_EXIT       <>        !
//  BUTTON_HIT    <String>  ! handle button hits (with button name) as page change requests
//  ON_ANYKEY     <int>     ! handle keyboard inputs as page change requests
//------------------------------------------------------------------------------
class Page : public Graphic
{
   DECLARE_SUBCLASS(Page, Graphic)

public:
   Page()                                       { STANDARD_CONSTRUCTOR() }

   const char* subpageName() const              { return cpName.c_str(); }
   Page* subpage() const                        { return cp; }

   bool isPostDrawSubpage() const               { return postDraw1; }

   // returns a member from the slot name given, else nullptr
   virtual base::Pair* findSubpageByName(const char* const slotname);
   // returns a member of the given type, else nullptr
   virtual base::Pair* findSubpageByType(const std::type_info& type);

   // returns true if the focus is slaved to a subpage
   bool isFocusSlavedToSubpage() const          { return focusSlavedToSubpage; }
   void setFocusSlavedToSubpage(const bool f)   { focusSlavedToSubpage = f; }

   // call new sub-page
   // changes subpages by page, returns true if page was found
   bool newSubpage(Page* const newPage, Page* theCaller, base::Object* theArg = nullptr);
   // changes subpages by name, returns true if the page was found
   bool newSubpage(const char* const name, Page* theCaller, base::Object* theArg = nullptr);

   // event handlers
   virtual bool onEntry();
   virtual bool onExit();
   // handles the button hit as a page change event
   virtual bool onButtonHit(const base::String* const obhobj);
   // handles the keyboard hit as a page change event
   virtual bool onKeyHit(const int key);

   void draw() override;
   base::Pair* findBySelectName(const GLuint name) override;
   bool event(const int event, base::Object* const obj = nullptr) override;

   void updateTC(const double dt = 0.0) override;
   void updateData(const double dt = 0.0) override;
   void reset() override;

protected:
   // Return our paging arguments
   base::Object* getArgument()               { return pageArg; }
   const Page* getCaller()                   { return caller; }

   // Return our subpages
   base::PairStream* subPages()              { return subpages; }

   // Manage our (sub)page stack
   bool clearSubpageStack();
   bool pushSubpage(const char* const name, Page* theCaller, base::Object* theArg = nullptr);
   bool popSubpage(Page* theCaller, base::Object* theArg = nullptr);

   // Call/push/pop major pages (our container's pages, which we are a member of)
   bool newPage(Page* const newPage, Page* theCaller, base::Object* theArg = nullptr);
   bool newPage(const char* const name, Page* theCaller, base::Object* theArg = nullptr);
   bool pushPage(const char* const name, Page* theCaller, base::Object* theArg = nullptr);
   bool popPage(Page* theCaller, base::Object* theArg = nullptr);

private:
   bool processSubpages();

   Page* cp {};                         // current subpage
   base::Identifier cpName;             // current subpage name
   Page* np {};                         // new subpage (requesting a page change)

   base::PairStream* subpages {};       // subpages
   base::PairStream* pageChgEvents {};  // page change events

   bool postDraw1 {};                   // post draw component (child) graphic
   bool focusSlavedToSubpage {true};    // input event focus should follow subpage changes

   // Passed by calling page
   base::safe_ptr<base::Object> pageArg;     // paging argument
   const Page* caller {};                    // calling page

   // Subpage Stack
   static const int SUBPAGE_STACK_SIZE {50};
   std::array<Page*, SUBPAGE_STACK_SIZE> subpageStack {};
   int subpageSP {SUBPAGE_STACK_SIZE};       // stack pointer

private:
   // slot table helper methods
   bool setSlotPage(const base::Identifier* const);
   bool setSlotSubpageStream(base::PairStream* const);
   bool setSlotSubpageSingle(Page* const);
   bool setSlotPagingEvent(base::PairStream* const);
   bool setSlotDrawSubpageFirst(const base::Boolean* const);
   bool setSlotFocusSlavedToSubpage(const base::Boolean* const);
};

}
}

#endif
