# Console++

[![Documentation](https://img.shields.io/badge/code-documented-brightgreen.svg?style=flat)](https://kazdragon.github.io/consolepp/) 
[![License](https://img.shields.io/github/license/KazDragon/consolepp.svg)](https://en.wikipedia.org/wiki/MIT_License) 
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/consolepp.svg)](https://github.com/KazDragon/consolepp/releases) 
[![Linux Build status](https://github.com/KazDragon/consolepp/workflows/build/badge.svg)](https://github.com/KazDragon/consolepp/actions?query=event%3Apush) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/63ec54845f2c41f1899706c61f1c316b)](https://www.codacy.com/app/KazDragon/consolepp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/consolepp&amp;utm_campaign=Badge_Grade)

Console++ is a small framework for interacting with the console on a character-at-a-time basis.  Used together with Terminal++ and Munin, this
library can form the foundation of any graphical terminal application.

## Requirements

- C++20 compiler
- CMake 3.16+
- Boost 1.69+ (required)
- Google Test (for tests only)

## Build And Install (From Source)

```bash
git clone https://github.com/KazDragon/consolepp.git
cd consolepp

cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$HOME/.local"

cmake --build build --config Release
cmake --install build --config Release
```

## Dependency Resolution With vcpkg

Console++ can resolve dependencies automatically through `vcpkg`:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

When using manifest mode (`vcpkg.json` in this repository), configure will
trigger dependency installation automatically.

## Automated Releases

Pushes to `master` that come from a merged pull request and pass the `build`
workflow automatically publish a GitHub Release. The release version is chosen
using the following precedence:

- pull request labels `semver:major`, `semver:minor`, `semver:patch`
- Conventional Commit cues from the PR title or commit subjects
- contextual hints in the PR title/body such as `breaking` or `feature`
- `patch` when nothing else matches

Downstream consumers continue to get the standard GitHub release experience,
including the automatically generated source archives for each tag.

## Consume From CMake (Installed Package)

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES CXX)

find_package(consolepp CONFIG REQUIRED)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE KazDragon::consolepp)
```

If installed to a non-system prefix:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$HOME/.local"
```

## Features / Roadmap / Progress

1. [x] Basic console handling
    * [x] Reading/writing from/to the console in character mode
    * [x] Queries and callbacks for console size

## Status

Console++ is automatically testing with GNU g++ 7.5.
