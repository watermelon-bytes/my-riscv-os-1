# Coding Convention

## General
- We do not aim to support other architectures than RISC-V yet; so RISC-V specific code doesn't have to be strictly separated from more abstract, architecture-independent code. However, since it's a good practice, it is a recommended thing to do.

## Naming
- All `camel_case`; only macros use `CAPS_LOCK_CASE` to be different.

## Formatting
- Use `clang-format`.
- The project provides a `.clang-format` file; do not override it.
- If your editor does not format automatically:
```sh
git add .
git clang-format --staged
```
 before commit.

## Modules
- Obviously, we avoid heavy external dependencies where possible. Designed specifically for embedded use, `libfdt` is an exception here.
- TODO: consider using git submodules for `libfdt` instead of copied source files 

## Error Handling
- Return error code as return value (e.g. `int`) where possible. `enum`s are excellent here.
- Use `bool` only when the function has exactly two meaningful outcomes (e.g. found/not found, valid/invalid, enabled/disabled) and no additional error information is needed.
- Use `_Static_assert` heavily. Run-time `assert` is also great for debugging, often better than tons of `prinf`'s.

### Rules

**Rule:** Only `kmain()` and functions directly called by `kmain()` may decide to halt the current hart. Lower-level code must report errors to its caller instead.

**Reason:** Keep policy decisions at the top level and make lower-level code reusable and testable.

