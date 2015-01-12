// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Grapher_h_
#define _Grapher_h_

#include "Connector.h"
#include "FWTree.h"
#include "IList.h"
#include "Rule.h"

#include <string>

namespace fw {

class Grapher {
public:
  Grapher(const std::string& out_dir, const std::string& base_filename);

  void AddMachine(const std::string& context, const Rule& machineRoot);
  void AddIList(const std::string& context, const IList& start, const std::string& label = "");
  void AddOTree(const std::string& context, const FWTree& root, const std::string& label = "");
  void AddIListeners(const std::string& context, const Connector& connector, const IList& start);
  void Signal(const std::string& context, const void*, bool isUpdate = false);

  bool IsDirty() const { return m_isDirty; }

  void SaveAndClear() {
    Save();
    Clear();
  }

private:
  void Save();
  void Clear();

  std::string m_out_dir;
  std::string m_base_filename;
  std::string m_graph;
  int m_img_count;
  bool m_isDirty;
};

}

#endif // _Grapher_h_
