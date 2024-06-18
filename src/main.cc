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
- createx a .c-set file in the current directory with the user's input
*/

#include <utility>
#include <locale>
#include <codecvt>

#include "template/template.hh"
#include "vm/vm.hh"
#include "ast/tree.hh"

auto main() -> int {
    std::locale::global(std::locale("en_US.UTF-8"));
    std::print("testing x96_x84 memory");

    vm::TreeWalker tree_walker;
    tree_walker.mem_test_x96_x84();

    std::string file = "/Volumes/Container/Projects/c-set/c++.c-set-template/.template";
    Template template_(file);
    auto tokens = template_.load();
    
    std::print(colors::fg16::red, "---------------------- TREE GEN ----------------------", colors::reset);

    Tree tree_(tokens, &tree_walker);
    tree_.generate();

    return 0;
}
