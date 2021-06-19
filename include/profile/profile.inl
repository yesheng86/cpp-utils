#pragma once
#include <chrono>
#include <iostream>
#include "profile.hpp"
#include "memorypool/objectpool.hpp"
#include "singleton/singleton.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace util::profile;

inline Timer::Timer(const std::string& message)
  : m_message(message), m_cancel(false) {
  m_start = high_resolution_clock::now();
  m_message_id = MessageLookup::get_instance().register_message(m_message);
  m_category = 0;
}

inline Timer::Timer(std::string&& message)
  : m_message(std::move(message)), m_cancel(false) {
  m_start = high_resolution_clock::now();
  m_message_id = MessageLookup::get_instance().register_message(m_message);
  m_category = 0;
}

inline Timer::Timer(const std::string& message, uint32_t category)
  : Timer(message) {
  m_category = category;
}

inline Timer::Timer(std::string&& message, uint32_t category)
  : Timer(std::move(message)) {
  m_category = category;
}

inline void Timer::cancel() {
  m_cancel = true;
}

inline Timer::~Timer() {
  if (m_cancel) {
    return;
  }
  time_point<high_resolution_clock> now = high_resolution_clock::now();
  microseconds duration_in_us = duration_cast<microseconds>(now - m_start);

  std::shared_ptr<TimerRecord> record = Singleton<ObjectPool<TimerRecord>>::instance().new_one();
  record->category = m_category;
  record->duration_us = duration_in_us.count();
  record->message_id = m_message_id;
	
  TimerProfiler::get_instance().record_one(std::move(record));
}

inline MessageLookup& MessageLookup::get_instance() {
  static MessageLookup instance;
  return instance;
}

inline MessageLookup::MessageLookup()
  : m_message_map(), m_id_increaser(0) {
}

inline MessageLookup::~MessageLookup() {
  std::scoped_lock(m_mutex);
  m_message_map.clear();
}

inline uint32_t MessageLookup::register_message(const std::string& message) {
  std::scoped_lock(m_mutex);
  for (auto const&[key, val] : m_message_map)
    {
      if (message == val)
	return key;
    }
  m_message_map[m_id_increaser] = message;
  return m_id_increaser++;
}

inline uint32_t MessageLookup::register_message(std::string&& message) {
  std::scoped_lock(m_mutex);
  for (auto const&[key, val] : m_message_map)
    {
      if (message == val)
	return key;
    }
  m_message_map[m_id_increaser] = std::move(message);
  return m_id_increaser++;
}

inline std::string& MessageLookup::MessageLookup::get_message(uint32_t id) {
  std::scoped_lock(m_mutex);
  return m_message_map[id];
}

inline uint32_t MessageLookup::size() {
  std::scoped_lock(m_mutex);
  return m_id_increaser;
}

inline BaseTimerRecorder::BaseTimerRecorder() : m_records_to_flush(0), m_total_records(0) {
}

inline BaseTimerRecorder::~BaseTimerRecorder() {
}

inline void BaseTimerRecorder::append_timer_record(const std::shared_ptr<TimerRecord>& record) {
  append_timer_record_impl(record);
  m_records_to_flush++;
  m_total_records++;
}

inline void BaseTimerRecorder::flush() {
  flush_impl();
  m_records_to_flush = 0;
}

inline uint32_t BaseTimerRecorder::records_to_flush() {
  return m_records_to_flush;
}

inline uint32_t BaseTimerRecorder::total_records() {
  return m_total_records;
}

inline FileTimerRecorder::FileTimerRecorder(const std::string& file_name)
  : BaseTimerRecorder(), m_file_name(file_name), m_string_stream(), m_output_file() {
  m_output_file.open(m_file_name);
}
inline FileTimerRecorder::FileTimerRecorder(std::string&& file_name)
  : BaseTimerRecorder(), m_file_name(std::move(file_name)), m_string_stream(), m_output_file() {
  m_output_file.open(m_file_name);
}

inline void FileTimerRecorder::flush_impl() {
  m_output_file << m_string_stream.str();
  m_output_file.flush();
  m_string_stream.str(std::string());
}

inline FileTimerRecorder::~FileTimerRecorder() {
  m_output_file.close();
}

inline PlainTextTimerRecorder::PlainTextTimerRecorder(const std::string& file_name)
  : FileTimerRecorder(file_name) {
}

inline PlainTextTimerRecorder::PlainTextTimerRecorder(std::string&& file_name)
  : FileTimerRecorder(std::move(file_name)) {
}

