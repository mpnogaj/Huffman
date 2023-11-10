#pragma once

#include <cstdint>
#include <fstream>

/**
 * @brief Wrapper around std::fstream for writing/reading bits to/from file
 */
class bit_file_io
{
  private:
    std::fstream &file_stream_;
    // buffers for bit manipulation
    uint8_t r_bit_buf_ = 0, r_bit_buf_size_ = 0;
    uint8_t w_bit_buf_ = 0, w_bit_buf_size_ = 0;

    // buffers for reading/writing from/to file
    uint8_t *r_buff_;
    size_t r_buff_cnt_ = 0, r_buff_size_;
    uint8_t *w_buff_;
    size_t w_buff_cnt_ = 0, w_buff_size_;

  public:
    bit_file_io(std::fstream &file_stream, size_t read_buff_size,
                size_t write_buff_size);
    ~bit_file_io();

	/**
	 * @brief Writes single bit, bit & 1
	 * @param bit bit
	 */
    void write_bit(uint8_t bit);

	/**
	 * @brief Causes bit buffer to be flushed
	 */
    void flush_bit_buffer();

	/**
	 * @brief Flushes buffer to file
	 */
    void flush_buffer();

	/**
	 * @brief Reads single bit from file
	 * @param bit read bit
	 * @return true when bit is read
	 * @return false when bit is not read
	 */
    bool read_bit(uint8_t &bit);

    bit_file_io &operator<<(const uint8_t bit);
    bool operator>>(uint8_t &bit);
};
