#pragma once

#include <fstream>

class bit_file_io
{
private:
	std::fstream &file_stream_;
	//buffers for bit manipulation
	uint8_t r_bit_buf_ = 0, r_bit_buf_size_ = 0;
	uint8_t w_bit_buf_ = 0, w_bit_buf_size_ = 0;
	
	//buffers for reading/writing from/to file
	uint8_t *r_buff_;
	size_t r_buff_cnt_ = 0, r_buff_size_;
	uint8_t *w_buff_;
	size_t w_buff_cnt_ = 0, w_buff_size_;

public:
	bit_file_io(std::fstream &file_stream, size_t read_buff_size, size_t write_buff_size);
	~bit_file_io();

	void write_bit(uint8_t bit);
	void flush_bit_buffer();
	void flush_buffer();

	bool read_bit(uint8_t &bit);
};
