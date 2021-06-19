#pragma once
#include "loopworker/loopworker.hpp"
#include "threadsafepipeset.hpp"

namespace util {
  namespace pipeline {

    template<class TIn, class TOut>
    class Processor : public util::mt::LoopWorker {
    public:
      Processor();
      virtual ~Processor();
      virtual bool do_work() override;
      virtual bool work(const std::set<PipePtr<TIn>>&, const std::set<PipePtr<TOut>>&) = 0;
      virtual void stop() override;
      bool attach_in_pipe(const PipePtr<TIn>& in_pipe);
      bool detach_in_pipe(const PipePtr<TIn>& in_pipe);
      bool attach_out_pipe(const PipePtr<TOut>& out_pipe);
      bool detach_out_pipe(const PipePtr<TOut>& out_pipe);
    private:
      ThreadSafePipeSet<TIn> m_in_pipes;
      ThreadSafePipeSet<TOut> m_out_pipes;
    };
  }
}

#include "processor.inl"
