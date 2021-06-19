#pragma once
#include <chrono>
#include "log/log.hpp"

using namespace util::pipeline;
using namespace std::chrono_literals;

template<class T>
inline util::pipeline::Duplicator<T>::Duplicator(bool deep)
  : util::mt::LoopWorker(),
  m_in_pipes(), m_out_pipes(), m_deep(deep) {}

template<class T>
inline Duplicator<T>::~Duplicator() {}

template<class T>
inline bool Duplicator<T>::do_work() {
  std::set<PipePtr<T>> in_pipes;
  std::set<PipePtr<T>> out_pipes;
  if (!m_in_pipes.wait_and_get_nonempty_copy(in_pipes)) {
    return false;
  }

  if (in_pipes.size() != 1) {
    THREAD_SAFE_PRINT("duplicator input size is %d, should be 1", in_pipes.size());
    return false;
  }
  std::shared_ptr<T> in_ptr;
  if (!(*in_pipes.begin())->wait_and_pop(in_ptr) && !in_ptr) {
    THREAD_SAFE_PRINT("failed to get one from input pipe");
    return true;
  }

  if (!m_out_pipes.try_get_nonempty_copy(out_pipes)) {
    THREAD_SAFE_PRINT("duplicator no out pipe, discard one");
    return true;
  }

  for (const PipePtr<T>& pipe : out_pipes) {
    if (m_deep) {
      std::shared_ptr<T> out_ptr = pipe->new_one();
      out_ptr = in_ptr;
      pipe->push(out_ptr);
    } else {
      pipe->push(in_ptr);
    }
    //THREAD_SAFE_PRINT("duplicator push one");
  }
  return true;
}

template<class T>
inline void Duplicator<T>::stop() {
  m_in_pipes.stop();
  m_out_pipes.stop();
  LoopWorker::stop();
}

template<class T>
inline bool Duplicator<T>::attach_in_pipe(const PipePtr<T>& in_pipe) {
  return m_in_pipes.add(in_pipe);
}

template<class T>
inline bool Duplicator<T>::detach_in_pipe(const PipePtr<T>& in_pipe) {
  return m_in_pipes.remove(in_pipe);
}

template<class T>
inline bool Duplicator<T>::attach_out_pipe(const PipePtr<T>& out_pipe) {
  return m_out_pipes.add(out_pipe);
}

template<class T>
inline bool Duplicator<T>::detach_out_pipe(const PipePtr<T>& out_pipe) {
  return m_out_pipes.remove(out_pipe);
}
