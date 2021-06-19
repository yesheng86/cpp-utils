#pragma once
#define DEFAULT_ALIGNMENT 0x40
#define DEFAULT_SIZE 0x4000

using namespace util::socket;

inline OnReadDelegate::OnReadDelegate()
  : OnReadDelegate(DEFAULT_ALIGNMENT, DEFAULT_SIZE) {}

inline OnReadDelegate::OnReadDelegate(uint32_t size)
  : OnReadDelegate(DEFAULT_ALIGNMENT, size) {}

inline OnReadDelegate::OnReadDelegate(uint32_t alignment, uint32_t size)
  : m_write_data_func(nullptr),
    m_buffer_alignment(alignment),
    m_buffer_size(size) {}

inline void OnReadDelegate::set_read_buffer_size(uint32_t size) {
  set_read_buffer_alignment_and_size(DEFAULT_ALIGNMENT, size);
}

inline void OnReadDelegate::set_read_buffer_alignment_and_size
(uint32_t alignment, uint32_t size) {
  m_buffer_alignment = alignment;
  m_buffer_size = size;
}

inline uint32_t OnReadDelegate::alignment() {
  return m_buffer_alignment;
}

inline uint32_t OnReadDelegate::size() {
  return m_buffer_size;
}

inline void OnReadDelegate::register_write_data_function
(const std::function<void (const BufferDescriptorPtr&)>& write_data_func) {
  m_write_data_func = write_data_func;
}

inline void OnReadDelegate::write_data(const BufferDescriptorPtr& buf_desc_ptr) {
  if (m_write_data_func) {
    m_write_data_func(buf_desc_ptr);
  }
}
