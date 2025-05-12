#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
#include <vector>

#include "ast/ast.hh"
#include "ast/parser.hh"
#include "lexer/lexer.hh"
#include "lexer/tokens.hh"
#include "thread/worker.hh"
#include "vm/codegen.hh"
#include "vm/vm.hh"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::wcerr << L"Usage: " << argv[0] << L" <source-file>\n";
        return 1;
    }

    std::locale::global(std::locale("en_US.UTF-8"));

    std::wstring filename =
        std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(argv[1]);
    std::wcout << L"Reading source file: " << filename << std::endl;

    // lexing
    std::wcout << L"--- Lexing ---" << std::endl;
    using ThreadManager = WorkerThread;
    ThreadManager worker;
    Lexer::Lexer<ThreadManager> lexer(filename, &worker);

    std::vector<Lexer::Token> tokens;
    for (auto t : lexer.tokenize()) {
        tokens.push_back(t);
        std::wcout << L"Token: " << t.value << L" (" << static_cast<int>(t.kind)
                   << L")\n";
    }

    if (tokens.empty()) {
        std::wcerr << L"No tokens produced. Exiting.\n";
        return 1;
    }

    // parsing
    std::wcout << L"\n--- Parsing ---" << std::endl;
    ParserContext ctx;
    Parser parser(tokens, ctx);
    ASTNodePtr ast = parser.parse_program();

    if (!ast) {
        std::wcerr << L"Parsing failed. Exiting.\n";
        return 1;
    }
    std::wcout << L"AST successfully built.\n";

    // code generation
    std::wcout << L"\n--- Code Generation ---" << std::endl;
    CodeGen codegen;
    Bytecode bytecode;
    try {
        bytecode = codegen.generate(ast);
    } catch (const std::exception &ex) {
        std::wcerr << L"Code generation error: " << ex.what() << std::endl;
        return 1;
    }
    std::wcout << L"Bytecode generated (" << bytecode.size()
               << L" instructions):\n";
    for (size_t i = 0; i < bytecode.size(); ++i) {
        const auto &instr = bytecode[i];
        std::wcout << i << L": " << static_cast<int>(instr.op);
        if (std::holds_alternative<int64_t>(instr.operand)) {
            std::wcout << L" " << std::get<int64_t>(instr.operand);
        } else if (std::holds_alternative<std::wstring>(instr.operand)) {
            std::wcout << L" \"" << std::get<std::wstring>(instr.operand)
                       << L"\"";
        } else if (std::holds_alternative<size_t>(instr.operand)) {
            std::wcout << L" @" << std::get<size_t>(instr.operand);
        }
        std::wcout << std::endl;
    }

    // vm execution
    std::wcout << L"\n--- VM Execution ---" << std::endl;
    VM vm;
    vm.load(bytecode, codegen.ctx.globals.size());
    try {
        vm.run();
    } catch (const std::exception &ex) {
        std::wcerr << L"Runtime error: " << ex.what() << std::endl;
        return 1;
    }

    std::wcout << L"Execution finished successfully.\n";
    return 0;
}