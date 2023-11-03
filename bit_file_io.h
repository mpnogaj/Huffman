#pragma once

#include <fstream>

class bit_file_io
{
private:
	std::fstream &file_stream_;
	uint8_t w_bit_buf_ = 0, w_bit_buf_size_ = 0;
	uint8_t r_bit_buf_ = 0, r_bit_buf_size_ = 0;
public:
	bit_file_io(std::fstream &file_stream) : file_stream_(file_stream) {  }

	void write_bit(uint8_t bit);
	uint8_t flush_bit_buffer();

	bool read_bit(uint8_t &bit);
};
