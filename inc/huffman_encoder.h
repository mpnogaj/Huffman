#pragma once

#include <cstdint>
#include <string>

#include "consts.h"
#include "ui.h"

class huffman_encoder
{
private:
	std::string input_file_, output_file_;
	const ui& ui_;


	uint8_t* const buffer_;
	const size_t buffer_size_;
	size_t buffer_cnt_ = 0;
public:
	huffman_encoder(std::string input_file, std::string output_file, const ui& ui, const size_t buffer_size = size_16_mb);
	huffman_encoder(const huffman_encoder& cpy);
	~huffman_encoder();

	void compress_file();
	void decompress_file();
};
