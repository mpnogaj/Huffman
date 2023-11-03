#pragma once

#include "argsParser.h"
#include "consts.h"


class huffman_encoder
{
private:
	uint8_t* const buffer_;
	const size_t buffer_size_ = size_16_mb;
	size_t buffer_cnt_ = 0;

	std::string input_file_, output_file_;
	

public:
	huffman_encoder(const program_options& program_options) : buffer_(new uint8_t[size_16_mb]),
	                                                          input_file_(program_options.get_input_file_path()),
	                                                          output_file_(program_options.get_output_file_path())
	{
	}

	~huffman_encoder() {delete[] buffer_;};

	void compress_file();
	void decompress_file();
};
