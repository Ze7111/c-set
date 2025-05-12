import os
import sys
import time
import pathlib
import platform
import threading
import subprocess
import concurrent.futures
from typing import Any

init_time: tuple[float, float] = [time.time(), -1]

try:
    from rich.live import Live
    from rich.panel import Panel
    from rich.panel import Panel
    from rich.table import Table
    from rich.console import Console
    from rich.traceback import install
    from rich import print
    install()
except ImportError:
    print("rich is missing, trying to install it...")
    exit_co = 0 if os.system(f"{sys.executable} -m pip install rich") == 0 else 1
    print("failed to install rich, install it manually") if exit_co != 0 else print("rich installed, re-run build.py")
    sys.exit(1)

try:
    import git
except ImportError:
    print("git is missing, trying to install it...")
    exit_co = 0 if os.system(f"{sys.executable} -m pip install gitpython") == 0 else 1
    print("failed to install gitpython, install it manually") if exit_co != 0 else print("gitpython installed, re-run build.py")
    sys.exit(1)
INCLUDES:  list[str] = [
    #r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\include",
    #r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.39.33519\ATLMFC\include",
    #r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\VS\include",
    #r"C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\ucrt",
    #r"C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\um",
    #r"C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\shared",
    #r"C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\winrt",
    #r"C:\Program Files (x86)\Windows Kits\10\include\10.0.22621.0\cppwinrt",
    #r"C:\Program Files (x86)\Windows Kits\NETFXSDK\4.8\include\um",
]

TARGET = ("x86_64-pc-windows" if os.name == "nt" else "x86_64-pc-darwin" if os.name == "darwin" else "x86_64-pc-linux")
DEV_PROMPT = "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\Common7\\Tools\\VsDevCmd.bat"
VERBOSE:   bool    = False
COMPILER:  str     = sys.argv[1] if len(sys.argv) > 1 else ("msvc" if os.name == "nt" else "clang")
PATH_SEP:  str     = '/'
DIRECTIVE: str     = "release" if len(sys.argv) > 2 and sys.argv[2] == "-r" else "debug"
CONSOLE:   Console = Console()
MAIN_FILE: str     = os.path.join(os.getcwd(), "main.cc")
STANDARD:  str     = "c++latest" if COMPILER == "msvc" else "c++2b"
COMPILE_COMMANDS = {
    "msvc": {
        "release": [
            "cl.exe",
            f"/std:{STANDARD}",
            "/O2",  # Enable optimizations
            "/GL",  # Whole program optimization
            "/EHsc",
            f"/Fo:{os.getcwd()}{PATH_SEP}build{PATH_SEP}release{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            f"/Fd:{os.getcwd()}{PATH_SEP}build{PATH_SEP}release{PATH_SEP}{{file_name_no_ext}}.pdb",
            *[f"/I{include}" for include in INCLUDES],
            "/c",
            "{source_file}"
        ],
        "debug": [
            "cl.exe",
            f"/std:{STANDARD}",
            "/Od",
            "/Zi",
            "/EHsc",
            f"/Fo:{os.getcwd()}{PATH_SEP}build{PATH_SEP}debug{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            f"/Fd:{os.getcwd()}{PATH_SEP}build{PATH_SEP}debug{PATH_SEP}{{file_name_no_ext}}.pdb",
            *[f"/I{include}" for include in INCLUDES],
            "/c",
            "{source_file}"
        ]
    },
    "clang": {
        "release": [
            "clang++",
            f"-std={STANDARD}",
            "-O3",  # Enable optimizations
            "-flto",  # Link time optimization
            "-fexceptions",
            f"-o", f"{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            *[f"-I{include}" for include in INCLUDES],
            "-c",
            "{source_file}"
        ],
        "debug": [
            "clang++",
            f"-std={STANDARD}",
            "-O0",  # No optimizations
            "-g",  # Generate debug information
            "-fexceptions",
            f"-o", f"{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            *[f"-I{include}" for include in INCLUDES],
            "-c",
            "{source_file}"
        ]
    },
    "gcc": {
        "release": [
            "g++",
            f"-std={STANDARD}",
            "-O3",  # Enable optimizations
            "-flto",  # Link time optimization
            "-fexceptions",
            f"-o", f"{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            *[f"-I{include}" for include in INCLUDES],
            "-c",
            "{source_file}"
        ],
        "debug": [
            "g++",
            f"-std={STANDARD}",
            "-O0",  # No optimizations
            "-g",  # Generate debug information
            "-fexceptions",
            f"-o", f"{os.getcwd()}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{{file_name_no_ext}}.obj",
            *[f"-I{include}" for include in INCLUDES],
            "-c",
            "{source_file}"
        ]
    }
}
LINK_COMMANDS = {
    "msvc": {
        "release": [
            "link.exe",
            "/LTCG",  # Link-time code generation for better optimization
            "/OPT:REF",  # Eliminate unused code
            "/OPT:ICF",  # Perform COMDAT folding
            "/OUT:" + os.getcwd() + PATH_SEP + "build" + PATH_SEP + "release" + PATH_SEP + "bin" + PATH_SEP + "main.exe"
        ],
        "debug": [
            "link.exe",
            "/DEBUG",  # Generate debug information
            "/DEBUGTYPE:cv,fixup",  # Specify the type of debugging information generated
            "/OUT:" + os.getcwd() + PATH_SEP + "build" + PATH_SEP + "debug" + PATH_SEP + "bin" + PATH_SEP + "main.exe",
        ]
    },
    "clang": {
        "release": [
            "clang++",
            # "-target", "aarch64-unknown-linux-gnu",
            "-O3",  # Optimization level 3
            "-flto",  # Link Time Optimization
            "-o", os.getcwd() + PATH_SEP + "build" + PATH_SEP + "release" + PATH_SEP + "bin" + PATH_SEP + "main",
        ],
        "debug": [
            "clang++",
            # "-target", "aarch64-unknown-linux-gnu",
            "-g",  # Generate source-level debug information
            "-o", os.getcwd() + PATH_SEP + "build" + PATH_SEP + "debug" + PATH_SEP + "bin" + PATH_SEP + "main",
        ]
    },
    "gcc": {
        "release": [
            "g++",
            # "-march=arm64",
            # "-mtune=generic",
            "-O3",  # Optimization level 3
            "-flto",  # Link Time Optimization
            "-o", os.getcwd() + PATH_SEP + "build" + PATH_SEP + "release" + PATH_SEP + "bin" + PATH_SEP + "main",
        ],
        "debug": [
            "g++",
            # "-march=arm64",
            # "-mtune=generic",
            "-g",  # Generate source-level debug information
            "-o", os.getcwd() + PATH_SEP + "build" + PATH_SEP + "debug" + PATH_SEP + "bin" + PATH_SEP + "main",
        ]
    }
}
files: list[str] = [MAIN_FILE]
changed_files: list[str] = []