inline void PlainTextTimerRecorder::append_timer_record_impl(const std::shared_ptr<TimerRecord>& record) {
  std::string& message = MessageLookup::get_instance().get_message(record->message_id);
  m_string_stream << "Profile:" << message << "(" << record->message_id << "), Category(" << record->category << "), Duration(" << record->duration_us << "us)\n";
}

inline PlainTextTimerRecorder::~PlainTextTimerRecorder() {
}

inline JsonTimerRecorder::JsonTimerRecorder(const std::string& file_name)
  : FileTimerRecorder(file_name) {
}


inline JsonTimerRecorder::JsonTimerRecorder(std::string&& file_name)
  : FileTimerRecorder(std::move(file_name)) {
}

inline JsonTimerRecorder::~JsonTimerRecorder() {
}

inline void JsonTimerRecorder::pre_append_timer_record() {
  m_string_stream << "{\n\t\"data\": [\n\t\t";
}

inline void JsonTimerRecorder::post_append_timer_record() {
  append_message_lookup();
  m_string_stream << " }\n";
}

inline void JsonTimerRecorder::append_timer_record_impl(const std::shared_ptr<TimerRecord>& record) {
  if (m_total_records != 0) {
    m_string_stream << ",\n\t\t";
  }
  m_string_stream << "[" << record->message_id << ", " << record->category << ", " << record->duration_us << "]";
}

inline void JsonTimerRecorder::append_message_lookup() {
  m_string_stream << "],\n\t\"message_lookup\" : [\n\t\t";
  for (uint32_t i = 0; i < MessageLookup::get_instance().size(); i++) {
    m_string_stream << "\"" << MessageLookup::get_instance().get_message(i) << "\"";
    if (i != (MessageLookup::get_instance().size() - 1))
      m_string_stream << ",\n\t\t";
  }
  m_string_stream << "]\n";
}

inline TimerProfiler& TimerProfiler::get_instance() {
  static TimerProfiler instance;
  return instance;
}

inline TimerProfiler::TimerProfiler() : m_recorder(nullptr), m_record_queue(), m_stop(true) {
}

inline TimerProfiler::~TimerProfiler() {
  std::cout << "TimerProfiler deconstruct" << std::endl;
}

inline void TimerProfiler::register_timer_recorder(std::unique_ptr<BaseTimerRecorder> recorder) {
  if (m_recorder == nullptr) {
    m_recorder = std::move(recorder);
  }
}

inline void TimerProfiler::start() {
  if (m_recorder == nullptr)
    return;
  m_stop = false;
  m_thread = std::move(std::thread([this]() {
        m_recorder->pre_append_timer_record();
        while (!m_stop) {
          process();
        }
        std::shared_ptr<TimerRecord> record;
        while (m_record_queue.try_pop(record)) {
          m_recorder->append_timer_record(record);
        }
        m_recorder->post_append_timer_record();
        m_recorder->flush();
        std::cout << "TimerProfiler thread end" << std::endl;
      }));
}

inline void TimerProfiler::stop() {
  if (m_recorder == nullptr)
    return;
  std::cout << "TimerProfiler stop" << std::endl;
  m_stop = true;
  m_record_queue.stop_queue();
  m_thread.join();
}

inline void TimerProfiler::process() {
  std::shared_ptr<TimerRecord> record;
  m_record_queue.wait_and_pop(record);
  if (m_stop)
    return;
  m_recorder->append_timer_record(record);
  if (m_recorder->records_to_flush() > 10) {
    m_recorder->flush();
  }
}

inline void TimerProfiler::record_one(const std::shared_ptr<TimerRecord>& recorder) {
  m_record_queue.push(recorder);
}

inline void TimerProfiler::record_one(std::shared_ptr<TimerRecord>&& recorder) {
  m_record_queue.push(std::move(recorder));
}


inline ProfileGuard::ProfileGuard(const ProfileRecordType type, const std::string& file) {
  switch(type) {
  case ProfileRecordTypeJson:
    m_recorder = std::make_unique<profile::JsonTimerRecorder>(file);
    break;
  case ProfileRecordTypePlain:
    m_recorder = std::make_unique<profile::PlainTextTimerRecorder>(file);
    break;
  default:
    break;
  }
  if (m_recorder) {
    TimerProfiler::get_instance().register_timer_recorder(std::move(m_recorder));
    TimerProfiler::get_instance().start();
    std::cout << "ProfileGuard start timer profile..." << std::endl;
  }
}

inline ProfileGuard::~ProfileGuard() {
  TimerProfiler::get_instance().stop();
  std::cout << "~ProfileGuard" << std::endl;
}
