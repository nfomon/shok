// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Regexp_h_
#define _Regexp_h_

/* Regexp rule */

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "OData.h"
#include "Rule.h"
#include "State.h"

#include "util/Graphviz.h"

#include <boost/regex.hpp>

#include <memory>

namespace fw {

struct RegexpData : public OData {
  RegexpData()
    : OData() {}
  virtual ~RegexpData() {}
  virtual operator std::string() const { return "RE<" + Util::safeLabelStr(matched) + ">"; }

  void Update(const std::string& s) {
    matched = s;
  }
  std::string matched;
};

class RegexpRule : public Rule {
public:
  RegexpRule(Log& log, const std::string& name, const boost::regex& regex)
    : Rule(log, name),
      m_regex(regex) {
    if (m_regex.empty()) {
      throw FWError("Cannot create Regexp with empty regex");
    }
  }
  virtual ~RegexpRule() {}

  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const {
    Update(connector, x);
  }

  virtual void Update(Connector& connector, FWTree& x) const {
    m_log.info("Regexp: updating " + std::string(*this) + " at " + std::string(x));
    State& state = x.GetState();
    state.Clear();
    std::string str;
    const IList* i = x.iconnection.istart;
    for (; i != NULL; i = i->right) {
      CharData idata = i->GetData<CharData>();
      str += idata.c;
      if (!boost::regex_match(str, m_regex)) {
        if (str.size() > 1) {
          if (boost::regex_match(str.begin(), str.end()-1, m_regex)) {
            state.GoComplete();
            std::string sstr = str.substr(0, str.length()-1);
            x.GetOConnection<OConnectionSingle>().ONodeUpdate<RegexpData>(sstr);
            break;
          }
        }
        state.GoBad();
        break;
      }
      connector.Listen(x, *i);
    }
    if (NULL == i) {
      boost::match_results<std::string::const_iterator> match_result;
      if (boost::regex_match(str, match_result, m_regex, boost::match_default | boost::match_partial)
          && match_result[0].matched) {
        state.GoDone();
        x.GetOConnection<OConnectionSingle>().ONodeUpdate<RegexpData>(str);
      }
    }
    x.iconnection.iend = i;
    x.iconnection.size = str.size();
    m_log.debug("Regexp " + std::string(*this) + " now: " + std::string(x));
  }

  virtual std::auto_ptr<OData> MakeData(const FWTree& x) const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;

private:
  const boost::regex m_regex;
};

std::auto_ptr<OData> RegexpRule::MakeData(const FWTree& x) const {
  return std::auto_ptr<OData>(new RegexpData());
}

std::auto_ptr<OConnection> RegexpRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSingle(m_log, x));
}

}

#endif // _Regexp_h_
