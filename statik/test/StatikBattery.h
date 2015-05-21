// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StatikBattery_h_
#define _StatikBattery_h_

#include "Battery.h"

#include "Keyword.h"

namespace statik_test {

class StatikBattery : public Battery {
public:
  StatikBattery(const std::string& name = "statik",
                std::istream& input = std::cin,
                std::ostream& output = std::cout) {
    m_tests.push_back(new Keyword());
  }
};

}

#endif // _StatikBattery_h_
