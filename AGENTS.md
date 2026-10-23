# AI programming assistance guidelines and workflow specifications

This document aims to establish the code of conduct for AI participation in the
writing, modification, and maintenance of this code. AI must strictly adhere to
the following workflow, interaction principles, architectural concepts, and code
standards to ensure the stability, consistency, and maintainability of the
project.

## 1. Core workflow and pre-check

### 1.1 Mandatory pre-check: Karpathy Guidelines

Before carrying out **any** code writing or modification operations, AI **must**
first read the SKILL.md file located in the .skills/karpathy-guidelines/
directory of the project.

- **If reading fails**: Prioritize attempting to install; if installation fails,
do not block the task and instead use the four principles outlined in this
section to execute: (a) **Think before coding**: Clearly state assumptions and
success criteria, and ask questions if unsure; (b) **Simplicity first**: Refuse
excessive abstraction and "just in case" code, and only write the minimum code
required to solve the current problem; (c) **Surgical modification**: Each line
of diff must be traceable to the user's request, and casual optimization of
unrelated code is prohibited; (d) **Goal-driven**: Define verifiable success
criteria first and then iterate. Enable the karpathy skill when it is installed,
and do not use it as a prerequisite for execution if it is not installed. If
installation fails, in addition to following the four principles, ask the user
multiple questions during the modification process to prevent issues with code
modification.

- **If the reading is successful**: The karpathy guidelines skill must be used
to precisely define and determine the scope of this modification. Any
unauthorized modifications beyond this scope are strictly prohibited to prevent
introducing unnecessary errors or disrupting existing logic.

### 1.2 Mandatory preview and confirmation mechanism

**This is an absolute prerequisite for AI to perform any coding tasks.** Before
starting any actual code writing, modification, or file operation, AI **must**:

1. Provide a **summary of text** for the anticipated modifications (including
the involved file names, names of modified functions/modules, and the specific
purposes of the modifications).
2. Only after **explicit confirmation from the user** can actual code
modifications be made.

*Without user confirmation, it is strictly prohibited to directly output the*
*modified complete code or perform write operations.*

### 1.3 Version control and isolation principle

- **Prioritize creating a branch before making modifications**: Before making
any code modifications, AI **must** prioritize suggesting or executing the
creation of an independent Git feature branch (such as feature/xxx or fix/xxx).
- **Security snapshots and quick restores**: It must be ensured that if there
are errors in modifications or deviations in direction, quick and safe restores
can be achieved through branch-level rollbacks or discarding (such as git
checkout -- . or deleting the branch), without polluting the main branch
(main/master) or the current development branch.
- **Small Step Commit Principle**: Make a small commit every time a subtask is
completed within the branch. The commit message must include "what has been
changed + verification evidence". It is **strictly prohibited** to bundle
multiple changes into a single "AI changes" commit.

### 1.4 Complex logic processing and requirement confirmation

- **Principle of minimal modification**: For logically complex code, AI should
adhere to the principle of "prioritizing functionality and minimizing
modifications".
- **Self-checking priority principle**: Before repeatedly asking the user, the
AI must first attempt to infer on its own through methods such as git blame,
reading the caller's code, or querying type definitions in header files. After
inference, it must clearly label the assumptions and only ask the user if it
remains uncertain (following the "grill-me" self-checking priority).
- **Grill-me mechanism**: When in-depth requirement confirmation is needed, the
AI must read the SKILL.md file located in .skills/grill-me/ under the project
directory (.skills), and invoke the Skill tool by passing in "grilling". Through
relentless questioning, the plan and design are refined to ensure absolute
consensus is reached before implementation.

### 1.5 Context and magic number handling

- When encountering magic numbers (hard-coded constants) in code, AI must fully
understand their contextual meaning.
- If the business background or potential impact of the magic number cannot be
determined, it is **strictly prohibited** to modify or extract it without
authorization. Repeated inquiries must be made to the user to confirm their true
intention.

### 1.6 Architectural decision record

- **Clear file location**: All architecture decision records must be uniformly
written into the DECISIONS.md file located in the **root directory** of the
project.
- **Sorting rules**: Records must be arranged strictly in the order of
"**Date + Commit in descending order**" (i.e., the latest record should be
placed at the top).
- **Standard format**: The title of each record must strictly follow the
following format: ## YYYY-MM-DD Reason/Trade-off/Risk
- **Immediate appending principle**: After modifying the business code, AI
**must immediately append** the decision record, and
**absolutely no retroactive appending is allowed**.

