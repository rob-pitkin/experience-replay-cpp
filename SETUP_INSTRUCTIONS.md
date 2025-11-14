# Phase 1.1: Project Setup Instructions

Welcome! This guide will walk you through setting up a modern C++ project. You'll learn CMake basics and Google C++ tooling.

## Step 1: Understanding the Directory Structure

You now have this structure:
```
experience-replay-cpp/
├── include/replay_buffer/   # Public header files (your API)
├── src/                      # Implementation files (.cpp)
├── tests/                    # Unit tests
├── benchmarks/               # Performance benchmarks
├── cmake/                    # CMake helper scripts
├── build/                    # Build output (git-ignored)
└── docs/                     # Documentation
```

**Why this structure?**
- `include/` - Headers are separate so users can `#include <replay_buffer/foo.h>`
- `src/` - Implementation details hidden from users
- Separation allows header-only or compiled library options

## Step 2: Create CMakeLists.txt (Root)

Create `CMakeLists.txt` in the project root. This is your build system configuration.

### TODO 1: Basic Project Declaration
```cmake
# Require modern CMake (Chrome uses even newer, but 3.20 is widely available)
cmake_minimum_required(VERSION 3.20)

# TODO: Declare your project name and version
# Hint: project(ProjectName VERSION x.y.z LANGUAGES CXX)
# Use "replay_buffer" as the name, version 0.1.0

# TODO: Set C++ standard to C++20
# Hint: set(CMAKE_CXX_STANDARD 20)
# Also set CMAKE_CXX_STANDARD_REQUIRED to ON

```

### TODO 2: Compiler Flags
Add these important flags for safety and Chrome-like strictness:

```cmake
# TODO: Add compiler warnings
# For Clang/GCC, add these flags to CMAKE_CXX_FLAGS:
# -Wall -Wextra -Wpedantic -Werror
# Hint: Use add_compile_options(-Wall -Wextra ...)
# -Werror treats warnings as errors (good practice!)

# TODO: Set optimization levels based on build type
# CMAKE_BUILD_TYPE can be Debug or Release
# For Debug: -g (debug symbols), -O0 (no optimization)
# For Release: -O3 (max optimization), -DNDEBUG (disable asserts)
# Hint: Use target_compile_options with generator expressions
# Or set CMAKE_CXX_FLAGS_DEBUG and CMAKE_CXX_FLAGS_RELEASE

```

### TODO 3: Include Directories
```cmake
# TODO: Make include/ directory available for #include statements
# Hint: include_directories(include)
# Modern CMake prefers target_include_directories, but we'll use this for now

```

### TODO 4: Add Subdirectories
```cmake
# TODO: Tell CMake to process subdirectories
# For now, just add tests (we'll add src later when we have .cpp files)
# Hint: add_subdirectory(tests)
# Comment this out until we create tests/CMakeLists.txt

```

## Step 3: Configure Google C++ Style Tools

### TODO 5: Create .clang-format
Create `.clang-format` in the project root. This auto-formats your code to Google style.

```yaml
# TODO: Set the base style to Google
# Hint: BasedOnStyle: Google
BasedOnStyle: Google

# TODO: Set column limit (Google uses 80)
ColumnLimit: 80

# TODO: Choose IndentWidth (Google uses 2 spaces)
IndentWidth: 2

# Additional useful settings:
Language: Cpp
Standard: c++20
```

**To use:** Run `clang-format -i <filename>` to format a file in-place.

### TODO 6: Create .clang-tidy
Create `.clang-tidy` for static analysis (finds bugs before runtime!).

```yaml
# TODO: Add these check categories
# - modernize-*  (suggests modern C++ features)
# - performance-* (finds performance issues)
# - readability-* (improves code clarity)
# - bugprone-*   (catches common bugs)
#
# Format: Checks: 'check1,check2,check3'
Checks: 'modernize-*,performance-*,readability-*,bugprone-*'

# Optional: Customize warnings
# WarningsAsErrors: '*'
```

## Step 4: Create .gitignore

### TODO 7: Add build artifacts to .gitignore
Create/update `.gitignore`:
```
# Build directories
build/
cmake-build-*/

# IDE files
.vscode/
.idea/
*.swp

# Compiled files
*.o
*.a
*.so
*.dylib

# OS files
.DS_Store
```

## Step 5: Verify Your Setup

Once you've completed the TODOs above:

```bash
# Create build directory
cd build

# Configure (generate build files)
cmake ..

# What to expect:
# - CMake should detect your compiler (clang++)
# - Should configure successfully
# - May warn about missing subdirectories (that's OK for now)

# Build (nothing to build yet, but this verifies configuration)
cmake --build .
```

## Key Concepts to Understand

1. **CMake is a build system generator** - it creates Makefiles or Ninja files
2. **Out-of-source builds** - we build in `build/` to keep source clean
3. **Targets** - things to build (libraries, executables, tests)
4. **Modern CMake** - prefers target-specific properties over global settings

## Next Steps

After completing this setup:
1. Run `cmake ..` from the build directory
2. Ask Claude to review your CMakeLists.txt
3. Move on to Phase 1.2 (GoogleTest integration)

## Questions to Consider

- Why do we separate include/ and src/?
- What's the difference between Debug and Release builds?
- Why use -Werror (warnings as errors)?
- How does CMake find your compiler?

Feel free to ask Claude about any of these concepts!
