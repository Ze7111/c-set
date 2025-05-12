"""
This script is used to build a C++ project. It provides functions for compiling and linking source files, as well as handling build configurations and dependencies.

The script supports different compilers based on the platform, such as cl.exe for Windows, clang++ for macOS, and g++ for other Unix-like systems.

Usage:
- Run the script with the appropriate command-line arguments to specify the build configuration (release or debug).
- The script will compile the source files and generate object files in the build directory.
- It will then link the object files to create the final executable in the bin directory.

Note: This script requires the 'rich' and 'gitpython' packages to be installed.
      It does install them if they are not found.

Author: Dhruvan Kartik
Date:   April 30 2024
"""

import os
import sys
import time
import pathlib
import logging
import platform
import threading
import subprocess
import concurrent.futures
from typing import Any

init_time: tuple[float, float] = [time.time(), -1]


def get_compiler(c_type: str):
    return (
        "cl.exe"
        if platform.system() == "Windows"
        else ("clang++" if c_type.lower() == "c++" else "clang")
        if platform.system() == "Darwin" else ("g++" if c_type.lower() == "c++" else "gcc")
    )


def get_triple(compiler: str):
    if compiler == "cl.exe":
        return "x86_64-pc-windows-msvc"
    try:
        result = subprocess.run(
            [compiler, "-dumpmachine"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
            text=True,
        )
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        print(f"failed to get target triple: {e.stderr}")
        sys.exit()
    except FileNotFoundError:
        print("failed to find compiler")
        sys.exit()


def get_standard(c_type: str):
    return f"{c_type.lower()}latest" if COMPILER == "cl.exe" else f"{c_type.lower()}2b"

C_TYPE:      str = "c++"
COMPILER:    str = get_compiler(C_TYPE)
STANDARD:    str = get_standard(C_TYPE)
TARGET:      str = get_triple(COMPILER)
DIRECTIVE:   str = "release" if len(sys.argv) > 2 and sys.argv[2] == "-r" else "debug"
MAIN_FILE:   str = os.path.join(os.getcwd(), "src", "main.cc")

DIRECTORIES: list[str] = ["/src"]
INCLUDES:    list[str] = []

PATH_SEP:   str = "/"
DEV_PROMPT: str = (
    "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat"
)
CLEAR:            bool = False    # clear the console after building
LINKER: dict[str, str] = {"cl.exe": "link.exe", "clang": "ld", "gcc": "ld"}

try:
    from rich.live import Live
    from rich.panel import Panel
    from rich.panel import Panel
    from rich.table import Table
    from rich.console import Console
    from rich.traceback import install
    from rich import print
    from rich.logging import RichHandler

    install()
except ImportError:
    exit_co = 0 if os.system(f"{sys.executable} -m pip install rich") == 0 else 1
    (
        print("failed to install rich, install it manually")
        if exit_co != 0
        else print("rich installed, re-run build.py")
    )
    sys.exit(1)
try:
    import git
except ImportError:
    exit_co = 0 if os.system(f"{sys.executable} -m pip install gitpython") == 0 else 1
    (
        print("failed to install gitpython, install it manually")
        if exit_co != 0
        else print("gitpython installed, re-run build.py")
    )
    sys.exit(1)

CONSOLE: Console = Console()
CLANG:   str     = "clang++" if C_TYPE.lower() == "c++" else "clang"
GCC:     str     = "g++"     if C_TYPE.lower() == "c++" else "gcc"

UNIX_COMPILE = [
    f"-std={STANDARD}",
    "-fexceptions",
    f"-o",
    f"{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
    *[f"-I{include}" for include in INCLUDES],
    "-c",
    "{source_file}",
]

WIN32_COMPILE = [
    "/EHsc",
    f"/Fo:{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
    *[f"/I{include}" for include in INCLUDES],
    "/c",
    "{source_file}",
]

SHARED_COMMANDS: dict[str, list[str]] = {
    "unix-release": [
        "-O3",
        "-flto",
    ],
    "unix-debug": [
        "-O0",
        "-g",
    ],
    "link-output": [
        "-o" if COMPILER != "cl.exe" else "/OUT:",
        os.getcwd()
        + PATH_SEP
        + "build"
        + PATH_SEP
        + DIRECTIVE
        + PATH_SEP
        + "bin"
        + PATH_SEP
        + "main"
        + (".exe" if COMPILER == "cl.exe" else ""),
    ],
    CLANG: [
        CLANG,
        f"--target={TARGET}",
    ],
    GCC: [GCC, f"--target={TARGET}"],
}

COMPILE_COMMANDS = {
    "cl.exe": {
        "release": ["cl.exe", f"/std:{STANDARD}", "/O2", "/GL", *WIN32_COMPILE],
        "debug": [
            "cl.exe",
            f"/std:{STANDARD}",
            "/Od",
            "/Zi",
            f"/Fd:{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}{{file_name_no_ext}}.pdb",
            *WIN32_COMPILE,
        ],
    },
    CLANG: {
        "release": [
            *SHARED_COMMANDS[CLANG],
            *SHARED_COMMANDS["unix-release"],
            *UNIX_COMPILE,
        ],
        "debug": [
            *SHARED_COMMANDS[CLANG],
            *SHARED_COMMANDS["unix-debug"],
            *UNIX_COMPILE,
        ],
    },
    GCC: {
        "release": [
            *SHARED_COMMANDS[GCC],
            *SHARED_COMMANDS["unix-release"],
            *UNIX_COMPILE,
        ],
        "debug": [
            *SHARED_COMMANDS[GCC],
            *SHARED_COMMANDS["unix-debug"],
            *UNIX_COMPILE,
        ],
    },
}
LINK_COMMANDS = {
    "cl.exe": {
        "release": [
            "link.exe",
            "/LTCG",
            "/OPT:REF",
            "/OPT:ICF",
            *SHARED_COMMANDS["link-output"],
        ],
        "debug": [
            "link.exe",
            "/DEBUG",
            "/DEBUGTYPE:cv,fixup",
            *SHARED_COMMANDS["link-output"],
        ],
    },
    CLANG: {
        "release": [
            *SHARED_COMMANDS[CLANG],
            *SHARED_COMMANDS["unix-release"],
            *SHARED_COMMANDS["link-output"],
        ],
        "debug": [
            *SHARED_COMMANDS[CLANG],
            *SHARED_COMMANDS["unix-debug"],
            *SHARED_COMMANDS["link-output"],
        ],
    },
    GCC: {
        "release": [
            *SHARED_COMMANDS[GCC],
            *SHARED_COMMANDS["unix-release"],
            *SHARED_COMMANDS["link-output"],
        ],
        "debug": [
            *SHARED_COMMANDS[GCC],
            *SHARED_COMMANDS["unix-debug"],
            *SHARED_COMMANDS["link-output"],
        ],
    },
}

files: list[str] = [MAIN_FILE.replace(os.sep, PATH_SEP)]
changed_files: list[str] = []
LOG = logging.getLogger("rich")
logging.basicConfig(
    level="INFO", format="%(message)s", datefmt="[%X]", handlers=[RichHandler()]
)


def run_command_with_dev(command: list[str]) -> None:
    command = ["cmd", "/k", f'"{DEV_PROMPT}"' "&"] + command
    LOG.warning(
        f"can not run '{command[0]}' without the Developer Command Prompt"
    )
    live_render.stop()
    #os.system(" ".join(command))


def run_command_without_dev(command: list[str]) -> tuple[bytes, bytes]:
    return subprocess.Popen(
        command, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    ).communicate()

def is_valid() -> bool:
    if COMPILER == "cl.exe":
        if os.environ.get("VSINSTALLDIR") is None:
            LOG.warning("you are not in the Dev Prompt")
            LOG.debug("start vscode from the Developer Command Prompt")
            return False
    return True


def run_command(command: list[str]) -> tuple[bytes, bytes] | None:
    try:
        if is_valid():
            pipe, return_code = run_command_without_dev(command)
        else:
            run_command_with_dev(command)
            return None, None
    except subprocess.CalledProcessError as e:
        LOG.critical(f"failed to run command: '{' '.join(command)}', errored with: {e}")
        return None, None
    except FileNotFoundError:
        LOG.critical(
            f"unable to find '{command[0]}' in PATH"
            + ", running with Developer Command Prompt"
            if COMPILER == "cl.exe"
            else ""
        )
        return None, None

    if return_code != b"" and not return_code.endswith(b"\r\n\r\n"):
        LOG.error(f"failed to run command: '{' '.join(command)}'")
        LOG.error(
            f"'{command[0]}' failed with exit code: {return_code.decode("utf-8")}, got the following output: \"{pipe.decode("utf-8")}\""
        )
        return None, None

    return pipe, return_code


def get_files(path: str) -> None:
    global files
    [
        (
            get_files(os.path.join(path, f))
            if os.path.isdir(os.path.join(path, f))
            else files.append(
                os.path.join(path, f).replace("/", PATH_SEP).replace("\\", PATH_SEP)
            )
        )
        for f in os.listdir(path)
    ]
    [
        files.remove(f)
        for f in files
        if (
            not f.endswith(".cc")
            and not f.endswith(".cpp")
            and not f.endswith(".cxx")
            and not f.endswith(".c")
            and not f.endswith(".i")
        )
    ]


def build_cmd(file: str) -> str:
    source_stem = pathlib.Path(file).stem
    command: list[str] = [
        cmd.replace("{file_name_no_ext}", source_stem).replace("{source_file}", file)
        for cmd in COMPILE_COMMANDS[COMPILER][DIRECTIVE]
        if cmd
    ]
    return command, source_stem


live_render: Live
shutdown_live: bool = False
compiled_files: list[str] = []


def handle_build_check(file: str, pipe: bytes, return_code: bytes) -> bool:
    if (
        (return_code is None or pipe is None)
        or (return_code != b""
        and not return_code.endswith(b"\r\n\r\n"))
    ):
        live_render.stop()

        if file in files:
            files[files.index(file)] = None
        #else:
        #    LOG.critical(f"this is weird, but uh, '{file}' is not in the files list")
        #    LOG.critical(f"files list: {files}")

        return False
    return True


def build(file: str) -> None:
    global live_render

    command, source_stem = build_cmd(file)
    compiled_path = f"{os.getcwd().replace(os.sep, PATH_SEP)}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{source_stem}.obj"

    LOG.debug(
        f"building {PATH_SEP}{os.path.relpath(file).replace(os.sep, PATH_SEP)} -> {PATH_SEP}{os.path.relpath(compiled_path).replace(os.sep, PATH_SEP)}"
    )
    pipe, return_code = run_command(command)
    files[files.index(file)] = compiled_path

    if not handle_build_check(file, pipe, return_code):
        return

    compiled_files.append(file)


def display_linker_files():
    liker_table = Table(highlight=True)
    liker_table.add_column("linking -> files")

    for file in files:
        liker_table.add_row(file)

    CONSOLE.print(liker_table)


def link_cmd() -> list[str]:
    return [cmd for cmd in LINK_COMMANDS[COMPILER][DIRECTIVE] if cmd] + files


def handle_link_check(pipe: bytes, return_code: bytes) -> bool:
    if (
        return_code is None
        or return_code != b""
        and not return_code.endswith(b"\r\n\r\n")
    ):
        LOG.critical(f"failed to link all files")
        return False
    return True


def link() -> float | None:
    link_time: list[float, float] = [time.time(), -1]
    display_linker_files()

    command = link_cmd()
    pipe, return_code = run_command(command)

    if not handle_link_check(pipe, return_code):
        return None

    link_time[1] = time.time()
    return link_time[1] - link_time[0]


def create_dirs():
    dir = os.getcwd() + PATH_SEP + "build" + PATH_SEP + DIRECTIVE + PATH_SEP
    if not os.path.exists(dir + "generated"):
        os.makedirs(dir + "generated")
    if not os.path.exists(dir + "bin"):
        os.makedirs(dir + "bin")


def check_compiler() -> bool:
    if COMPILER in COMPILE_COMMANDS:
        return True

    LOG.error(f"'{COMPILER}' is not defined in the compiler commands")

    compilers: list[str] = list(COMPILE_COMMANDS.keys())

    table: Table = Table(highlight=True)
    table.add_column("compilers")
    for compiler in compilers:
        table.add_row(compiler)

    CONSOLE.print(table)
    return False


def show_live_render():
    global live_render

    with live_render as render:
        while shutdown_live is False:
            render.update(generate_table())


def generate_table() -> Table:
    table: Table = Table(highlight=True)
    table.add_column("files -> compiled")

    for file in compiled_files:
        table.add_row(file)

    return table


def fork_compiler(workers: concurrent.futures.ThreadPoolExecutor):
    global live_render, shutdown_live, files
    live_render = Live(generate_table(), refresh_per_second=10)

    render = threading.Thread(target=show_live_render)
    render.start()

    # remove any duplicates in the files:
    files = list(set(files))

    try:
        build_times: list[float] = [time.time(), -1]

        workers.map(build, files)
        workers.shutdown(wait=True)

        # for file in files:
        #    build(file)

        build_times[1] = time.time()
        live_render.update(generate_table())

    finally:
        shutdown_live = True
        render.join(0.1)

    return build_times[1] - build_times[0]


def check_files():
    if len(files) == 0:
        LOG.critical("failed to find any files to compile")
        return False

    if len(files) == 1:
        LOG.warning("only found one file to compile")
        LOG.warning("this is not recommended, add more files to compile")
        LOG.warning("to add more files, add them to the src folder")
        LOG.warning("or run the build tool from the src folder")

    for file in files:
        if not os.path.exists(file):
            LOG.critical(f"failed to find {file}")
            return False

    return True


def file_hash(filepath):
    import hashlib

    hasher = hashlib.sha256()
    with open(filepath, "rb") as f:
        buf = f.read()
        hasher.update(buf)
    return hasher.hexdigest()


def identify_changed_files() -> bool:
    import json

    global changed_files
    state_file = "build/state.lock"

    try:
        repo = git.Repo(search_parent_directories=True)
    except git.InvalidGitRepositoryError:
        LOG.error("this is not a git repo, cannot track changes")
        LOG.error("will still run but theres no caching")
        return True

    current_files = {item.a_path for item in repo.index.diff(None)}
    current_files.update({item.a_path for item in repo.index.diff("HEAD")})
    current_files.update(repo.untracked_files)

    current_files_state = {
        file: file_hash(os.path.join(repo.working_dir, file))
        for file in current_files
        if os.path.exists(os.path.join(repo.working_dir, file))
    }

    if os.path.exists(state_file):
        with open(state_file, "r") as file:
            previous_files_state = json.load(file)
    else:
        previous_files_state = {}

    changes = {}
    for file, hash in current_files_state.items():
        if file not in previous_files_state or previous_files_state[file] != hash:
            changes[file] = "modified"
    for file in previous_files_state:
        if file not in current_files_state:
            changes[file] = "removed"

    with open(state_file, "w") as file:
        json.dump(current_files_state, file, indent=4)

    changed_files = list(changes.keys())
    LOG.info(f"found {len(changed_files)} changed files")

    return len(changes) > 0

def initialize_workers(amt: int) -> concurrent.futures.ThreadPoolExecutor:
    LOG.info(f"spawning {amt} workers")

    return (
        concurrent.futures.ThreadPoolExecutor(
            max_workers=amt,
            initializer=lambda: None,
        )
    )

def init():
    init_time[1] = time.time()

    max_workers = (
        len(files) + 1
        if len(files) + 1 < (os.cpu_count() * 2)
        else (os.cpu_count() * 2)
    )
    workers = initialize_workers(max_workers)

    for dirs in DIRECTORIES:
        get_files(os.getcwd() + dirs)

    create_dirs()

    if not identify_changed_files() and os.path.exists(
        f"build{PATH_SEP}{DIRECTIVE}{PATH_SEP}bin{PATH_SEP}main{'.exe' if COMPILER == 'cl.exe' else ''}"
    ):
        LOG.debug("no files have changed, skipping build")

        build_times = 0
        link_time = 0
    else:
        if not check_files():
            sys.exit(1)
        if not check_compiler():
            sys.exit(1)

        build_times = fork_compiler(workers)
        if compiled_files:
            link_time = link()
        else:
            LOG.critical("no files were compiled, exiting")
            sys.exit(1)

    if CLEAR: os.system("cls" if sys.platform == "win32" else "clear")
    CONSOLE.print(
        Panel(
            f"built all files in {build_times:.3f}s\nlinked all files in {link_time:.3f}s",
            title="build time",
            highlight=True,
        )
    )

    compiled_path = (
        os.getcwd()
        + os.sep
        + "build"
        + os.sep
        + DIRECTIVE
        + os.sep
        + "bin"
        + os.sep
        + "main"
        + (".exe" if COMPILER == "cl.exe" else "")
    )

    run_time: list[float, float] = [time.time(), -1]

    if os.path.exists(compiled_path):
        os.system(compiled_path)
    else:
        LOG.error(f"failed to find {compiled_path}")

    run_time[1] = time.time()

    CONSOLE.print(
        Panel(
            f"runtime for main is {(run_time[1] - run_time[0]):.3f}s\ntotal time is {(run_time[1] - init_time[1]):.3f}s",
            border_style="red",
            highlight=True,
        )
    )


if __name__ == "__main__":
    CONSOLE.print(
        Panel(
            f"Ze7111 (R) Dhruvan's custom build tool V0.9 for '{TARGET}'\nCopyright {time.strftime('%Y')} (C) Apache License, Version 2.0."
        )
    )
    if COMPILER == "cl.exe" and platform.system() != "Windows":
        LOG.critical("cl.exe is only available on windows")
        sys.exit(1)

    if not os.path.exists(MAIN_FILE):
        LOG.critical(f"failed to find {MAIN_FILE}")
        sys.exit(1)
    init()
    sys.exit(0)
