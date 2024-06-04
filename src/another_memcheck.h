#ifndef SEODISPARATE_COM_ANOTHER_MEMCHECK_H
#define SEODISPARATE_COM_ANOTHER_MEMCHECK_H

namespace SC_AM_Internal {
  // Forward declaration.
  struct Stats;

  extern Stats *stats;

  Stats *get_init_stats();

  constexpr unsigned int DATA_BLOCK_SIZE = 24;
  constexpr unsigned int DATA_ITEMS_SIZE = 500;

  extern void *(*real_malloc)(std::size_t size);
  extern void (*real_free)(void *ptr);

  struct Malloced {
    Malloced();
    Malloced(void *address, std::size_t size);

    void* address;
    std::size_t size;
  };

  struct ListNode {
    ListNode() : next(nullptr), data(nullptr) {}

    ListNode *next;
    Malloced *data;
  };

  using MapDataType = std::pair<const void*, Malloced>;

  struct Stats {
    Stats();

    ListNode *malloced_list;

    void initialize();

    void *do_malloc(std::size_t size);
    void do_free(void *ptr);

    void print_status() const;
  };
}

#endif

// vim: et sw=2 ts=2 sts=2
