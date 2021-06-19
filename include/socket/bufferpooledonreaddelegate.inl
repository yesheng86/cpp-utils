#pragma once
#include "memorypool/bufferpool.hpp"
#include "singleton/singleton.hpp"

using namespace util::socket;
using namespace util::memorypool;

inline BufferPooledOnReadDelegate::BufferPooledOnReadDelegate()
  : m_stop(false) {}

inline BufferPooledOnReadDelegate::~BufferPooledOnReadDelegate() {}

inline BufferDescriptorPtr BufferPooledOnReadDelegate::prepare_buffer() {
  if (m_stop) {
    return nullptr;
  }
  //THREAD_SAFE_PRINT("prepare buffer %d, %d", alignment(), size());
  return util::Singleton<BufferPool>::instance().alloc(alignment(), size());
}

inline bool BufferPooledOnReadDelegate::start() {
  m_stop = false;
  return true;
}

inline bool BufferPooledOnReadDelegate::stop() {
  m_stop = true;
  return true;
}

inline bool BufferPooledOnReadDelegate::is_stopped() {
  return m_stop;
}
