#include "huffman_encoder.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "huffman_tree.h"
#include "bit_file_io.h"

static void write_file_header(const freq_map &map, uint8_t padding, std::fstream &output_file);
static huffman_tree* read_file_header(std::fstream &file);

void huffman_encoder::compress_file()
{
	std::cout << "Compression started" << std::endl;
	std::fstream input_file(this->input_file_, std::ios::in | std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out | std::ios_base::binary);

	if (input_file.peek() == std::ifstream::traits_type::eof())
	{
		input_file.close();
		output_file.close();
		return;
	}

	bit_file_io output_file_bit_io(output_file, 1, size_16_mb);

	std::cout << "Counting byte frequency..." << std::endl;
	freq_map map;
	//https://stackoverflow.com/a/67854635

	while (input_file.good())
	{
		input_file.read(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_size_);
		this->buffer_cnt_ = static_cast<size_t>(input_file.gcount());
		for (size_t i = 0; i < buffer_cnt_; i++)
		{
			uint8_t byte = this->buffer_[i];
			map.inc(byte);
		}
	}
	
	std::cout << "Finished counting" << std::endl;

	std::cout << "Building huffman tree..." << std::endl;
	const huffman_tree *tree = new huffman_tree(map);
	auto codes = tree->get_codes();

	uint8_t padding = 0;

	for (uint16_t i = 0; i <= UINT8_MAX; i++)
		padding = (padding + ((codes[i].size() % CHAR_BIT) * (map.get(i) % CHAR_BIT)) % CHAR_BIT) % CHAR_BIT;

	std::cout << "Tree created, and codes generated" << std::endl;

	write_file_header(map, CHAR_BIT - padding, output_file);

	input_file.clear(); //clear eof flag
	input_file.seekg(0, std::ios_base::beg);


	std::cout << "Transforming bytes..." << std::endl;
	while (input_file.good())
	{
		input_file.read(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_size_);
		this->buffer_cnt_ = static_cast<size_t>(input_file.gcount());
		for (size_t i = 0; i < this->buffer_cnt_; i++)
		{
			uint8_t byte = this->buffer_[i];
			auto& code = codes[byte];
			for (bool bit : code)
			{
				output_file_bit_io.write_bit(bit);
			}
		}
	}

	output_file_bit_io.flush_bit_buffer();
	output_file_bit_io.flush_buffer();

	std::cout << "Compression finished" << std::endl;

	input_file.close();
	output_file.close();

	delete tree;
}

void huffman_encoder::decompress_file()
{
	std::fstream input_file(this->input_file_, std::ios::in | std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out | std::ios_base::binary);
	bit_file_io input_file_bit_io(input_file, size_16_mb, 1);

	const auto tree = read_file_header(input_file);

	uint8_t bit = 0, byte = 0;
	while (input_file_bit_io.read_bit(bit))
	{
		if (tree->try_get_byte(byte, bit))
		{
			this->buffer_[this->buffer_cnt_++] = byte;
			if (this->buffer_cnt_ == this->buffer_size_)
			{
				output_file.write(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_cnt_);
				this->buffer_cnt_ = 0;
			}
		}
	}

	if (buffer_cnt_ > 0)
	{
		output_file.write(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_cnt_);
		this->buffer_cnt_ = 0;
	}

	input_file.close();
	output_file.close();

	delete tree;
}

static void write_file_header(const freq_map &map, uint8_t padding, std::fstream &output_file)
{
	uint8_t buf[2] = {
		static_cast<uint8_t>(map.size() - 1), //bytes_size + 1 = unique bytes count;
		padding // needed padding
	};

	output_file.write(reinterpret_cast<char*>(&buf), sizeof(buf));

	for (uint16_t chr = 0; chr <= UINT8_MAX; chr++)
	{
		if (uint64_t frq = map.get(chr))
		{
			output_file.write(reinterpret_cast<char*>(&chr), sizeof(uint8_t));
			output_file.write(reinterpret_cast<char*>(&frq), sizeof(uint64_t));
		}
	}
}

static huffman_tree* read_file_header(std::fstream &file)
{
	uint8_t header[2];
	//header[0] -> num of unique bytes - 1
	//header[1] -> padding at the end
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	const uint16_t unique_bytes = static_cast<uint16_t>(header[0]) + 1;
	freq_map map;
	uint8_t byte = 0;
	uint64_t count = 0;
	uint16_t bytes_read = 2;

	while (bytes_read < ((unique_bytes * 9) + 2) && file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)) && file.
		read(reinterpret_cast<char*>(&count), sizeof(uint64_t)))
	{
		bytes_read += 9;
		map.set(byte, count);
	}

	return new huffman_tree(map);
}
