// Standard library includes.
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <cstdio>

// Local includes.
#include "another_memcheck.h"

namespace SC_AM_Internal {
  Stats *stats = nullptr;
  int is_env_status = 0;

  Stats *get_init_stats() {
    Stats *stats = reinterpret_cast<SC_AM_Internal::Stats*>(
        real_malloc(sizeof(Stats)));
    stats->initialize();
    is_env_status = getenv("ANOTHER_MEMCHECK_QUIET") != nullptr
      ? ANOTHER_MEMCHECK_QUIET_EXISTS
      : ANOTHER_MEMCHECK_QUIET_NOT_EXISTS;
    return stats;
  }

  void *(*real_malloc)(std::size_t) = nullptr;
  void *(*real_calloc)(std::size_t, std::size_t) = nullptr;
  void (*real_free)(void*) = nullptr;

  Malloced::Malloced() : address(nullptr), size(0) {}
  Malloced::Malloced(void *address, std::size_t size) : address(address), size(size) {}

  void ListNode::add_to_list(ListNode *tail, Malloced *data) {
    ListNode *new_node = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    new_node->next = tail;
    new_node->prev = tail->prev;
    tail->prev->next = new_node;
    tail->prev = new_node;

    new_node->data = data;
  }

  bool ListNode::remove_from_list(ListNode *head, void *ptr) {
    ListNode *node = head;
    while (node != nullptr) {
      node = node->next;
      if (node->data && node->data->address == ptr) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        real_free(node->data);
        real_free(node);
        return true;
      }
    }

    return false;
  }

  Stats::Stats() : malloced_list_head(nullptr), malloced_list_tail(nullptr), pthread_mutex{.__align=0} {
  }

  void Stats::initialize() {
    malloced_list_head = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    malloced_list_tail = reinterpret_cast<ListNode*>(real_malloc(sizeof(ListNode)));
    malloced_list_head->next = malloced_list_tail;
    malloced_list_head->prev = nullptr;
    malloced_list_tail->next = nullptr;
    malloced_list_tail->prev = malloced_list_head;
    malloced_list_head->data = nullptr;
    malloced_list_tail->data = nullptr;

    pthread_mutex_init(&pthread_mutex, nullptr);

    on_exit([] ([[maybe_unused]] int status, void *ptr) {
      Stats *stats = reinterpret_cast<Stats*>(ptr);
      stats->print_status();
      stats->cleanup();
    }, this);
  }

  void Stats::cleanup() {
    pthread_mutex_destroy(&pthread_mutex);
    // TODO maybe cleanup list, but it is the end of the program.
  }

  void *Stats::do_malloc(std::size_t size) {
    if(int ret = pthread_mutex_lock(&pthread_mutex); ret == EINVAL) {
      std::clog << "ERROR: pthread mutex not properly initialized!\n";
      return nullptr;
    }

    void *address = real_malloc(size);
    if (address != nullptr) {
      Malloced *data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
      data->address = address;
      data->size = size;
      ListNode::add_to_list(malloced_list_tail, data);
    }

    pthread_mutex_unlock(&pthread_mutex);
    return address;
  }

  void *Stats::do_calloc(std::size_t n, std::size_t size) {
    if(int ret = pthread_mutex_lock(&pthread_mutex); ret == EINVAL) {
      std::clog << "ERROR: pthread mutex not properly initialized!\n";
      return nullptr;
    }

    void *address = real_calloc(n, size);
    if (address != nullptr) {
      Malloced *data = reinterpret_cast<Malloced*>(real_malloc(sizeof(Malloced)));
      data->address = address;
      data->size = n * size;
      ListNode::add_to_list(malloced_list_tail, data);
    }

    pthread_mutex_unlock(&pthread_mutex);
    return address;
  }

  void Stats::do_free(void *ptr) {
    if(int ret = pthread_mutex_lock(&pthread_mutex); ret == EINVAL) {
      std::clog << "ERROR: pthread mutex not properly initialized!\n";
      return;
    }

    if (ptr) {
      if(!ListNode::remove_from_list(malloced_list_head, ptr)) {
        std::clog << "WARNING: Attempted free of unknown memory location!\n";
      }
    }

    pthread_mutex_unlock(&pthread_mutex);
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
        std::clog << "  " << node->data->address << ": size " << node->data->size << '\n';
      }
    }
  }
}

// vim: et sw=2 ts=2 sts=2
