#pragma once
#include <functional>
#include <map>
#include "waitqueue/waitqueue.hpp"
#include "log/log.hpp"

using namespace util::memorypool;
using namespace util::buffer;
using namespace util::mt;

class BufferPool::Impl {
  using BufferDescriptorQueue = WaitQueue<BufferDescriptor*>;
public:
  Impl() = default;

  ~Impl() {
    for (auto& pair: m_buf_desc_queue_map) {
      BufferDescriptorQueue& queue = pair.second;
      queue.stop_queue();
      BufferDescriptor* p_desc = NULL;
      while (queue.try_pop(p_desc)) {
        if (p_desc->raw_ptr()) {
          free(p_desc->raw_ptr());
        }
        if (p_desc) {
          delete(p_desc);
        }
      }
    }
    THREAD_SAFE_PRINT("BufferPool::~Impl");
  }

  BufferDescriptorPtr alloc(uint32_t size) {
    return alloc(0, size);
  }
  
  BufferDescriptorPtr alloc(uint32_t alignment, uint32_t size) {
    BufferDescriptor* p_buf_desc = NULL;
    uint64_t key = get_map_key(alignment, size);
    if (!m_buf_desc_queue_map[key].try_pop(p_buf_desc)) {
      void* buf_ptr = NULL;
      if (alignment == 0) {
        buf_ptr = std::malloc(size);
      } else {
        buf_ptr = std::aligned_alloc(alignment, size);
      }
      if (!buf_ptr) {
        return nullptr;
      }
      p_buf_desc = new BufferDescriptor(buf_ptr, alignment, size);
      if (!p_buf_desc) {
        free(buf_ptr);
        return nullptr;
      }
    }
    return std::shared_ptr<BufferDescriptor>(p_buf_desc, std::bind(&BufferPool::Impl::deleter, this, std::placeholders::_1));
  }
  
private:
  void deleter(BufferDescriptor* p_buf_desc) {
    if (p_buf_desc == NULL) {
      return;
    }
    uint64_t key = get_map_key(p_buf_desc->alignment(), p_buf_desc->size());
    m_buf_desc_queue_map[key].push(p_buf_desc);
  }

  inline uint64_t get_map_key(uint32_t alignment, uint32_t size) {
    uint64_t key = (uint64_t)alignment;
    key = (key << 32) | (uint64_t)size;
    return key;
  }
      
  std::map<uint64_t, BufferDescriptorQueue> m_buf_desc_queue_map;
};

inline BufferPool::BufferPool()
  : util::base::MoveOnly(), m_p_impl{ std::make_unique<Impl>() } {}

inline BufferPool::~BufferPool() = default;

inline BufferDescriptorPtr BufferPool::alloc(uint32_t size) {
  return m_p_impl->alloc(size);
}

inline BufferDescriptorPtr BufferPool::alloc(uint32_t alignment, uint32_t size) {
  return m_p_impl->alloc(alignment, size);
}