def get_default_compiler():
    if platform.system() == "Windows":
        return "cl"  # Default to MSVC on Windows
    elif platform.system() == "Darwin":
        return "clang++"  # Default to Clang on macOS
    else:
        return "g++"  # Default to GCC on Linux

def get_compiler_target_triple(compiler_command=None):
    if not compiler_command:
        compiler_command = get_default_compiler()

    if compiler_command == "cl":
        command = [compiler_command, '/?']
    else:
        command = [compiler_command, '-dumpmachine']

    try:
        result = subprocess.run(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=True,
            text=True
        )
        if compiler_command == "cl":
            return "MSVC does not directly support -dumpmachine, use other methods to determine architecture."
        return result.stdout.strip()
    except subprocess.CalledProcessError as e:
        print(f"Error getting target triple: {e}")
        return None
    except FileNotFoundError:
        print("Compiler not found. Please ensure the compiler path is correctly set.")
        return None

def get_files(path: str) -> None:
    global files; [
        (    get_files     (os.path.join(path, f))
        if   os.path.isdir (os.path.join(path, f))
        else files  .append(os.path.join(path, f)
                    .replace("/", PATH_SEP).replace("\\", PATH_SEP)
        ))
        for  f in os.listdir(path)
    ]; [
        files.remove(f)
        for f in files
        if  (not f.endswith(".cc")
        and not f.endswith(".cpp")
        and not f.endswith(".cxx")
        and not f.endswith(".c"))
        #or  open(f).read().strip() == ""
    ]

def timeit(func: callable, *args, **kwargs) -> tuple[Any, float]:
    start = time.time()
    ret   = func(*args, **kwargs)
    end   = time.time()
    return ret, end - start

def cleanup_build_cmd(file: str) -> str:
    source_stem = pathlib.Path(file).stem
    command: list[str] = [
        cmd.replace("{file_name_no_ext}", source_stem)
           .replace("{source_file}", file)
        for cmd in COMPILE_COMMANDS[COMPILER][DIRECTIVE]
        if cmd
    ]
    return command, source_stem

live_render: Live
compiled_files: list[str] = []

