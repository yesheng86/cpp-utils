#pragma once
#include <functional>
#include <iostream>
#include "waitqueue/waitqueue.hpp"

using namespace util::memorypool;

template <class T>
template <class U>
class ObjectPool<T>::Impl {
public:
  Impl() = default;

  ~Impl() {
    m_free_list.stop_queue();
    U* p_obj;
    while(m_free_list.try_pop(p_obj)) {
      if (p_obj) {
        free(p_obj);
      }
    }
  }

  std::shared_ptr<U> new_one() {
    U *one = NULL;
    if (!m_free_list.try_pop(one)) {
      one = new U();
    }
    else {
      if (one) {
        new(one) U();
      }
    }
    return std::shared_ptr<U>(one, std::bind(&ObjectPool<T>::Impl<U>::deleter, this, std::placeholders::_1));
  }

private:
  void deleter(U* one) {
    if (!one) {
      return;
    }
    one->~U();
    m_free_list.push(one);
  }

  util::mt::WaitQueue<U*> m_free_list;
};

template <class T>
ObjectPool<T>::ObjectPool()
  : util::base::MoveOnly(), m_p_impl{ std::make_unique<Impl<T>>() } {}

template <class T>
ObjectPool<T>::~ObjectPool() = default;

template <class T>
std::shared_ptr<T> ObjectPool<T>::new_one() {
  return m_p_impl->new_one();
}
