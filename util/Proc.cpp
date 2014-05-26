// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Proc.h"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
namespace io = boost::iostreams;

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>
using std::string;
using std::vector;

extern char** environ;

/* public */

Proc::Proc(const string& name)
  : name(name),
    pid(-1),
    cmd(name),
    pipechat(true),
    env(environ) {
}

Proc::~Proc() {
  finish();
}

void Proc::run() {
  const int READ = 0;
  const int WRITE = 1;

  int tochild[2];
  int toparent[2];
  if (pipechat) {
    makePipe(tochild, "to child");
    makePipe(toparent, "to parent");
  }

  pid = fork();
  if (pid == -1) {
    perror((name + " fork").c_str()); exit(1);
  }

  if (pid == 0) {
    // child
    child_init();

    if (pipechat) {
      closePipe(tochild[WRITE], "child to child write");
      closePipe(toparent[READ], "child to parent read");

      if (STDIN_FILENO != tochild[READ]) {
        dupPipe(tochild[READ], STDIN_FILENO, "child to child input");
        closePipe(tochild[READ], "child to child input after dup");
      }
      if (STDOUT_FILENO != toparent[WRITE]) {
        dupPipe(toparent[WRITE], STDOUT_FILENO, "child to parent output");
        closePipe(toparent[WRITE], "child to parent output after dup");
      }

      // set stdin and stdout line-buffered
      setlinebuf(stdout);
      setlinebuf(stdin);
    }

    child_exec();
    perror((name + " failed to exit").c_str());
    _exit(1);
  }

  // parent
  if (pipechat) {
    closePipe(tochild[READ], "parent to child read");
    closePipe(toparent[WRITE], "parent to parent write");
    in.open(const_cast<const int&>(toparent[READ]), io::close_handle);
    out.open(const_cast<const int&>(tochild[WRITE]), io::close_handle);

    // Close the in and out file descriptors on any subsequent exec
    closeOnExec(in->handle(), "parent to parent input");
    closeOnExec(out->handle(), "parent to child output");
  }
}

void Proc::finish() {
  if (in.is_open()) in.close();
  if (out.is_open()) out.close();
}

/* protected */

void Proc::child_init() {
}

void Proc::child_exec() {
  if (string::npos == cmd.find_first_of('/')) {
    // use PATH
    string PATH(getenv("PATH"));
    size_t pos = 0;
    size_t sep_pos = PATH.find_first_of(':');
    while (sep_pos != string::npos) {
      path.push_back(PATH.substr(pos, sep_pos-pos));
      // Put a trailing / on the path, preceding the command name, if needed
      if ("/" != path.back().substr(path.back().length()-1, 1)) {
        path.back().push_back('/');
      }
      pos = sep_pos+1;
      sep_pos = PATH.find_first_of(':', sep_pos+1);
    }
  } else if (0 == cmd.find_first_of('/') ||
            "./" == cmd.substr(0,2) ||
            "../" == cmd.substr(0,3)) {
    path.push_back("");
  } else {
    path.push_back("./");
  }
  char** argv = new char*[args.size()+2];
  argv[0] = const_cast<char*>(cmd.c_str());
  int i = 0;
  for (vector<string>::const_iterator it = args.begin();
       it != args.end(); ++it) {
    ++i;
    argv[i] = const_cast<char*>(it->c_str());
  }
  argv[args.size()+1] = NULL;
  errno = 0;
  for (vector<string>::const_iterator i = path.begin();
       i != path.end(); ++i) {
    execve((*i + cmd).c_str(), argv, env);
    if (ENOENT != errno) break;
  }
  delete[] argv;
  perror((name + " failed to exec").c_str());
  exit(1);
}

void Proc::makePipe(int fds[2], const string& msg) {
  if (pipe(fds) == -1) {
    perror((name + " pipe " + msg).c_str()); exit(1);
  }
}

void Proc::closePipe(int fd, const string& msg) {
  if (-1 == close(fd)) {
    perror((name + " failed to close pipe " + msg).c_str());
    _exit(1);
  }
}

void Proc::dupPipe(int oldfd, int newfd, const string& msg) {
  if (-1 == dup2(oldfd, newfd)) {
    perror((name + " failed to dup " + msg).c_str());
    _exit(1);
  }
}

void Proc::closeOnExec(int fd, const string& msg) {
  if (0 != fcntl(fd, F_SETFD, 1)) {
    perror((name + " failed to set close-on-exec flag for " + msg).c_str());
    _exit(1);
  }
}
