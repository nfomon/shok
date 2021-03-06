// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Grapher_h_
#define _statik_Grapher_h_

#include "IncParser.h"
#include "List.h"
#include "Rule.h"
#include "STree.h"

#include <string>

namespace statik {

class Grapher {
public:
  Grapher(const std::string& out_dir, const std::string& base_filename);

  void AddMachine(const std::string& context, const Rule& machineRoot);
  void AddOrderList(const std::string& context, const OrderList& orderList, const List& start);
  void AddIList(const std::string& context, const List& start, const std::string& label = "");
  void AddSTree(const std::string& context, const STree& root, const std::string& label = "", const STree* initiator = NULL);
  void AddIListeners(const std::string& context, const IncParser& incParser, const List& start);
  void AddOBatch(const std::string& context, const Batch& batch, const std::string& label = "");
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

#endif // _statik_Grapher_h_
