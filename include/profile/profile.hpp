#pragma once
#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <atomic>
#include <map>
#include <thread>
#include "waitqueue/waitqueue.hpp"

#ifdef ENABLE_PROFILE

#define ENABLE_JSON_PROFILER_SCOPED(f) util::profile::ProfileGuard _json_profile_guard(util::profile::ProfileRecordTypeJson, f);

#define ENABLE_PLAIN_PROFILER_SCOPED(f) util::profile::ProfileGuard _json_profile_guard(util::profile::ProfilRecordTypePlain f);

#define START_TIMER(m)  { util::profile::Timer _nb_profile_timer(m);

#define START_TIMER_2(m, c)  { util::profile::Timer _nb_profile_timer(m, c);

#define START_SCOPED_TIMER(m) util::profile::Timer _nb_profile_timer(m);

#define START_SCOPED_TIMER_2(m, c) util::profile::Timer _nb_profile_timer(m, c);

#define CANCEL_SCOPED_TIMER _nb_profile_timer.cancel();

#define STOP_TIMER }

#else
#define ENABLE_JSON_PROFILER_SCOPED(f)

#define ENABLE_PLAIN_PROFILER_SCOPED(f)

#define START_TIMER(s)

#define START_TIMER_2(m, c)

#define START_SCOPED_TIMER(s)

#define START_SCOPED_TIMER_2(m, c)

#define CANCEL_SCOPED_TIMER

#define STOP_TIMER
#endif

namespace util {
  namespace profile {
    class Timer {
    public:
      Timer(const std::string&);
      Timer(std::string&&);
      Timer(const std::string&, uint32_t);
      Timer(std::string&&, uint32_t);
      Timer& operator=(const Timer&) = delete;
      Timer& operator=(Timer&&) = delete;
      void cancel();
      virtual ~Timer();
    private:
      std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
      const std::string m_message;
      uint32_t m_message_id;
      uint32_t m_category;
      bool m_cancel;
    };

    class MessageLookup {
    public:
      static MessageLookup &get_instance();
      MessageLookup(const MessageLookup&) = delete;
      MessageLookup(MessageLookup&&) = delete;
      void operator=(const MessageLookup&) = delete;
      void operator=(MessageLookup&&) = delete;
      uint32_t register_message(const std::string& string);
      uint32_t register_message(std::string&& string);
      std::string &get_message(uint32_t id);
      uint32_t size();
    private:
      MessageLookup();
      virtual ~MessageLookup();
      std::map<uint32_t, std::string> m_message_map;
      std::mutex m_mutex;
      uint32_t m_id_increaser;
    };

    typedef struct _TimerRecord {
      int64_t duration_us;
      uint32_t category;
      uint32_t message_id;
    } TimerRecord;

    class BaseTimerRecorder {
    public:
      BaseTimerRecorder();
      BaseTimerRecorder(const BaseTimerRecorder&) = delete;
      BaseTimerRecorder(BaseTimerRecorder&&) = delete;
      void operator=(const BaseTimerRecorder&) = delete;
      void operator=(BaseTimerRecorder&&) = delete;
      virtual ~BaseTimerRecorder();
      void append_timer_record(const std::shared_ptr<TimerRecord>&);
      void flush();
      virtual void pre_append_timer_record() {}
      virtual void post_append_timer_record() {}
      virtual void append_timer_record_impl(const std::shared_ptr<TimerRecord>&) = 0;
      virtual void flush_impl() = 0;
      uint32_t records_to_flush();
      uint32_t total_records();
    private:
      std::atomic_uint32_t m_records_to_flush;
    protected:
      std::atomic_uint32_t m_total_records;

    };

    class FileTimerRecorder : public BaseTimerRecorder {
    public:
      FileTimerRecorder(const std::string&);
      FileTimerRecorder(std::string&&);
      virtual ~FileTimerRecorder() override;
      virtual void append_timer_record_impl(const std::shared_ptr<TimerRecord>&) = 0;
      virtual void flush_impl() override;
    protected:
      std::string m_file_name;
      std::ostringstream m_string_stream;
      std::ofstream m_output_file;
    };

    class PlainTextTimerRecorder : public FileTimerRecorder {
    public:
      PlainTextTimerRecorder(const std::string&);
      PlainTextTimerRecorder(std::string&&);
      virtual ~PlainTextTimerRecorder() override;
      virtual void append_timer_record_impl(const std::shared_ptr<TimerRecord>&) override;
    };

    class JsonTimerRecorder : public FileTimerRecorder {
    public:
      JsonTimerRecorder(const std::string&);
      JsonTimerRecorder(std::string&&);
      virtual ~JsonTimerRecorder() override;
      virtual void append_timer_record_impl(const std::shared_ptr<TimerRecord>&) override;
      virtual void pre_append_timer_record() override;
      virtual void post_append_timer_record() override;
    private:
      void append_message_lookup();
    };

    class TimerProfiler {
    public:
      static TimerProfiler &get_instance();
      TimerProfiler(const TimerProfiler&) = delete;
      TimerProfiler(TimerProfiler&&) = delete;
      void operator=(const TimerProfiler&) = delete;
      void operator=(TimerProfiler&&) = delete;
      void register_timer_recorder(std::unique_ptr<BaseTimerRecorder>);
      void start();
      void stop();
      void process();
      void record_one(const std::shared_ptr<TimerRecord>&);
      void record_one(std::shared_ptr<TimerRecord>&&);
    private:
      TimerProfiler();
      virtual ~TimerProfiler();
      std::unique_ptr<BaseTimerRecorder> m_recorder;
      util::mt::WaitQueue<std::shared_ptr<TimerRecord>> m_record_queue;
      std::atomic_bool m_stop;
      std::thread m_thread;
    };

    enum ProfileRecordType {
      ProfileRecordTypeUnknown = 0,
      ProfileRecordTypeJson,
      ProfileRecordTypePlain
    };

    class ProfileGuard {
    public:
      ProfileGuard(const ProfileRecordType, const std::string&);
      ProfileGuard(const ProfileGuard&) = delete;
      ProfileGuard(ProfileGuard&&) = delete;
      ProfileGuard& operator = (const ProfileGuard&) = delete;
      ProfileGuard& operator = (ProfileGuard&&) = delete;
      ~ProfileGuard();
    private:
      std::unique_ptr<util::profile::BaseTimerRecorder> m_recorder;
    };

  }
}

#include "profile.inl"