def build(file: str) -> None:
    global live_render

    command, source_stem = cleanup_build_cmd(file)
    compiled_path = f"{os.getcwd().replace(os.sep, PATH_SEP)}{PATH_SEP}build{PATH_SEP}{DIRECTIVE}{PATH_SEP}generated{PATH_SEP}{source_stem}.obj"

    try:
        pipe, return_code = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        ).communicate()
    except FileNotFoundError:
        live_render.stop()
        if COMPILER == "msvc":
            CONSOLE.print("make sure you are in the [red]Developer Command Prompt[/red]")
            CONSOLE.print("if using vscode run code from the Dev Prompt")
            CONSOLE.print("attempting to re-run the build tool in the Dev Prompt...")
            os.system(f"cmd /k \"{DEV_PROMPT}\" & {sys.executable} {' '.join(sys.argv)}")
        return

    files[files.index(file)] = compiled_path

    if return_code != b'' and not return_code.endswith(b"\r\n\r\n"):
        live_render.stop()
        CONSOLE.print(f"failed to build {file}.\nexit code: {return_code}")
        CONSOLE.print(f"'{COMPILE_COMMANDS[COMPILER][DIRECTIVE][0]}' tried to run with the following command:" + "\n" + " ".join(command) + "\n" + " ".join(pipe.decode("utf-8").split("\n")))
        if file in files:
            files[files.index(file)] = None
        else: CONSOLE.print(files)
        sys.exit(1)

    compiled_files.append(file)

def link() -> float | None:
    link_time: list[float, float] = [time.time(), -1]

    liker_table = Table(highlight=True)
    liker_table.add_column("linking -> files")

    for file in files:
        liker_table.add_row(file)

    CONSOLE.print(liker_table)

    command: list[str] = [
        cmd
        for cmd in LINK_COMMANDS[COMPILER][DIRECTIVE]
        if cmd
    ] + files

    if VERBOSE: CONSOLE.print(f"linking all files in '{DIRECTIVE}' mode with '{COMPILER}'.")
    if VERBOSE: CONSOLE.print(command)

    if not os.environ.get("PATH").split(os.pathsep)[0] + PATH_SEP + command[0]:
        CONSOLE.print(f"failed to find {command[0]} in PATH")
        if COMPILER == "msvc":
            CONSOLE.print("make sure you are in the [red]Developer Command Prompt[/red]")
            CONSOLE.print("if using vscode run code from the Dev Prompt")
        return

    try:
        pipe, return_code = subprocess.Popen(
            command,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        ).communicate()
    except FileNotFoundError:
        if COMPILER == "msvc":
            CONSOLE.print("make sure you are in the [red]Developer Command Prompt[/red]")
            CONSOLE.print("if using vscode run code from the Dev Prompt")
            CONSOLE.print("attempting to re-run the build tool in the Dev Prompt...")
            os.system(f"cmd /k \"{DEV_PROMPT}\" & {sys.executable} {' '.join(sys.argv)}")
        sys.exit(1)


    if return_code != b'' and not return_code.endswith(b"\r\n\r\n"):
        CONSOLE.print(f"failed to link all files")
        CONSOLE.print(f"'{LINK_COMMANDS[COMPILER][DIRECTIVE][0]}' failed with: ", end="")
        CONSOLE.print(pipe.decode("utf-8"))
        CONSOLE.print(f"got return code: {return_code}")
        return

    link_time[1] = time.time()
    return (link_time[1] - link_time[0])

def display_compiler_files():
    CONSOLE.print(Panel(f"Ze7111 (R) Dhruvan's custom build tool V0.4 for {sys.platform}\nCopyright {time.strftime('%Y')} (C) Apache License, Version 2.0."))

    table: Table = Table(highlight=True)
    table.add_column("compiling > files")

    for file in files:
        table.add_row(file)

    CONSOLE.print(table)

def create_dirs():
    dir1 = (
          os.getcwd()
        + PATH_SEP
        + "build"
        + PATH_SEP
        + DIRECTIVE
        + PATH_SEP
        + "generated"
    )
    dir2 = (
          os.getcwd()
        + PATH_SEP
        + "build"
        + PATH_SEP
        + DIRECTIVE
        + PATH_SEP
        + "bin"
    )
    if not os.path.exists(dir1): os.makedirs(dir1)
    if not os.path.exists(dir2): os.makedirs(dir2)

def check_compiler():
    if COMPILER not in COMPILE_COMMANDS:
        CONSOLE.print(f"failed to find compiler '{COMPILER}'")

        compilers: list[str] = list(COMPILE_COMMANDS.keys())

        table: Table = Table(highlight=True)
        table.add_column("compilers")
        for compiler in compilers: table.add_row(compiler)

        CONSOLE.print(table)
        sys.exit(1)

shutdowned: bool = False
def show_live_render():
    global live_render

    with live_render as render:
        while shutdowned is False: render.update(generate_table())


def generate_table() -> Table:
    table: Table = Table(highlight=True)
    table.add_column("compiled")
    for file in compiled_files:
        table.add_row(file)
    return table

