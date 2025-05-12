#ifndef VM_HH
#define VM_HH

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class VM {
  public:
    VM();
    ~VM();

    void execute(const std::string &code);
    void reset();

  private:
    std::map<std::string, std::function<void()>> instructions;
    std::vector<int> stack;
    int programCounter;

    void initializeInstructions();
};

#endif