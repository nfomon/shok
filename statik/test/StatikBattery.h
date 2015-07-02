// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_StatikBattery_h_
#define _statik_test_StatikBattery_h_

#include "Battery.h"

#include "Keyword.h"
#include "Or.h"
#include "Star.h"

namespace statik_test {

class StatikBattery : public Battery {
public:
  StatikBattery(const std::string& name = "statik",
                std::istream& input = std::cin,
                std::ostream& output = std::cout) {
    m_tests.push_back(new Keyword());
    m_tests.push_back(new Or());
    m_tests.push_back(new Star());
  }
};

}

#endif // _statik_test_StatikBattery_h_
