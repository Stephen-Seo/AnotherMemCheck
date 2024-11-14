# Changelog

## Upcoming Changes

Add counts for bad `realloc`s and `free`s that are printed at program's end.

(A bad `realloc` or `free` is an attempted `realloc` or `free` of a memory
location that wasn't `malloc`d, `calloc`d, or `realloc`d.)

## Version 2.10

Internal refactorings.

Remove dependency on `pthread` in CMakeLists.txt.

## Version 2.9

Add checking/stat-printing of when NULL pointers are free'd.

Add note in README.md to use this library with a debugger.

## Version 2.8

Internal refactorings.

Minor change to avoid segfault when program exits with `exit(...)` function.

## Version 2.7

Fix incorrect initialization.

## Version 2.6

Use a more robust mutex (by using `std::recursive_mutex` instead of
`pthread_mutex`).

Minor refactorings.

## Version 2.5

Added support for realloc().

## Version 2.4

Fixed free() not actually calling real_free().

Fixed nullptr dereference during free.

## Version 2.3

Added ids to each chunk of allocated memory to make it easier to keep track of
which allocated memory refers to which unfreed memory.

## Version 2.2

Minor refactoring: remove unused constexpr declarations in header.

Change implementation to not print when malloc/calloc/free is called if the
"ANOTHER_MEMCHECK_QUIET" environment variable is defined.

## Verison 2.1

Previous implementation did not fully account for total size of a call to
calloc. This version should fix this.

## Version 2.0

Minor refactorings.

Change internal implementation of linked list to be doubly-linked.

Add support for calloc.

Add pthreads to mutex-lock malloc/calloc/free.

## Version 1.0

Initial version with working `AnotherMemCheck`.
