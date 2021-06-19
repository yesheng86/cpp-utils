#pragma once

using namespace util::mt;

inline LoopWorker::LoopWorker()
  : util::base::MoveOnly(), m_stopped(true) {
}

inline LoopWorker::~LoopWorker() {
  if (!m_stopped) {
    stop();
  }
}

inline void LoopWorker::start() {
  if (!m_stopped) {
    return;
  }
  m_stopped = false;
  std::thread([this]() {
      if(!pre_work()) {
        return;
      }
      while(!m_stopped && do_work()) {}
      if(!post_work()) {
        return;
      }
    }).detach();
}

inline void LoopWorker::stop() {
  m_stopped = true;
}

inline bool LoopWorker::pre_work() {
  return true;
}

inline bool LoopWorker::post_work() {
  return true;
}
