#pragma once
#include "loopworker/loopworker.hpp"
#include "threadsafepipeset.hpp"

namespace util {
  namespace pipeline {

    template<class TOut>
    class Source : public util::mt::LoopWorker {
    public:
      Source();
      virtual ~Source();
      virtual bool do_work() override;
      virtual bool work(const std::set<PipePtr<TOut>>&) = 0;
      virtual void stop() override;
      bool attach_out_pipe(const PipePtr<TOut>&);
      bool detach_out_pipe(const PipePtr<TOut>&);
    protected:
      ThreadSafePipeSet<TOut> m_out_pipes;
    };
  }
}

#include "source.inl"
