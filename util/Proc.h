// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Proc_h_
#define _Proc_h_

/* Parent-child process communication
 *
 * A Proc represents an interface between a parent and a child process.
 *
 * The parent constructs a Proc implementation and calls its .run(), which will
 * fork off a child.
 *
 * Proc is virtual -- an implementor represents the child process and provides
 * a child_exec() function, which is automatically called by the child, when
 * the parent calls Proc.run().  child_exec() is expected to exec a new
 * process, and thus never return.  The Proc and its members disappear to the
 * child.  The child's stdin and stdout redirect to communication channels with
 * the parent (unless pipechat is set false).  Its stderr remains connected to
 * the parent's stderr stream.
 *
 * The parent is left with the Proc, and communicates to the child via the
 * Proc.in and Proc.out member streams (unless pipechat is set false).
 */

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
namespace io = boost::iostreams;

#include <string>
#include <vector>

class Proc {
public:
  Proc(const std::string& name);
  ~Proc();

  void run();
  void finish();

  const std::string name;
  // Enables in/out pipes for communication between the parent and the child's
  // stdin/stdout.  Default: true.  If false, the child inherits the parent's
  // stdin/stdout (sketchy).
  pid_t pid;
  std::string cmd;      // command for child to invoke; defaults to name
  std::vector<std::string> args;  // args for the command
  std::vector<std::string> path;  // PATH to search if cmd does not contain a /
  bool pipechat;
  char** env;
  // Streams only used by parent
  io::stream<io::file_descriptor_source> in;
  io::stream<io::file_descriptor_sink> out;

protected:
  virtual void child_init();
  virtual void child_exec();

  void makePipe(int fds[2], const std::string& msg);
  void closePipe(int fd, const std::string& msg);
  void dupPipe(int oldfd, int newfd, const std::string& msg);
  void closeOnExec(int fd, const std::string& msg);
};

#endif // _Proc_h_