## 2. Architectural philosophy and code design principles

When generating or refactoring code, despite it being a C language project, AI
must adhere to the design philosophy and engineering best practices of the Go
language. It must strictly follow the core principles of
**low coupling and high cohesion**, while utilizing the `internal/` directory to
achieve internal encapsulation and boundary isolation of modules. The following
core principles should be followed:

### 2.0 Directory structure and internal encapsulation convention

This project employs the `internal/` directory to store code exclusively for
internal use within the project, thereby achieving module encapsulation and
boundary isolation. Since C language lacks a package-level visibility control
mechanism (unlike Go's unexported identifiers), it is imperative to safeguard
internal code from external misuse through the following **triple means**:

- **Directory Convention**: All source files (.c) and their corresponding
private header files (.h) that serve only for internal implementation within
this project must be uniformly placed in the `internal/` directory or its
subdirectories. Code within `internal/` must not be directly `#included` or
linked by any external consumers of the project.
- **Build system constraints**: In build configurations (such as Makefile,
CMakeLists.txt), the path of header files under the `internal/` directory must
not be exposed to the include search path of external targets (such as the
PUBLIC/INTERFACE scope of `target_include_directories`), and is only allowed to
be referenced by internal targets of the project in a PRIVATE manner.
- **Symbol Visibility Control**: All functions within `internal/` that are not
intended to be exposed externally must be designated with the `static` keyword
to restrict their linkage scope to the current translation unit. For functions
that must be shared across multiple translation units within `internal/` but are
not to be exposed externally, they must be declared through a private header
file located under `internal/`, ensuring that this private header file is not
installed into the public include path.
- **Naming Convention**: Public functions within `internal/` (i.e., functions
shared across internal modules) must use a project-level prefix (such as
`proj_internal_xxx`) to explicitly identify their internal nature in the global
symbol table, thereby reducing the risk of symbol conflicts with external
libraries.

### 2.0A musl libc priority principle

This project prioritizes the development using **musl libc** as the target C
standard library, rather than glibc. musl strictly adheres to ISO C and POSIX
standards and does not provide private extensions of g libc. When generating or
modifying code, AI must adhere to the following constraints:

#### Prohibited proprietary extensions of glibc

The following functions, macros, and header files are private extensions of
glibc, and musl does not provide them or their behavior may be inconsistent. It
is **strictly prohibited** to use them in project code:

- **GNU extension functions**: `strdupa()`, `asprintf()`, `vasprintf()`,
`qsort_r()`, `getpt()`, `error()` / `error_at_line()` (from `<error.h>`),
`fts_open()` / `fts_read()` / `fts_close()` (fts function family), `obstack`
series functions, `argp` series functions
- **GNU extension headers**: `<error.h>`, `<obstack.h>`, `<argp.h>`, `<fts.h>`,
`<sys/cdefs.h>` (`__BEGIN_DECLS` / `__END_DECLS` etc.)
- **BSD compatibility macros**: `MAXNAMLEN` (should be replaced with `NAME_MAX`
from the POSIX standard), `MSG_TRYHARD`
- **GNU printf format specifier**: such as `%Ld` (a non-standard alias for
`long long`), musl will report an error; strictly use C standard format
specifiers (such as `%lld`)
- **Standard stream reassignment**: In musl, `stdout`, `stdin`, and `stderr` are
read-only expressions, and the syntax `stdout = fopen(...)` is prohibited;
instead, you must use `freopen("file", "w", stdout)`

#### Explicit inclusion of header files

musl does not implicitly expose symbols through indirect inclusion. Each `.c`
file must explicitly `#include` **all** the header files it directly uses, and
it is strictly prohibited to rely on the indirect inclusion chain of glibc (for
example, if `strcmp()` is used, it is necessary to explicitly
`#include <string.h>`, even if other header files happen to already include it).

#### Considerations for thread stack size

The default thread stack size of musl is significantly smaller than that of
glibc (glibc's default is about 8MB, while musl's default is much smaller). For
threads that require deep recursion or large local arrays, it is necessary to
explicitly set a sufficient stack size through `pthread_attr_setstacksize()`,
otherwise stack overflow crashes may occur.

#### Feature detection, not platform macros

The use of `#ifdef __GLIBC__` for conditional compilation is prohibited.
Standard feature detection macros (such as `_POSIX_C_SOURCE`, `_XOPEN_SOURCE`)
or the feature checks provided by the build system (such as CMake's
`check_function_exists`) should be employed to determine the availability of a
feature.

### 2.1 Explicit is better than implicit

- **Refuse "magic" and excessive abstraction**: Avoid using macro black magic,
implicit linker symbol injection, or indirect calls that are difficult to trace
(such as excessive reliance on function pointer tables without documentation).
The execution logic, data flow, and dependency relationships of the code must be
clearly visible.
- **Explicit Dependency Declaration**: Module dependencies should be explicitly
declared through `#include` in header files, and function dependencies should be
explicitly passed in through parameters (such as passing context through a
struct pointer, rather than relying on global variables). Each `.c` file must
`#include` all its direct dependencies, and indirect inclusion (include
chaining) is strictly prohibited.
- **Explicit error handling**: It is prohibited to swallow error return values.
The return values of all potentially failing function calls (such as memory
allocation, file IO, system calls) must be explicitly checked and handled.
Follow the principle of "who allocates, who releases" to ensure clear and
controllable resource lifecycle.
- **Self-contained header files**: Since musl does not guarantee indirect
inclusion, each public header file must be self-contained, meaning that simply
`#include`ing the header file is sufficient for compilation to pass, without
relying on the caller to include other header files beforehand.

### 2.2 Modularity and Single Responsibility

- **Module Isolation**: Adhere to the principle of "high cohesion, low
coupling". Each `.c` file and its corresponding `.h` header file constitute a
translation unit module, which should focus solely on a specific business
domain or function, avoiding the confusion of business logic with IO
operations, platform adaptation, etc.
- **Composition over deep inheritance**: Prefer to use composition of structures
and function pointer tables (vtable pattern) to achieve code reuse and
polymorphic behavior. Strictly prohibit the abuse of deep nested structure
inheritance simulation (such as "base class" nesting exceeding 3 levels).
- **Explicit module boundaries**: Strictly isolate module boundaries through the
public declaration in header files (`.h`) and the internal implementation in
source files (`.c`). Only expose necessary type definitions, macros, and
function declarations in header files; all internal functions that are not
exposed externally must be qualified with the `static` keyword to have file
scope. Direct cross-module referencing of non-`static` internal implementations
in other `.c` files is prohibited. Communication between modules must be
conducted through interfaces exposed in public header files.
- **Ownership of the internal directory**: Modules that serve only for internal
implementation of this project and should not be relied upon by external
consumers must be placed in the `internal/` directory; publicly exposed API
modules are retained in the public directory (such as `src/` or `lib/`), and
their header files are exposed through the public include path.

### 2.3 In terms of ensuring speed, parallelism outperforms serial execution

- **Tasks without dependencies must be executed in parallel**: When handling
multiple independent tasks without data dependencies between them (such as batch
file IO, multiple network requests, and independent data parsing),
**it is mandatory** to prioritize parallel execution (such as POSIX threads
(pthread), thread pools, or asynchronous IO mechanisms like epoll/io_uring). In
this scenario, **serial execution is strictly prohibited**.
- **Serial processing allowed for dependent tasks**: If there are input
dependencies, sequence constraints, or state transition relationships (such as
establishing a connection before sending data) between tasks, serial processing
is allowed and should be used to ensure logical correctness.
- **Concurrent Control**: When involving shared state changes, it is necessary
to design a reasonable synchronization mechanism (such as mutex, read-write lock
rwlock, atomic operation atomic, semaphore) to prevent data race and reentrancy
issues. This ensures thread safety and system stability while improving speed.
- **Explicit declaration of thread stack size**: Since the default thread stack
size of musl libc is significantly smaller than that of glibc (glibc has a
default size of approximately 8MB, while musl has a significantly smaller
default size), for threads that require deep recursion or large local arrays, it
is **mandatory** to explicitly set a sufficient stack size through
`pthread_attr_setstacksize()`. Relying on the default value is strictly
prohibited.
- **Multithreading is superior to single-threading**: When dealing with tasks
that require simultaneous processing, multithreading should be preferred over
single-threading.

### 2.4 Separation of log output and user prompts

- **Program logs (stderr/stdout)**: All underlying logs for developers or
operators (such as debug information, error messages, and warning messages
output through ` fprintf(stderr, ...)`, `syslog()`, or custom logging functions)
**must be in English**.
  - *Purpose*: To prevent messy code output in logs due to inconsistent terminal
  encoding, while facilitating global log retrieval, error tracking, and
  integration with international log analysis platforms.
- **User Interface Prompts**: All visual feedback for end users (such as TUI
interface prompts, terminal interactive output, and error pop-up text)
**must be dynamically switched according to the current locale**, for example,
using the corresponding json files in the lang directory to achieve multilingual
translation.
- **Log Prefix and Format Specification**: To facilitate subsequent debugging,
the program's log output must strictly adhere to the following prefix and format
requirements:
  - When outputting panic logs, the prefix must be `"[PANIC] "`;
  - When outputting error logs, the prefix must be `"[ERROR] "`;
  - When outputting warning logs, the prefix must be `"[WARN] "`;
  - When outputting regular logs, the prefix must be `"[INFO] "`;
  - When outputting debug logs, the prefix must be `"[DEBUG] "`;
  - **Mandatory format requirement**: There **must be a space** between the
  aforementioned prefix and the specific log content (e.g.:
  ` fprintf(stderr, "[ERROR] Failed to open config file: %s\n", path); `).
  - **Stack Trace Output Requirement**: For ease of debugging, the error log
  **must** output the complete stack trace.
  - **Logging level requirements**: The output level must be allowed to be
  modified in the configuration (config.json) or through command settings, to
  facilitate debugging.
  - **Log Output Location**: The location for log output should be the file
  specified in config.json/user settings. Output to the user terminal is
  prohibited to prevent cluttering of the user terminal content.

## 3. Maintenance specifications for existing code

### 3.1 Abide by the principle of minimal modification

- **Local norm application**: When modifying existing legacy code, apply this
specification only to the local code directly affected by the current change.
- **Prohibition of Global Refactoring**: For existing historical code, AI is
**strictly prohibited** from actively performing global refactoring or
large-scale overhaul. The principle of minimal modification must be adhered to,
ensuring the absolute stability of historical business logic.

## 4. Specification for symbol isolation and namespace conflict prevention

To prevent global symbol pollution and naming conflicts between modules, AI must
strictly adhere to the following isolation principles when writing C code:

### 4.1 Strict symbol namespace isolation

- **Prevent global symbol conflicts**: In source files of different modules, it
is **strictly prohibited** to use the same global function names, global
variable names, or macro names (such as generic names like `init()`, `parse()`,
`MAX_SIZE`, etc.) as those in public API header files or other modules.
- **Module-level Naming Prefix**: All public symbols (functions, global
variables, macros, type definitions) within each module must bear a distinct
module-level prefix (e.g., `config_parse()`, `config_get_value()`,
`CONFIG_MAX_SIZE`; `net_connect()`, `net_send()`) to ensure that symbols are
exclusively attributed to the current module, thereby eliminating any potential
link conflicts or semantic ambiguities.
- **Static internal symbols**: Auxiliary functions and file-scope variables used
within a module must be declared with the `static` keyword to prevent them from
entering the global symbol table, thereby fundamentally eliminating accidental
cross-module references.
- **Header File Protection**: All header files must utilize include guards
(`#ifndef / #define / #endif` or `#pragma once`) to prevent compilation errors
caused by repeated inclusion.
- **Avoid conflicts with musl internal symbols**: The global symbol table of
musl is more concise than that of glibc, but there is still a risk of conflicts
between project internal symbols and libc symbols. All project global symbols
must use a project-level prefix (such as `proj_`), and it is strictly prohibited
to use identifiers that start with an underscore followed by a capital letter or
double underscores (these are reserved for the C standard and libc).

## 5. Debugging and exception handling mechanism

### 5.1 The principle of prioritizing compilation over runtime

- After code modification or generation, AI must prioritize ensuring that the
code can be compiled without any warnings (with zero warnings as the goal), and
guide users to check the runtime output.
- **Compilation Errors:** Fixing compilation errors must be treated as a top
priority task, and no new feature development should proceed until these
errors are resolved.
- **Compilation Warnings:** AI should suspend operations and ask the user
whether these warnings need to be fixed. It is strictly prohibited to modify the
code without authorization to eliminate warnings.
- **Error-free verification process**: When there are no compilation errors, the
AI must first output a verification evidence list (such as: unit test X is green
/ build passes / static analysis with 0 warnings / no memory errors detected by
Valgrind or AddressSanitizer / this diff involves only M files out of N lines),
and then ask the user whether the functionality meets expectations. If the
project targets musl as the libc, it must also confirm that there are no
compilation errors under the musl toolchain.

### 5.2 Global error handling

- When it comes to program initialization or core logic, one should refer to
existing global error handling mechanisms (such as using `setjmp/longjmp` for
exception jumping, a unified error code system, or signal handling handlers) to
ensure graceful degradation, release of allocated resources, or provision of
clear error feedback in the event of unforeseen errors.
- **Resource cleanup**: All error paths must ensure that allocated resources
(memory, file descriptors, locks, etc.) are properly released to prevent
resource leaks. It is recommended to adopt a unified `cleanup` or `goto err`
pattern for centralized cleanup.

## 6. Interaction and authority boundary

### 6.1 Prioritize inquiry and prohibit exceeding authority

- **Core Advocacy**: When writing any code, AI must prioritize "inquiring the
user".
- **No Haphazard Modifications**: It is absolutely prohibited for AI to
haphazardly modify any non-target code (including but not limited to:
refactoring unrelated functions, modifying indentation, changing variable names,
optimizing unrelated logic, etc.) without explicit user permission.

## 7. Establish system and dependency management specifications

### 7.1 Establish configuration specifications

- **Build system consistency**: The project should use a unified build system
(such as Makefile, CMake, Meson, etc.). When AI adds new source files or header
files, the build configuration file must be updated simultaneously to ensure
that the new files are correctly incorporated into the compilation process.
- **Zero tolerance for compilation warnings**: Common warning options (such as
`-Wall -Wextra -Wpedantic`) must be enabled in the build configuration, with the
goal of developing with zero warnings as the benchmark.

### 7.2 Dependency management principle

- **Principle of minimal dependency**: Prioritize the use of functions provided
by the C standard library (libc), and only introduce third-party libraries when
the standard library cannot meet the requirements.
- **Explicit dependency declaration**: All external dependencies must be
explicitly declared in the build configuration file (such as `find_package` or
`FetchContent` in CMake, or link flags in Makefile), and implicit dependency on
system-specific libraries is prohibited.
- **Header file inclusion order**: It is recommended to organize the `#include`
statements in the following order: (1) the header file of the corresponding
module itself; (2) the C standard library header files; (3) the header files of
third-party libraries; (4) the header files of other modules within the project.
Separate each group with a blank line.

### 7.3 musl libc build and linking specifications

- **Static Linking Preferred**: musl is naturally suited for static linking (MIT
license without LGPL constraints). In embedded or container deployment
scenarios, it is recommended to use `-static` for full static compilation,
resulting in a single-file executable program with no external dependencies.
- **Building Environment Consistency**: The compilation toolchain for CI/CD and
the development environment must be consistent with the target deployment
environment. If the target environment uses musl (such as Alpine Linux), the
musl toolchain (such as `musl-gcc` or `x86_64-linux-musl-gcc`) must also be used
for compilation and testing during the development stage. It is strictly
prohibited to develop in a glibc environment and then deploy directly to a musl
environment.
- **Compilation Warning Option**: When compiling in the musl environment, it is
recommended to additionally enable `-D_POSIX_C_SOURCE=200809L` to ensure that
only POSIX standard interfaces are used, thereby avoiding the accidental
introduction of platform-specific extensions.
- **Linker Script and Symbol Versioning**: musl does not utilize the symbol
versioning mechanism of glibc, eliminating issues related to `GLIBC_2.xx` symbol
dependencies. If a project requires compatibility with both g libc and musl, it
is prohibited to rely on symbol versioning features in the code.

## 8. Cross-platform development specifications

The project must be designed from the outset following the principle of
"platform-independence prioritized, platform-specific isolation maintained". It
is prohibited to scatter if windows / else linux type of judgments within the
business logic. All platform-specific capabilities, including process calls,
signal handling, file locks, permissions, service registration, clipboard,
system tray, network interface enumeration, etc., must be converged into an
independent platform adaptation layer, exposing a unified interface externally,
with the corresponding implementation selected by the factory or build tag,
ensuring that the core business code is unaware of the operating system. Paths
must be concatenated using the path library provided by the language, strictly
prohibiting the hardcoding of backslashes or fixed absolute paths. Configuration
files and log paths should be dynamically parsed based on the directory where
the program resides or the user directory. File names must be strictly
case-consistent, as Linux distinguishes between uppercase and lowercase, while
Windows typically does not. References to file names, module names, and package
names in the code must fully match the actual ones; otherwise, Windows may run
directly on Linux and report errors. Text reading and writing should explicitly
specify the encoding as UTF-8, and handle differences in line breaks properly,
using the newline parameter for control when necessary. Additionally, configure
.gitattributes in the repository to unify CRLF and LF, avoiding diff and parsing
anomalies. Do not rely on shell commands, especially do not directly call bash,
grep, sed, which, chmod, etc. When executing external programs, use parameter
arrays instead of concatenating strings, considering the situation where
commands do not exist on Windows or require the .exe suffix. Environment
variables, proxies, and temporary directories should be obtained through
standard APIs, without assuming the existence of /tmp, /proc/cpuinfo, or certain
Linux-specific files. For building, use cross-platform build systems such as
CMake, Go's GOOS/GOARCH, or uniformly use Docker and CI matrix to compile and
run tests on both Linux and Windows simultaneously, exposing differences as
early as possible. If it is a Go project, prefer to generate pure static
binaries with CGO_ENABLED=0. Once CGO is introduced, it will depend on the host
libc, and cross-compilation requires additional toolchains. If static
compilation is necessary, consider using musl in conjunction with external
static linking. C/C++ projects should rely on standard libraries and
cross-platform libraries as much as possible, avoiding direct calls to WinAPI or
POSIX-specific interfaces. Pay attention to ABI, byte order, and structure
alignment differences, and perform explicit conversions when necessary. Finally,
do not only verify on a system you commonly use. WSL can only be considered an
approximate environment, and Windows path appending, executable file
compatibility, and kernel behavior may differ from real Linux. Critical
functions must be tested on both real or containerized platforms through smoke
testing. For systems compatible with musl libc, cross-platform compatibility
only requires compatibility with musl, and there is no need to ensure
compatibility with glibc.

## 9. Code comment standard

To unify the visual experience of the code repository and enhance code
readability, all code comments generated by AI must strictly adhere to the
following format and quality requirements:

### 9.1 Format specification

- **Language**: Use **English** for annotation and explanation.
- **Hard Tab Specification**: The presence of any Hard Tab characters in the
code is **prohibited** as they compromise readability. Please replace Hard Tabs
with Soft Tabs.
- **Suckless style**: The code should adhere to the Suckless principles of
`simplicity, clarity, minimalization, and usability`, and the addition of
unexpected or redundant functionality and code is prohibited.

### 9.2 Quality requirements: detailed and unambiguous

- **Strictly prohibit vague annotations**: Annotations must be detailed and
accurate, clearly explaining the business logic of the code, the meaning of
parameters, the return values, or the execution steps of complex algorithms.
- **No nonsense**: It is strictly prohibited to use vague descriptions devoid of
information, such as "processing data", "performing operations", "initializing
variables", etc.
- **Header file interface annotation**: All function declarations in public
header files must be accompanied by annotations, specifying the function
purpose, parameter meanings, return value semantics, and possible error codes.
- **Detailed example**:

```c
// Create the main configuration parser for the application, read the
// configuration file from the specified path, and populate it into the config
// struct
// Successfully returned 0. If the file does not exist, return
// CONFIG_ERR_FILE_NOT_FOUND. If the format is incorrect, return
// CONFIG_ERR_PARSE
int config_load(config_t *config, const char *filepath);
```

- **musl compatibility note**: When there are coding practices in the code that
deviate from glibc behavior (such as using `freopen` instead of directly
assigning `stdout`, or using `NAME_MAX` instead of `MAXNAMLEN`), a comment must
be added to explain the reason, in the following format:

```c
// [musl-compat] Use freopen instead of stdout = fopen(), because stdout is
// read-only in musl
freopen("output.log", "w", stdout);

**AI Confirmation Receipt**: Before commencing any coding task, the AI should
confirm in its internal thoughts or responses that it has read and comprehended
this guide, and strictly adhere to the aforementioned process.