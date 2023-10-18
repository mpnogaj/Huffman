#pragma once

#include <string>

const std::string mode_compress = "compress";
const std::string mode_decompress = "decompress";

enum class mode
{
	INVALID = 0,
	COMPRESS,
	DECOMPRESS,
};

class program_options
{
private:
	std::string input_file_path_;
	std::string output_file_path_;
	mode mode_;


public:
	program_options(std::string input_file_path, std::string output_file_path, const mode mode) : input_file_path_(std::move(input_file_path)), output_file_path_(std::move(output_file_path)), mode_(mode) {}


	const std::string& get_input_file_path() const { return this->input_file_path_; }
	const std::string& get_output_file_path() const { return this->output_file_path_; }
	const mode& get_mode() const { return this->mode_; }
};