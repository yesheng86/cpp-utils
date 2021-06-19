#pragma once

using namespace util::pipeline;

template<class TIn, class TOut>
inline Processor<TIn, TOut>::Processor()
  : util::mt::LoopWorker(),
  m_in_pipes(), m_out_pipes() {}

template<class TIn, class TOut>
inline Processor<TIn, TOut>::~Processor() = default;

template<class TIn, class TOut>
inline bool Processor<TIn, TOut>::do_work() {
  std::set<PipePtr<TIn>> in_pipes;
  std::set<PipePtr<TOut>> out_pipes;
  if (!m_in_pipes.wait_and_get_nonempty_copy(in_pipes)) {
    return false;
  }
  if (!m_out_pipes.wait_and_get_nonempty_copy(out_pipes)) {
    return false;
  }
  return work(in_pipes, out_pipes);
}

template<class TIn, class TOut>
inline void Processor<TIn, TOut>::stop() {
  m_in_pipes.stop();
  m_out_pipes.stop();
  LoopWorker::stop();
}

template<class TIn, class TOut>
inline bool Processor<TIn, TOut>::attach_in_pipe(const PipePtr<TIn>& in_pipe) {
  return m_in_pipes.add(in_pipe);
}

template<class TIn, class TOut>
inline bool Processor<TIn, TOut>::detach_in_pipe(const PipePtr<TIn>& in_pipe) {
  return m_in_pipes.remove(in_pipe);
}

template<class TIn, class TOut>
inline bool Processor<TIn, TOut>::attach_out_pipe(const PipePtr<TOut>& out_pipe) {
  return m_out_pipes.add(out_pipe);
}

template<class TIn, class TOut>
inline bool Processor<TIn, TOut>::detach_out_pipe(const PipePtr<TOut>& out_pipe) {
  return m_out_pipes.remove(out_pipe);
}
