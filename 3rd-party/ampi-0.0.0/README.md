Asynchronous MessagePack Implementation (AMPI)
==============================================

This is a temporary repository due to issues with private commit history.

Dependencies:
- LLVM/Clang 12 (unreleased).
  - `patches/llvm*` contains patches to version 11.0.1 to allow compilation to succeed.
    - libc++ patch is header only and can be applied to installed files without recompiling libc++.
- Boost 1.76 (unreleased).
  - `patches/boost*` contain patches to version 1.75 to successfully build the library. All of these patches are header-only and can be applied to installed files without recompiling boost.
- Boost.UT is required for testing, see https://github.com/boost-ext/ut.
- ntc-cmake from top of master branch.
