/*
flow:

run 1:
$ ./c-set
? do you want to add the path to the user PATH? [y/n]: y
? default compiler? [none]: g++
? default standard? [none]: 17
? default build type? [none] (r|d): r

run 2 ( if no compiler, standard, or build type is set ):
$ ./c-set
? use entry point? [y/n]: y
    ? entry file name? [main] (created if it does not exist): main.cc
? which compiler? [none]: g++
? which standard? [none]: 17
? which build type? [none] (r|d): r

- if the user chooses to use the entry point, the file is created with the
following content:
    #include "incl/default.hh"

    auto main() -> int {
        std_v2::print("Hello, World!");
        return 0;
    }

- copy files from the template directory to the current directory
- copy folders from the template directory to the current directory
- replace the placeholders in the files with the user's input
- create a .c-set file in the current directory with the user's input


*/

#include <utility>
#include <locale>
#include <codecvt>

#include "src/template/template.hh"
#include "src/vm/vm.hh"

auto main() -> int {
    std::locale::global(std::locale("en_US.UTF-8"));
    //std::string file = "c++.c-set-template/.template";
    //const Template config(file);
    return 0;
}
