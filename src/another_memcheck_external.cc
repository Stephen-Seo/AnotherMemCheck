// Standard library includes.
#include <iostream>

// Unix includes.
#include <dlfcn.h>

// Local includes.
#include "another_memcheck.h"

extern "C" {
  void *malloc(std::size_t size) {
    if (SC_AM_Internal::real_malloc == nullptr) {
      SC_AM_Internal::real_malloc = reinterpret_cast<void*(*)(std::size_t)>(dlsym(RTLD_NEXT, "malloc"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    if (SC_AM_Internal::is_env_status == SC_AM_Internal::ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
      std::clog << "attempting to malloc size: " << size;
    }

    return SC_AM_Internal::stats->do_malloc(size);
  }

  void *calloc(std::size_t n, std::size_t size) {
    if (SC_AM_Internal::real_calloc == nullptr) {
      SC_AM_Internal::real_calloc = reinterpret_cast<void*(*)(std::size_t, std::size_t)>(
          dlsym(RTLD_NEXT, "calloc"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    if (SC_AM_Internal::is_env_status == SC_AM_Internal::ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
      std::clog << "attempting to calloc size: " << size;
    }

    return SC_AM_Internal::stats->do_calloc(n, size);
  }

  void *realloc(void *ptr, std::size_t size) {
    if (SC_AM_Internal::real_realloc == nullptr) {
      SC_AM_Internal::real_realloc = reinterpret_cast<void*(*)(void*, std::size_t)>(dlsym(RTLD_NEXT, "realloc"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    if (SC_AM_Internal::is_env_status == SC_AM_Internal::ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
      std::clog << "attempting to realloc size: " << size;
    }

    return SC_AM_Internal::stats->do_realloc(ptr, size);
  }

  void free(void *ptr) {
    if (SC_AM_Internal::real_free == nullptr) {
      SC_AM_Internal::real_free = reinterpret_cast<void(*)(void*)>(dlsym(RTLD_NEXT, "free"));
    }

    if (SC_AM_Internal::stats == nullptr) {
      SC_AM_Internal::stats = SC_AM_Internal::get_init_stats();
    }

    if (SC_AM_Internal::is_env_status == SC_AM_Internal::ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
      std::clog << "attempting to free";
    }

    // TODO maybe handle boolean result, but already prints warnings on error.
    SC_AM_Internal::stats->do_free(ptr);
    std::clog << std::endl;
  }
}

// vim: et sw=2 ts=2 sts=2
