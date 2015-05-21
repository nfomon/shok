// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Battery_h_
#define _Battery_h_

#include "Test.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <iostream>
#include <istream>
#include <ostream>
#include <vector>

namespace statik_test {

class Battery {
public:
  Battery(const std::string& name = "",
          std::istream& input = std::cin,
          std::ostream& output = std::cout);
  void Run();

protected:
  std::string m_name;
  std::istream& m_input;
  std::ostream& m_output;

  typedef boost::ptr_vector<Test> test_vec;
  typedef test_vec::const_iterator test_iter;
  typedef test_vec::iterator test_mod_iter;
  test_vec m_tests;
};

}

#endif // _Battery_h_
