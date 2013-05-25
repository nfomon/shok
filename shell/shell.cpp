#include <cstdio>
#include <iostream>
#include <fstream>
using namespace std;

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
namespace io = boost::iostreams;

#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

const string PROGRAM_NAME = "lush";
const string PROMPT = "lush: ";

void setlinebuf(FILE* stream) {
  setvbuf(stream, (char*)NULL, _IOLBF, 0);
}

class Proc {
public:
  Proc(const string& name)
    : name(name) {
  }
  ~Proc() {}

  void run() {
    int pipefds_toproc[2];
    int pipefds_toshell[2];
    if (pipe(pipefds_toproc) == -1) {
      perror((name + " pipe to proc").c_str()); exit(1);
    }
    if (pipe(pipefds_toshell) == -1) {
      perror((name + " pipe to shell").c_str()); exit(1);
    }
    pid = fork();
    if (pid == -1) {
      perror((name + " fork").c_str()); exit(1);
    }
    const int toproc_read = pipefds_toproc[0];
    const int toproc_write = pipefds_toproc[1];
    const int toshell_read = pipefds_toshell[0];
    const int toshell_write = pipefds_toshell[1];

    if (pid == 0) {
      // child
      if (-1 == close(toproc_write)) {
        perror((name + " failed to close toproc_write").c_str());
        _exit(1);
      }
      if (STDIN_FILENO != toproc_read) {
        if (-1 == dup2(toproc_read, STDIN_FILENO)) {
          perror((name + " failed to dup input").c_str());
          _exit(1);
        }
        if (-1 == close(toproc_read)) {
          perror((name + " failed to close toproc_read after dup").c_str());
          _exit(1);
        }
      }

      if (-1 == close(toshell_read)) {
        perror((name + " failed to close toshell_read").c_str());
        _exit(1);
      }
      if (STDOUT_FILENO != toshell_write) {
        if (-1 == dup2(toshell_write, STDOUT_FILENO)) {
          perror((name + " failed to dup output").c_str());
          _exit(1);
        }
        if (-1 == close(toshell_write)) {
          perror((name + " failed to close toshell_write after dup").c_str());
          _exit(1);
        }
      }
      // set stdin and stdout line-buffered
      setlinebuf(stdout);
      setlinebuf(stdin);
      f();
      perror((name + " failed to exit").c_str());
      _exit(1);
    }

    // parent (shell)
    close(toproc_read);
    out.open(toproc_write, io::close_handle);
    close(toshell_write);
    in.open(toshell_read, io::close_handle);
  }

  // This is pretty useless...
  bool isRunning() {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == -1) {
      perror((name + " failed querying child process status").c_str());
    }
    return result == 0;
  }

  const string name;
  pid_t pid;
  io::stream<io::file_descriptor_source> in;
  io::stream<io::file_descriptor_sink> out;

protected:
  virtual void f() = 0;
};

class Lexer : public Proc {
public:
  Lexer() : Proc("lexer") {}
protected:
  virtual void f() {
    execlp("./lush_lexer", "lush_lexer", (char*)NULL);
  }
};

class Parser : public Proc {
public:
  Parser() : Proc("parser") {}
protected:
  virtual void f() {
    execlp("./lush_parser", "lush_parser", (char*)NULL);
  }
};

int main(int argc, char *argv[]) {
  if (1 != argc) {
    cout << "usage: " << PROGRAM_NAME << endl;
    return 1;
  }

  Lexer lexer;
  lexer.run();

  Parser parser;
  parser.run();

  cout << PROMPT;
  string line;
  while (std::getline(cin, line)) {
    // send line to lexer
    lexer.out << line << endl;

    // get tokens
    string tokens;
    std::getline(lexer.in, tokens);
    //cout << "[shell] tokens: '" << tokens << "'" << endl;
    if (!lexer.isRunning()) {
      cout << "[shell] Lexer error; aborting" << endl;
      break;
    }

    // send tokens to parser
    parser.out << tokens << endl;
    //cout << "sent '" << tokens << "' to parser" << endl;
    if (!parser.isRunning()) {
      cout << "[shell] Parser error; aborting" << endl;
      break;
    }

    // get AST
    string ast;
    std::getline(parser.in, ast);
    if ("" != ast) {
      cout << "[shell] ast: '" << ast << "'" << endl;
    }

    // send AST to eval
    // get commands
    // run commands
    // get output
    // display output

    // redisplay prompt
    cout << PROMPT;
  }
  cout << "Done" << endl;
  cout << endl;
  lexer.out.close();
  lexer.in.close();
  parser.out.close();
  parser.in.close();

  if (-1 == wait(NULL)) {
    perror("waiting for child");
    _exit(1);
  }
  return 0;
}
