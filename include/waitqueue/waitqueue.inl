#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace util::mt;

template<class T>
class WaitQueue<T>::Impl {
public:
  Impl()
    : m_queue(), m_mutex(), m_condition(), m_stopped(false) {}

  virtual ~Impl() {
  }

  void push(const T& element) {
    {
      std::unique_lock lock(m_mutex);
      m_queue.push(element);
    }
    m_condition.notify_one();
  }

  void push(T&& element){
    {
      std::unique_lock lock(m_mutex);
      m_queue.push(std::move(element));
    }
    m_condition.notify_one();
  }

  bool empty() {
    std::unique_lock lock(m_mutex);
    return m_queue.empty();
  }

  bool try_pop(T& element) {
    std::unique_lock lock(m_mutex);
    if (m_queue.empty()) {
      return false;
    }
    element = std::move(m_queue.front());
    m_queue.pop();
    return true;
  }

  bool wait_and_pop(T& element) {
    std::unique_lock lock(m_mutex);
    while (m_queue.empty() && !m_stopped) {
      m_condition.wait(lock);
    }
    if (m_stopped) {
      return false;
    }
    element = std::move(m_queue.front());
    m_queue.pop();
    return true;
  }

  void stop_queue()  {
    {
      std::unique_lock lock(m_mutex);
      m_stopped = true;
    }
    m_condition.notify_all();
  }

private:
  std::queue<T> m_queue;
  std::mutex m_mutex;
  std::condition_variable m_condition;
  bool m_stopped;
};

template<class T>
inline WaitQueue<T>::WaitQueue()
  : util::base::MoveOnly(), m_p_impl{ std::make_unique<Impl>() } {}

template<class T>
inline WaitQueue<T>::~WaitQueue() = default;

template<class T>
inline void WaitQueue<T>::push(const T& element) {
  m_p_impl->push(element);}

template<class T>
inline void WaitQueue<T>::push(T&& element) {
  m_p_impl->push(std::move(element));}

template<class T>
inline bool WaitQueue<T>::empty() {
  return m_p_impl->empty();}

template<typename T>
inline bool WaitQueue<T>::try_pop(T& element) {
  return m_p_impl->try_pop(element);}

template<class T>
inline bool WaitQueue<T>::wait_and_pop(T& element) {
  return m_p_impl->wait_and_pop(element);}

template<class T>
inline void WaitQueue<T>::stop_queue() {
  m_p_impl->stop_queue();}
