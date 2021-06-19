#pragma once
#include "singleton/singleton.hpp"

using namespace util::pipeline;
using namespace util::memorypool;

template<class T>
Pipe<T>::Pipe()
  : m_queue() {}

template<class T>
Pipe<T>::~Pipe() {}

template<class T>
void Pipe<T>::push(const std::shared_ptr<T>& one) {
  m_queue.push(one);
}

template<class T>
void Pipe<T>::push(std::shared_ptr<T>&& one) {
  m_queue.push(std::move(one));
}

template<class T>
bool Pipe<T>::try_pop(std::shared_ptr<T>& one) {
  return m_queue.try_pop(one);
}

template<class T>
bool Pipe<T>::wait_and_pop(std::shared_ptr<T>& one) {
  return m_queue.wait_and_pop(one);
}

template<class T>
void Pipe<T>::stop() {
  m_queue.stop_queue();
}

template<class T>
std::shared_ptr<T> Pipe<T>::new_one() {
  return util::Singleton<ObjectPool<T>>::instance().new_one();
}

template<class T>
PipePtr<T> util::pipeline::make_pipe() {
  return std::make_shared<Pipe<T>>();
}
