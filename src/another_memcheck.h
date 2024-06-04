#ifndef SEODISPARATE_COM_ANOTHER_MEMCHECK_H
#define SEODISPARATE_COM_ANOTHER_MEMCHECK_H

// Standard library includes.
/*#include <unordered_map>*/
/*#include <memory>*/
/*#include <cstdio>*/

namespace SC_AM_Internal {
  // Forward declaration.
  struct Stats;

  extern Stats *stats;

  Stats *get_init_stats();

  constexpr unsigned int DATA_BLOCK_SIZE = 24;
  constexpr unsigned int DATA_ITEMS_SIZE = 500;

  /// Assumes each data block is 24 bytes.
/*  extern unsigned char data[DATA_BLOCK_SIZE * DATA_ITEMS_SIZE];*/
/*  extern unsigned int data_offset;*/


  extern void *(*real_malloc)(std::size_t size);
  extern void (*real_free)(void *ptr);

  /// Always returns DATA_BLOCK_SIZE blocks of memory.
/*  void *alt_malloc();*/
/*  void alt_free(void *ptr);*/

  struct Malloced {
    Malloced();
    Malloced(void *address, std::size_t size);

    void* address;
    std::size_t size;
  };

  // TODO: hashmap for faster memchecking.
  struct ListNode {
    ListNode() : next(nullptr), data(nullptr) {}

    ListNode *next;
    Malloced *data;
  };

  using MapDataType = std::pair<const void*, Malloced>;

/*  struct RealAllocator : public std::allocator<MapDataType> {*/
/*    // Default allocators are stateless, but declare constructor anyways.*/
/*    RealAllocator();*/
/*    virtual ~RealAllocator();*/
/**/
/*    MapDataType *allocate(std::size_t n, const void *hint = 0) override;*/
/*    void deallocate(MapDataType *ptr, std::size_t n) override;*/
/*  };*/

  struct Stats {
    Stats();

/*    void *(*real_malloc)(std::size_t);*/
/*    void (*real_free)(void*) noexcept;*/

/*    std::unordered_map<void*,*/
/*                       Malloced,*/
/*                       std::hash<void*>,*/
/*                       std::equal_to<void*>,*/
/*                       RealAllocator> malloced_addresses;*/
    ListNode *malloced_list;

    void initialize();

    void *do_malloc(std::size_t size);
    void do_free(void *ptr);

    void print_status() const;
  };
}

#endif

// vim: et sw=2 ts=2 sts=2
