#pragma once

#include "argsParser.h"

class huffman_encoder
{
private:
	std::string input_file_, output_file_;

public:
	huffman_encoder(const program_options& program_options) : input_file_(program_options.get_input_file_path()),
	                                                          output_file_(program_options.get_output_file_path())
	{
	};

	void compress_file() const;
	void decompress_file() const;
};