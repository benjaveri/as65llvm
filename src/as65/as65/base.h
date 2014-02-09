#ifndef _base_h
#define _base_h

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <iomanip>

#include <boost/config/warning_disable.hpp>
#include <boost/variant.hpp>

#define TEST

struct Object {
    virtual std::string toString() const = 0;
};

//inline std::string hex(unsigned u,unsigned w) { std::stringstream s; s << std::setfill("0") << std::setw(w) << std::hex << u; return s.str(); }
inline std::string dec(unsigned u) { std::stringstream s; s << u; return s.str(); }

#endif
