
#ifndef __mixr_map_rpf_factory_HPP__
#define __mixr_map_rpf_factory_HPP__

#include <string>

namespace mixr {
namespace base { class Object; }
namespace rpf {
base::Object* factory(const std::string&);
}
}

#endif
