// Standard library includes.
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cstdio>

// Local includes.
#include "another_memcheck.h"

namespace SC_AM_Internal {
  Stats *stats = nullptr;

  Stats *get_init_stats() {
    Stats *stats = reinterpret_cast<SC_AM_Internal::Stats*>(
        real_malloc(sizeof(Stats)));
    stats->initialize();
    return stats;
  }

  void *(*real_malloc)(std::size_t) = nullptr;
  void (*real_free)(void*) = nullptr;

  Malloced::Malloced() : address(nullptr), size(0) {}
  Malloced::Malloced(void *address, std::size_t size) : address(address), size(size) {}

  Stats::Stats() : malloced_list(nullptr) {
  }

  void Stats::initialize() {
    malloced_list = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    malloced_list->next = nullptr;
    malloced_list->data = nullptr;
    on_exit([] ([[maybe_unused]] int status, void *ptr) {
      const Stats *stats = reinterpret_cast<const Stats*>(ptr);
      stats->print_status();
    }, this);
  }

  void *Stats::do_malloc(std::size_t size) {
    void *address = real_malloc(size);
    if (address != nullptr) {
      auto *node = malloced_list;
      while (node->next != nullptr) {
        node = node->next;
      }
      node->next = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
      node->next->next = nullptr;
      node->next->data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
      node->next->data->address = address;
      node->next->data->size = size;
    }

    return address;
  }

  void Stats::do_free(void *ptr) {
    if (ptr) {
      auto *node = malloced_list;
      decltype(node) parent_node;
      while (node->next != nullptr) {
        parent_node = node;
        node = node->next;
        if (node->data && node->data->address == ptr) {
          real_free(node->data);
          parent_node->next = node->next;
          real_free(node);
          break;
        }
      }
    }
  }

  void Stats::print_status() const {
    // This function intentionally does not free its list of malloced memory as
    // it is expected for the OS to reclaim memory from stopped processes and
    // this function only runs at the end of program execution.
    std::clog << "List of unfreed memory:\n";
    auto *node = malloced_list;
    while (node->next != nullptr) {
      node = node->next;

      std::clog << "  " << node->data->address << ": size " << node->data->size << '\n';
    }
  }
}

// vim: et sw=2 ts=2 sts=2
