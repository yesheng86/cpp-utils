#pragma once
#include "loopworker/loopworker.hpp"
#include "threadsafepipeset.hpp"

namespace util {
  namespace pipeline {

    template<class TIn>
    class Sink : public util::mt::LoopWorker {
    public:
      Sink();
      virtual ~Sink();
      virtual bool do_work() override;
      virtual bool work(const std::set<PipePtr<TIn>>&) = 0;
      virtual void stop() override;
      bool attach_in_pipe(const PipePtr<TIn>& in_pipe);
      bool detach_in_pipe(const PipePtr<TIn>& in_pipe);
    protected:
      ThreadSafePipeSet<TIn> m_in_pipes;
    };
  }
}

#include "sink.inl"
