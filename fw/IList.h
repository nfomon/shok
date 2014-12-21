// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IList_h_
#define _IList_h_

#include "FWError.h"
#include "OData.h"

#include <memory>
#include <string>

namespace fw {

struct IList {
  IList(std::auto_ptr<OData> data, IList* left = NULL, IList* right = NULL);
  IList(const FWTree* owner, IList* left = NULL, IList* right = NULL);
  const FWTree* owner;

private:
  std::auto_ptr<OData> m_data;

public:
  OData& GetData() const { return *m_data.get(); }
  template <typename DataType>
  DataType& GetData() const {
    DataType* data = dynamic_cast<DataType*>(m_data.get());
    if (!data) {
      throw FWError("Cannot retrieve data to incorrect type");
    }
    return *data;
  }

  operator std::string() const { return "(IList " + (m_data.get() ? std::string(GetData()) : "<null>") + ")"; }
  std::string print() const {
    std::string s = "<" + std::string(GetData()) + ">";
    if (right) {
      s += "-" + right->print();
    }
    return s;
  }

  std::string DrawNode(const std::string& context) const;

  IList* left;
  IList* right;
};

}

#endif // _IList_h_
