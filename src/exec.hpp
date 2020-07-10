#ifndef exec_hpp
#define exec_hpp

#include <string>

class ExecResult {
public:
  int exit_code;
  std::string out;
  std::string err;
  ExecResult() {
    exit_code = 0;
    out = "";
    err = "";
  }
};

extern ExecResult exec_program(const char * const *command, std::string input);

#endif /* exec_hpp */
