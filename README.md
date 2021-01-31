# Console++

[![Documentation](https://img.shields.io/badge/code-documented-brightgreen.svg?style=flat)](https://kazdragon.github.io/consolepp/) 
[![License](https://img.shields.io/github/license/KazDragon/consolepp.svg)](https://en.wikipedia.org/wiki/MIT_License) 
[![GitHub Releases](https://img.shields.io/github/release/KazDragon/consolepp.svg)](https://github.com/KazDragon/consolepp/releases) 
[![Linux Build status](https://github.com/KazDragon/consolepp/workflows/build/badge.svg)](https://github.com/KazDragon/consolepp/actions?query=event%3Apush) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/63ec54845f2c41f1899706c61f1c316b)](https://www.codacy.com/app/KazDragon/consolepp?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=KazDragon/consolepp&amp;utm_campaign=Badge_Grade)
[![Download](https://api.bintray.com/packages/kazdragon/conan-public/consolepp%3Akazdragon/images/download.svg)](https://bintray.com/kazdragon/conan-public/consolepp%3Akazdragon/_latestVersion)

Console++ is a small framework for interacting with the console on a character-at-a-time basis.  Used together with Terminal++ and Munin, this
library can form the foundation of any graphical terminal application.

# Requirements

Console++ requires a C++14 compiler and the following libraries:
  * Boost (At least version 1.69.0)
  * GSL-lite (Exactly version 1.34 until 1.38 is available, due to [This issue](https://github.com/gsl-lite/gsl-lite/issues/270)
  * (For testing only) Google Test

# Installation - CMake

Console++ can be installed from source using CMake.  This requires Boost, GSL-Lite and any other dependencies to have been installed beforehand, using their own instructions, or for the call to `cmake --configure` to be adjusted appropriately (e.g. `-DBOOST_ROOT=...` or `-Dgsl-lite_DIR=...`).  If you do not wish to install into a system directory, and thus avoid the use of sudo, you can also pass `-DCMAKE_INSTALL_PREFIX=...` into the `cmake --configure` call.

    git clone https://github.com/KazDragon/consolepp.git && cd consolepp
    mkdir build && cd build
    cmake --configure -DCMAKE_BUILD_TYPE=Release ..
    cmake --build .
    sudo cmake --install .

# Installation - Conan

You can also use [The Conan Package Manager](https://conan.io/) to install Telnet++ and its dependencies.

# Features / Roadmap / Progress

1. [x] Basic console handling
  * [x] Reading/writing from/to the console in character mode
  * [x] Queries and callbacks for console size

# Status

Console++ is automatically testing with GNU g++ 7.5.
