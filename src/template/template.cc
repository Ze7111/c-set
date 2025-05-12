#include "template.hh"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <locale>
#include <codecvt>

#include "../../pkgs/base/include.hh"
#include "lexer.hh"


Template::Template(std::string &file)
    : filePath(file)
    , open(false)
    , valid(false) {
    load();
}

Template::~Template() { close(); }

auto Template::load() -> void {
    if (open) {
        close();
    }

    std::vector<lexer::token> parsed = lexer::lex(filePath);

    open = true;
    valid = true;
}

auto Template::close() -> void {
    if (open) {
        open = false;
        valid = false;
    }
}

auto Template::get(const std::string &key) -> std::string {
    return "nun";
}
