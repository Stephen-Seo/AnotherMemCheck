#ifndef SEODISPARATE_COM_ANOTHER_MEMCHECK_H
#define SEODISPARATE_COM_ANOTHER_MEMCHECK_H

extern "C" {
#include <pthread.h>
}

namespace SC_AM_Internal {
  // Forward declaration.
  struct Stats;

  extern Stats *stats;

  constexpr int ANOTHER_MEMCHECK_QUIET_EXISTS = 1;
  constexpr int ANOTHER_MEMCHECK_QUIET_NOT_EXISTS = 2;

  /// 0 If unknown, 1 if env "ANOTHER_MEMCHECK_QUIET" exists, 2 if not.
  extern int is_env_status;

  Stats *get_init_stats();

  extern void *(*real_malloc)(std::size_t size);
  extern void *(*real_calloc)(std::size_t n, std::size_t size);
  extern void (*real_free)(void *ptr);

  struct Malloced {
    Malloced();
    Malloced(void *address, std::size_t size);

    void* address;
    std::size_t size;
  };

  struct ListNode {
    ListNode() : next(nullptr), prev(nullptr), data(nullptr) {}

    static void add_to_list(ListNode *tail, Malloced *data);
    /// Returns true if removed.
    static bool remove_from_list(ListNode *head, void *ptr);

    ListNode *next;
    ListNode *prev;
    Malloced *data;
  };


  struct Stats {
    Stats();

    ListNode *malloced_list_head;
    ListNode *malloced_list_tail;
    pthread_mutex_t pthread_mutex;

    void initialize();
    void cleanup();

    void *do_malloc(std::size_t size);
    void *do_calloc(std::size_t n, std::size_t size);
    void do_free(void *ptr);

    void print_status() const;
  };
}

#endif

// vim: et sw=2 ts=2 sts=2
