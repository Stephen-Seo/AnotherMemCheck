// Standard library includes.
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <mutex>

// Local includes.
#include "another_memcheck.h"

// Function declaration.
void exit_handler_stats();

namespace SC_AM_Internal {
  Stats *stats = nullptr;
  int is_env_status = 0;
  unsigned long long Malloced::count = 0;

  Stats *get_init_stats() {
    Stats *stats = reinterpret_cast<SC_AM_Internal::Stats*>(
        real_malloc(sizeof(Stats)));
    [[maybe_unused]] void *unused = new(stats) Stats{};
    is_env_status = getenv("ANOTHER_MEMCHECK_QUIET") != nullptr
      ? ANOTHER_MEMCHECK_QUIET_EXISTS
      : ANOTHER_MEMCHECK_QUIET_NOT_EXISTS;
    return stats;
  }

  void *(*real_malloc)(std::size_t) = nullptr;
  void *(*real_calloc)(std::size_t, std::size_t) = nullptr;
  void *(*real_realloc)(void*, std::size_t) = nullptr;
  void (*real_free)(void*) = nullptr;

  Malloced::Malloced() : address(nullptr), size(0), id(0) {}
  Malloced::Malloced(void *address, std::size_t size) : address(address), size(size), id(0) {}

  void ListNode::add_to_list(ListNode *tail, Malloced *data) {
    ListNode *new_node = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    new_node->next = tail;
    new_node->prev = tail->prev;
    tail->prev->next = new_node;
    tail->prev = new_node;

    new_node->data = data;
  }

  void ListNode::add_to_list(ListNode *tail, ListNode *node) {
    node->next = tail;
    node->prev = tail->prev;
    tail->prev->next = node;
    tail->prev = node;
  }

  bool ListNode::remove_from_list(ListNode *head,
                                  void *ptr,
                                  Stats *defer_handler) {
    ListNode *node = head;
    while (node != nullptr) {
      node = node->next;
      if (node && node->data && node->data->address == ptr) {
        if (is_env_status == ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
          std::clog << " removing id: " << node->data->id;
          if (!defer_handler) {
            std::clog << std::endl;
          }
        }
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (defer_handler) {
          defer_handler->deferred_node = node;
        } else {
          real_free(node->data);
          real_free(node);
        }
        return true;
      }
    }

    return false;
  }

  Stats::Stats() : malloced_list_head(nullptr), malloced_list_tail(nullptr), deferred_node(nullptr), recursive_mutex(nullptr) {
    malloced_list_head = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    malloced_list_tail = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    malloced_list_head->next = malloced_list_tail;
    malloced_list_head->prev = nullptr;
    malloced_list_tail->next = nullptr;
    malloced_list_tail->prev = malloced_list_head;
    malloced_list_head->data = nullptr;
    malloced_list_tail->data = nullptr;

    deferred_node = nullptr;

    recursive_mutex = real_malloc(sizeof(std::recursive_mutex));
    [[maybe_unused]] void *unused = new(recursive_mutex) std::recursive_mutex{};

    std::atexit(exit_handler_stats);
  }

  Stats::~Stats() {
    using std_recursive_mutex = std::recursive_mutex;
    ((std::recursive_mutex*)recursive_mutex)->~std_recursive_mutex();
    real_free(recursive_mutex);
    // TODO maybe cleanup list, but it is the end of the program.
  }

  void *Stats::do_malloc(std::size_t size) {
    ((std::recursive_mutex*)recursive_mutex)->lock();

    void *address = real_malloc(size);
    if (address != nullptr) {
      Malloced *data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
      data->address = address;
      data->size = size;
      data->id = data->count++;
      if (is_env_status == ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
        std::clog << " id: " << data->id << std::endl;
      }
      ListNode::add_to_list(malloced_list_tail, data);
    }

    ((std::recursive_mutex*)recursive_mutex)->unlock();
    return address;
  }

  void *Stats::do_calloc(std::size_t n, std::size_t size) {
    ((std::recursive_mutex*)recursive_mutex)->lock();

    void *address = real_calloc(n, size);
    if (address != nullptr) {
      Malloced *data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
      data->address = address;
      data->size = n * size;
      data->id = data->count++;
      if (is_env_status == ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
        std::clog << " id: " << data->id << std::endl;
      }
      ListNode::add_to_list(malloced_list_tail, data);
    }

    ((std::recursive_mutex*)recursive_mutex)->unlock();
    return address;
  }

  void *Stats::do_realloc(void *ptr, std::size_t size) {
    ((std::recursive_mutex*)recursive_mutex)->lock();

    if (ptr) {
      if (!ListNode::remove_from_list(malloced_list_head, ptr, this)) {
        std::clog << "WARNING: Attempted free of unknown memory location!\n";
      }
    }

    void *address = real_realloc(ptr, size);
    if (address != nullptr) {
      if (deferred_node) {
        deferred_node->data->address = address;
        deferred_node->data->size = size;
        deferred_node->data->id = deferred_node->data->count++;
        if (is_env_status == ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
          std::clog << ", adding id: " << deferred_node->data->id << std::endl;
        }
        ListNode::add_to_list(malloced_list_tail, deferred_node);
        deferred_node = nullptr;
      } else {
        Malloced *data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
        data->address = address;
        data->size = size;
        data->id = data->count++;
        if (is_env_status == ANOTHER_MEMCHECK_QUIET_NOT_EXISTS) {
          std::clog << ", adding id: " << data->id << std::endl;
        }
        ListNode::add_to_list(malloced_list_tail, data);
      }
    }

    ((std::recursive_mutex*)recursive_mutex)->unlock();
    return address;
  }

  bool Stats::do_free(void *ptr) {
    bool result = false;
    ((std::recursive_mutex*)recursive_mutex)->lock();

    if (ptr) {
      if (!ListNode::remove_from_list(malloced_list_head, ptr)) {
        std::clog << "WARNING: Attempted free of unknown memory location!\n";
      } else {
        result = true;
        real_free(ptr);
      }
    }

    ((std::recursive_mutex*)recursive_mutex)->unlock();
    return result;
  }

  void Stats::print_status() const {
    // This function intentionally does not free its list of malloced memory as
    // it is expected for the OS to reclaim memory from stopped processes and
    // this function only runs at the end of program execution.
    std::clog << "List of unfreed memory:\n";
    auto *node = malloced_list_head;
    while (node->next != nullptr) {
      node = node->next;

      if (node->data) {
        std::clog << "  " << node->data->address
          << ": size " << node->data->size
          << " id " << node->data->id << '\n';
      }
    }
  }
}

// Function definition.
void exit_handler_stats() {
  if (SC_AM_Internal::stats != nullptr) {
    SC_AM_Internal::stats->print_status();

    // Avoids segfault when program calls `exit(...)`. OS should reclaim memory
    // when the process ends even if it isn't free'd here.
    //using SCS_AM_INTERNAL_Stats = SC_AM_Internal::Stats;
    //SC_AM_Internal::stats->~SCS_AM_INTERNAL_Stats();
    //SC_AM_Internal::stats = nullptr;
  }
}

// vim: et sw=2 ts=2 sts=2
