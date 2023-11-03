#include <functional>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "argsParser.h"
#include "huffman_tree.h"
#include "huffman_encoder.h"

class option
{
private:
	const std::string short_name_, long_name_;
	const std::string description_;

	std::function<void(int&)> function_;

public:
	option(std::string short_name, std::string long_name, std::string description, std::function<void(int&)> function) :
		short_name_(std::move(short_name)), long_name_(std::move(long_name)), description_(std::move(description)),
		function_(
			std::move(function))
	{
	}

	const std::string& get_short_name() const { return this->short_name_; }
	const std::string& get_long_name() const { return this->long_name_; }
	const std::string& get_description() const { return this->description_; }
	std::function<void(int&)> get_function() const { return this->function_; }

	bool matches(const std::string& candidate) const
	{
		return this->get_long_name() == candidate || this->get_short_name() == candidate;
	}
};

static void invalid_usage(const std::string& program_name)
{
	std::cout << "Invalid arguments. Run: " << program_name << " --help for help" << std::endl;
	exit(EXIT_FAILURE);
}

static void app_error(const std::string& msg)
{
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	const std::string program_name = argv[0];
	std::string input_file, output_file;
	auto mode = mode::INVALID;

	std::vector<option> options{
		option("-h", "--help", "Prints help", [program_name, &options](int& i) {
			std::cout << "Running: " << program_name << " [OPTIONS] " << mode_compress << "|" << mode_decompress <<
				std::endl;
			std::cout << "Options: " << std::endl;
			for (const auto& option : options)
				std::cout << "  " << option.get_short_name() << ", " << option.get_long_name() << " - " << option.
					get_description() << std::endl;
			exit(EXIT_SUCCESS);
		}),
		option("-i", "--input-file", "Input file path [required]", [argc, argv, &input_file](int& i) {
			if (i + 1 >= argc)
				app_error("Input file not specified");
			input_file = std::string(argv[i + 1]);
			i++;
		}),
		option("-o", "--output-file", "Output file path [required]", [argc, argv, &output_file](int& i) {
			if (i + 1 >= argc)
				app_error("Output file not specified");
			output_file = std::string(argv[i + 1]);
			i++;
		}),
		option("-m", "--mode", "Compression algorithm mode [required]", [argc, argv, &mode](int& i) {
			if (i + 1 >= argc)
				app_error("Mode not specified");
			if (argv[i + 1] == mode_compress)
				mode = mode::COMPRESS;
			else if (argv[i + 1] == mode_decompress)
				mode = mode::DECOMPRESS;
			i++;
		})
	};

	/*if (argc < 2)
		invalid_usage(program_name);

	for (int i = 1; i < argc; i++)
		for (const auto& option : options)
			if (option.matches(argv[i]))
				option.get_function()(i);

	if (mode == mode::INVALID)
		invalid_usage(program_name);*/

	mode = mode::DECOMPRESS;

	auto program_opt2 = program_options("vid.mp4", "out.mp4", mode::COMPRESS);
	auto program_opt = program_options("out.mp4", "res.mp4", mode::COMPRESS);
	auto encoder = huffman_encoder(program_opt);
	auto encoder2 = huffman_encoder(program_opt2);

	encoder2.compress_file();
	encoder.decompress_file();

	return EXIT_SUCCESS;
}
