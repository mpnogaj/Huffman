#pragma once

#include "argsParser.h"



class huffman_encoder
{
private:
	static constexpr size_t buffer_size_ = 16777216; //16mb
	uint8_t* buffer_;

	std::string input_file_, output_file_;

public:
	huffman_encoder(const program_options& program_options) : buffer_(new uint8_t[buffer_size_]),
	                                                          input_file_(program_options.get_input_file_path()),
	                                                          output_file_(program_options.get_output_file_path())
	{
	}

	~huffman_encoder() {delete buffer_;}

	void compress_file() const;
	void decompress_file() const;
};