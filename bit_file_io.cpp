#include "bit_file_io.h"

void bit_file_io::write_bit(uint8_t bit)
{
	if (bit)
		this->w_bit_buf_ |= 1 << ((CHAR_BIT - 1) - this->w_bit_buf_size_);
	this->w_bit_buf_size_++;

	if (this->w_bit_buf_size_ == CHAR_BIT)
		this->flush_bit_buffer();
}

uint8_t bit_file_io::flush_bit_buffer()
{
	const uint8_t flushed_bits = w_bit_buf_size_;
	this->file_stream_.write(reinterpret_cast<char*>(&this->w_bit_buf_), 1);
	this->w_bit_buf_size_ = 0;
	this->w_bit_buf_ = 0;
	return flushed_bits;
}

bool bit_file_io::read_bit(uint8_t& bit)
{
	if(this->r_bit_buf_size_ == 0)
	{
		auto x = this->file_stream_.good();

		if(!this->file_stream_.read(reinterpret_cast<char*>(&this->r_bit_buf_), 1))
			return false;
		this->r_bit_buf_size_ = 8;
	}

	this->r_bit_buf_size_--;
	bit = r_bit_buf_ & (1 << this->r_bit_buf_size_);
	return true;
}
