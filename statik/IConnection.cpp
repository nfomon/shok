// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IConnection.h"

#include "SError.h"

using namespace statik;

IConnection::IConnection()
  : m_istart(NULL),
    m_iend(NULL) {
}

void IConnection::Clear() {
  m_istart = NULL;
  m_iend = NULL;
}

bool IConnection::IsClear() const {
  return !m_istart;
}

const List& IConnection::Start() const {
  if (!m_istart) {
    throw SError("Cannot get start of uninitialized IConnection");
  }
  return *m_istart;
}

const List& IConnection::End() const {
  if (!m_iend) {
    throw SError("Cannot get end of uninitialized IConnection");
  }
  return *m_iend;
}

void IConnection::Restart(const List& istart, bool total) {
  g_log.debug() << "Restarting node to " << istart;
  m_istart = &istart;
  if (total) {
    m_iend = &istart;
  }
}

void IConnection::SetEnd(const List& iend) {
  if (!m_istart) {
    throw SError("Cannot set end of IConnection that has not been started");
  }
  m_iend = &iend;
}
