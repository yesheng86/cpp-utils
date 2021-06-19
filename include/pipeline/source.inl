#pragma once

using namespace util::pipeline;

template<class TOut>
inline Source<TOut>::Source()
  : util::mt::LoopWorker(), m_out_pipes() {}

template<class TOut>
inline Source<TOut>::~Source() = default;

template<class TOut>
inline bool Source<TOut>::do_work() {
  std::set<PipePtr<TOut>> out_pipes;
  if (!m_out_pipes.wait_and_get_nonempty_copy(out_pipes)) {
    return false;
  }
  return work(out_pipes);
}

template<class TOut>
inline void Source<TOut>::stop() {
  m_out_pipes.stop();
  LoopWorker::stop();
}

template<class TOut>
inline bool Source<TOut>::attach_out_pipe(const PipePtr<TOut>& out_pipe) {
  return m_out_pipes.add(out_pipe);
}

template<class TOut>
inline bool Source<TOut>::detach_out_pipe(const PipePtr<TOut>& out_pipe) {
  return m_out_pipes.remove(out_pipe);
}
