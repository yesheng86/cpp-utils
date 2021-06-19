#pragma once

using namespace util::pipeline;

template<class TIn>
inline Sink<TIn>::Sink()
  : util::mt::LoopWorker(), m_in_pipes() {}

template<class TIn>
inline Sink<TIn>::~Sink() = default;

template<class TIn>
inline bool Sink<TIn>::do_work() {
  std::set<PipePtr<TIn>> in_pipes;
  if (!m_in_pipes.wait_and_get_nonempty_copy(in_pipes)) {
    return false;
  }
  return work(in_pipes);
}

template<class TIn>
inline void Sink<TIn>::stop() {
  m_in_pipes.stop();
  LoopWorker::stop();
}

template<class TIn>
inline bool Sink<TIn>::attach_in_pipe(const PipePtr<TIn>& in_pipe) {
  return m_in_pipes.add(in_pipe);
}

template<class TIn>
inline bool Sink<TIn>::detach_in_pipe(const PipePtr<TIn>& in_pipe) {
  return m_in_pipes.remove(in_pipe);
}
