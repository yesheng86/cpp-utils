#pragma once
#include "loopworker/loopworker.hpp"
#include "threadsafepipeset.hpp"

namespace util {
  namespace pipeline {

    template<class T>
    class Duplicator : public util::mt::LoopWorker {
    public:
      // bool: true - deep copy, false - shallow copy
      Duplicator(bool);
      virtual ~Duplicator() override;
      virtual bool do_work() override;
      virtual void stop() override;
      bool attach_in_pipe(const PipePtr<T>& in_pipe);
      bool detach_in_pipe(const PipePtr<T>& in_pipe);
      bool attach_out_pipe(const PipePtr<T>& out_pipe);
      bool detach_out_pipe(const PipePtr<T>& out_pipe);
    protected:
      ThreadSafePipeSet<T> m_in_pipes;
      ThreadSafePipeSet<T> m_out_pipes;
      bool m_deep;
    };
  }
}

#include "duplicator.inl"
