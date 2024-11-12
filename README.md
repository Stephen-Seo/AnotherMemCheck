# AnotherMemCheck

This software was created as an alternative to valgrind/memcheck.

## How to Build

1. Install cmake.
2. Run `cmake -S <AnotherMemCheck_project_dir> -B buildDebug`.
3. Run `make -C buildDebug`. The output library should be in the buildDebug/
   directory.

If a release build is desired, use `cmake -S <project_dir> -B buildRelease
-DCMAKE_BUILD_TYPE=Release`. (Note that the "build directory" was changed to
"buildRelease" in the preceeding command.)

## Usage

1. Build this software with cmake.
2. Assuming the output library is called `libAnotherMemCheck.so`, run your
   program with the `LD_PRELOAD` environment variable set to this library.

        LD_PRELOAD=path/libAnotherMemCheck.so ./myProgram

3. When the program exits, there should be diagnostic output from this library
   showing all memory that has been `malloc`ed or `calloc`ed and not `free`ed.
   Note that some memory will always be shown as having been not `free`ed (e.g.
   initializing heap stuff for the program at start).

## With gdb

1. Start the debugger
    ```
    gdb ./my_program
    ```

2. Set the env variable to use AnotherMemCheck.
    ```
    set environment LD_PRELOAD ./path/to/libAnotherMemCheck.so
    ```

3. Set debug breakpoints if you want to step through things
    ```
    b my_source_file.c:123
    ```

4. Run it
    ```
    r --my-arg-one --my-arg-two
    ```

AnotherMemCheck should output when memory is handled as the debugger runs, even
when stepping through from breakpoints.

## Other Notes

As mentioned earlier, there probably is memory that is always allocated at the
start of a program that never gets free'd, but this should be ok because the OS
should reclaim all allocated memory from a stopped process.

Also, if one wants to prevent this software from printing every
malloc/calloc/free call, one can set the `ANOTHER_MEMCHECK_QUIET=1` environment
variable so that it only prints unfree'd memory at program execution end.

Since this is relatively newly written software (as of middle of 2024), there
might be a memory leak within this software when keeping track of memory. Thus,
this software may be revised later on. If you are interested in verifying
whether there is a memory leak or not, you may check the source and [submit an
issue/pull-request](https://git.seodisparate.com/stephenseo#contributing) if
there is any discovered issue with the software.
