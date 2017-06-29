
#ifndef __mixr_gui_glut_factory_H__
#define __mixr_gui_glut_factory_H__

#include <string>

namespace mixr {
namespace base { class Object; }
namespace glut {
base::Object* factory(const std::string&);
}
}

#endif
