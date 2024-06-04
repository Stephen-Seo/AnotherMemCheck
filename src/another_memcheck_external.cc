// Standard library includes.
#include <iostream>
/*#include <cstdlib>*/
/*#include <cstdio>*/

// Unix includes.
#include <dlfcn.h>

// Local includes.
#include "another_memcheck.h"

extern "C" {
  void *malloc(std::size_t size) {
    std::clog << "attempting to malloc size: " << size << "...\n";

    if (SC_AM_Internal::real_malloc == nullptr) {
      SC_AM_Internal::real_malloc = reinterpret_cast<void*(*)(std::size_t)>(dlsym(RTLD_NEXT, "malloc"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    return SC_AM_Internal::stats->do_malloc(size);
  }

  void free(void *ptr) {
    std::clog << "attempting to free...\n";

    if (SC_AM_Internal::real_free == nullptr) {
      SC_AM_Internal::real_free = reinterpret_cast<void(*)(void*)>(dlsym(RTLD_NEXT, "free"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    return SC_AM_Internal::stats->do_free(ptr);
  }
}

// vim: et sw=2 ts=2 sts=2