def fork_compiler(workers: concurrent.futures.ThreadPoolExecutor):
    global live_render, shutdowned
    live_render = Live(generate_table(), refresh_per_second=10)

    CONSOLE.print(f"spawning {workers._max_workers} workers")

    render = threading.Thread(target=show_live_render)
    render.start()

    build_times: list[float] = [time.time(), -1]

    if sys.platform == "win32":
        workers.map(build, files)
        workers.shutdown(wait=True)
    else:
        for file in files:
            build(file)

    build_times[1] = time.time()

    live_render.update(generate_table())

    shutdowned = True
    render.join(0.1)

    return (build_times[1] - build_times[0])

def check_files():
    if len(files) == 0:
        CONSOLE.print("failed to find any files to compile")
        sys.exit(1)

    if len(files) == 1:
        CONSOLE.print("only found one file to compile")
        CONSOLE.print("this is not recommended, add more files to compile")
        CONSOLE.print("to add more files, add them to the src folder")
        CONSOLE.print("or run the build tool from the src folder")

        CONSOLE.print("or use the -r flag to build in release mode")
        CONSOLE.print("or use the -v flag to enable verbose mode")
        CONSOLE.print("or use the -c flag to change the compiler")
        CONSOLE.print("or use the -s flag to change the standard")
        CONSOLE.print("or use the -i flag to add include directories")
        CONSOLE.print("or use the -h flag to display this help message")

    for file in files:
        if not os.path.exists(file):
            CONSOLE.print(f"failed to find {file}")
            sys.exit(1)

def file_hash(filepath):
    import hashlib
    hasher = hashlib.sha256()
    with open(filepath, 'rb') as f:
        buf = f.read()
        hasher.update(buf)
    return hasher.hexdigest()

def identify_changed_files():
    import json
    global changed_files
    state_file = 'build/.changes'

    # Initialize the repo
    repo = git.Repo(search_parent_directories=True)

    # Get all tracked files including staged and unstaged changes
    current_files = {item.a_path for item in repo.index.diff(None)}
    current_files.update({item.a_path for item in repo.index.diff('HEAD')})
    current_files.update(repo.untracked_files)

    # Get the hash of current file contents
    current_files_state = {file: file_hash(os.path.join(repo.working_dir, file)) for file in current_files if os.path.exists(os.path.join(repo.working_dir, file))}

    # Load previous state
    if os.path.exists(state_file):
        with open(state_file, 'r') as file:
            previous_files_state = json.load(file)
    else:
        previous_files_state = {}

    # Compare hashes to determine changes
    changes = {}
    for file, hash in current_files_state.items():
        if file not in previous_files_state or previous_files_state[file] != hash:
            changes[file] = 'modified'
    for file in previous_files_state:
        if file not in current_files_state:
            changes[file] = 'removed'

    # Save current state for next run
    with open(state_file, 'w') as file:
        json.dump(current_files_state, file, indent=4)

    # Update global variable
    changed_files = list(changes.keys())
    CONSOLE.print(f"found {len(changed_files)} changed files")

    return len(changes) > 0

def init():
    init_time[1] = time.time()

    workers: concurrent.futures.ThreadPoolExecutor = concurrent.futures.ThreadPoolExecutor(max_workers=(
                len(files) + 1
            if    len(files) + 1 < (os.cpu_count() * 2)
            else (os.cpu_count() * 2)
        ), initializer=lambda: None
    )

    get_files(os.getcwd() + PATH_SEP + "src")
    create_dirs()

    if not identify_changed_files() and os.path.exists(f"build{PATH_SEP}{DIRECTIVE}{PATH_SEP}bin{PATH_SEP}main{'.exe' if COMPILER == 'msvc' else ''}"):
        CONSOLE.print("no files have changed, skipping build")
        build_times = 0
        link_time = 0
    else:
        check_files()
        check_compiler()
        build_times = fork_compiler(workers)
        link_time = link()

    #os.system("cls" if sys.platform == "win32" else "clear")
    CONSOLE.print(Panel(f"built all files in {build_times:.3f}s\nlinked all files in {link_time:.3f}s", title="build time", highlight=True))

    compiled_path = os.getcwd() + os.sep + "build" + os.sep + DIRECTIVE + os.sep + "bin" + os.sep + "main" + (".exe" if COMPILER == "msvc" else "")

    run_time: list[float, float] = [time.time(), -1]
    if os.path.exists(compiled_path):
        os.system(compiled_path)
    else:
        CONSOLE.print(f"failed to find {compiled_path}")
    run_time[1] = time.time()

    CONSOLE.print(Panel(f"runtime for main is {(run_time[1] - run_time[0]):.3f}s\ntotal time is {(run_time[1] - init_time[1]):.3f}s", border_style="red", highlight=True))

if __name__ == "__main__":
    if not os.path.exists(MAIN_FILE):
        CONSOLE.print(f"failed to find {MAIN_FILE}")
        sys.exit(1)
    init()
    sys.exit(0)
