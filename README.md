# AnotherMemCheck

This software was created as an alternative to valgrind/memcheck.

## Usage

1. Build this software with cmake.
2. Assuming the output library is called `libAnotherMemCheck.so`, run your
   program with the `LD_PRELOAD` environment variable set to this library.

        LD_PRELOAD=path/libAnotherMemCheck.so ./myProgram

3. When the program exits, there should be diagnostic output from this library
   showing all memory that has been `malloc`ed and not `free`ed. Note that some
   memory will always be shown as having been not `free`ed (e.g. initializing
   heap stuff for the program at start).
