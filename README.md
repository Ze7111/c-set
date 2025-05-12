# c-set

A modern, extensible, and embeddable scripting language and virtual machine, written in C++. It features a custom lexer, parser, semantic analyzer, code generator, and a stack-based virtual machine. The project is designed for flexibility, async/threaded lexing, and easy integration with external modules and configuration systems.

## Features

- **Custom Language**: Variables, functions, imports, control flow (`if`, `while`, `return`, etc.), arithmetic, logic, and more.
- **Stack-based Virtual Machine**: Executes bytecode generated from source code.
- **Async Threading**: Lexer can tokenize input asynchronously using a thread manager, improving performance for large files.
- **Symlink Support**: Designed to work with symlinked directories and files, making it easy to organize and share code.
- **Configurable Build System**: Build scripts support multiple compilers (MSVC, Clang, GCC) and platforms (Windows, macOS, Linux).
- **Extensible**: Easily add new opcodes, language features, or modules.
- **Template System**: Includes a template interpreter and configuration system for project scaffolding and customization.
- **Rich Error Reporting**: Semantic and runtime errors are reported with context.

## Project Structure

- `src/lexer/` — Lexer, token definitions, and async/threading support.
- `src/ast/` — AST node definitions and parser.
- `src/sema/` — Semantic analysis and symbol table.
- `src/vm/` — Bytecode, code generation, and VM implementation.
- `src/templates/` — Project templates, config system, and interpreter scripts.
- `build.py` — Python build script for compiling and linking.
- `xmake.lua` — XMake build configuration for multi-platform support.

## Language Overview

- **Variables**: `var x = 42;`
- **Functions**: `function foo(a, b) { return a + b; }`
- **Control Flow**: `if`, `else`, `while`, `break`, `continue`, `return`
- **Expressions**: Arithmetic (`+`, `-`, `*`, `/`, `%`), logic (`&&`, `||`, `!`), comparison (`==`, `!=`, `<`, `>`, `<=`, `>=`)
- **Imports**: `import time;`
- **Literals**: Numbers, strings (with escape support), identifiers

## Async Threading

The lexer is designed to run asynchronously using a `WorkerThread` manager. This allows tokenization to be performed in the background, making the system responsive even for large files.

## Symlink Usage

The project supports symlinked source and include directories, making it easy to share code between projects or organize modules in a flexible way.

## Build Instructions

### Using XMake

```sh
xmake
xmake run c-set
```

- Configure for your platform:
  ```
  xmake f -p [macosx|linux|windows] -a [x86_64|arm64] -m [debug|release]
  ```

### Using build.py

```sh
python3 build.py [release|debug]
```

- The script auto-detects your platform and compiler.
- Output binaries are placed in `build/{debug|release}/bin/`.

### Requirements

- C++23 compiler (Clang, GCC, or MSVC)
- Python 3.8+ (for build.py)
- [rich](https://github.com/Textualize/rich) and [gitpython](https://github.com/gitpython-developers/GitPython) Python packages (auto-installed by build.py if missing)

## Configuration and Templates

- The `src/templates/config.hh` and `config.cc` provide a TOML-based configuration system.
- The `.template` and `intrepreter.s` files show how to scaffold new projects and scripts, with support for variable substitution and platform-specific logic.

## Extending the Language

- Add new opcodes in `src/vm/bytecode.hh` and implement them in `src/vm/vm.cc`.
- Extend the parser and AST in `src/ast/ast.hh` and `parser.hh`.
- Add new keywords or tokens in `src/lexer/lexemes.hh`.

## License

This project is licensed under the Apache License 2.0. See the [LICENSE](LICENSE) file for details.

---

For more details, see the code comments and the `docs.md` file. If you have questions or want to contribute, please open an issue or pull request!
