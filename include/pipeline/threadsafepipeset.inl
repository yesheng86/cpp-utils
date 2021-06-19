#pragma once

using namespace util::pipeline;

template<class T>
inline ThreadSafePipeSet<T>::ThreadSafePipeSet()
  : m_pipe_set(), m_stopped(false) {}

template<class T>
inline ThreadSafePipeSet<T>::~ThreadSafePipeSet() {}

template<class T>
inline bool ThreadSafePipeSet<T>::add(const PipePtr<T>& pipe) {
  bool result;
  {
    std::unique_lock lock(m_mutex);
    result = (m_pipe_set.insert(pipe)).second;
  }
  m_condition.notify_one();
  return result;
}

template<class T>
inline bool ThreadSafePipeSet<T>::remove(const PipePtr<T>& pipe) {
  std::unique_lock lock(m_mutex);
  return m_pipe_set.erase(pipe);
}

template<class T>
inline bool ThreadSafePipeSet<T>::wait_and_get_nonempty_copy(std::set<PipePtr<T>>& set) {
  std::unique_lock lock(m_mutex);
  while (m_pipe_set.empty() && !m_stopped) {
    m_condition.wait(lock);
  }
  if (m_stopped) {
    return false;
  }
  set = m_pipe_set;
  return true;
}

template<class T>
inline bool ThreadSafePipeSet<T>::try_get_nonempty_copy(std::set<PipePtr<T>>& set) {
  std::unique_lock lock(m_mutex);
  if (m_pipe_set.empty()) {
    return false;
  }
  set = m_pipe_set;
  return true;
}

template<class T>
inline std::size_t ThreadSafePipeSet<T>::size() {
  std::unique_lock lock(m_mutex);
  return m_pipe_set.size();
}

template<class T>
inline void ThreadSafePipeSet<T>::stop() {
  {
    std::unique_lock lock(m_mutex);
    m_stopped = true;
  }
  m_condition.notify_all();  
}
