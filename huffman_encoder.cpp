#include "huffman_encoder.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include "huffman_tree.h"
#include "bit_file_io.h"

static void write_file_header(const std::unordered_map<uint8_t, uint64_t> &freq_map, std::fstream &output_file);
static std::pair<huffman_tree*, uint8_t> read_file_header(std::fstream &file);

void huffman_encoder::compress_file() const
{
	std::cout<<"Compression started" << std::endl;
	std::fstream input_file(this->input_file_, std::ios::in | std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out | std::ios_base::binary);

	if (input_file.peek() == std::ifstream::traits_type::eof())
	{
		input_file.close();
		output_file.close();
		return;
	}

	bit_file_io output_file_bit_io(output_file);

	std::cout<<"Counting byte frequency..."<<std::endl;
	std::unordered_map<uint8_t, uint64_t> freq_map;
	//https://stackoverflow.com/a/67854635
	while (input_file.read(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_size_))
	{
		for(size_t i = 0; i < static_cast<size_t>(input_file.gcount()); i++)
		{
			uint8_t byte = this->buffer_[i];
			if (freq_map.count(byte)) freq_map[byte]++;
			else freq_map[byte] = 1;
		}
	}
	
	std::cout<<"Finished counting"<<std::endl;
	write_file_header(freq_map, output_file);

	input_file.clear(); //clear eof flag
	input_file.seekg(0, std::ios_base::beg);

	std::cout<<"Building huffman tree..."<<std::endl;
	const huffman_tree *tree = new huffman_tree(freq_map);
	auto codes = tree->calculate_codes();
	std::cout<<"Tree created, and codes generated"<<std::endl;

	std::cout<<"Transforming bytes..." << std::endl;
	while (input_file.read(reinterpret_cast<char*>(this->buffer_), sizeof(uint8_t) * this->buffer_size_))
	{
		for(size_t i = 0; i < static_cast<size_t>(input_file.gcount()); i++)
		{
			uint8_t byte = this->buffer_[i];
			auto code = codes[byte];
			for (bool bit : code)
			{
				output_file_bit_io.write_bit(bit ? 1 : 0);
			}
		}
	}

	//fill missing info about bit cnt in last byte
	if (uint8_t flushed_bits = output_file_bit_io.flush_bit_buffer())
	{
		output_file.clear();
		output_file.seekg(1, std::ios_base::beg);
		output_file.write(reinterpret_cast<char*>(&flushed_bits), 1);
	}

	std::cout<<"Compression finished" << std::endl;

	input_file.close();
	output_file.close();

	delete tree;
}

void huffman_encoder::decompress_file() const
{
	std::fstream input_file(this->input_file_, std::ios::in | std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out | std::ios_base::binary);
	bit_file_io input_file_bit_io(input_file);

	const auto [tree, last_byte_bits] = read_file_header(input_file);
	auto codes = tree->calculate_codes();


	uint8_t bit = 0, byte = 0;
	uint8_t bits_cnt = 0;
	while (input_file_bit_io.read_bit(bit))
	{
		if (tree->try_get_byte(byte, bit))
			output_file.write(reinterpret_cast<char*>(&byte), 1);

		bits_cnt++;

		//discard last end_padding bits
		if (input_file.peek() == std::ifstream::traits_type::eof())
			if (bits_cnt == last_byte_bits)
				break;

		bits_cnt %= 8;
	}

	input_file.close();
	output_file.close();

	delete tree;
}

static void write_file_header(const std::unordered_map<uint8_t, uint64_t> &freq_map, std::fstream &output_file)
{
	uint8_t buf[2] = {
		static_cast<uint8_t>(freq_map.size() - 1), //bytes_size + 1 = unique bytes count;
		0 // placeholder
	};

	output_file.write(reinterpret_cast<char*>(&buf), sizeof(buf));

	std::vector<std::pair<uint8_t, uint64_t>> elems(freq_map.begin(), freq_map.end());

	std::sort(elems.begin(), elems.end(),
	          [](const std::pair<uint8_t, uint64_t> &p1, const std::pair<uint8_t, uint64_t> &p2) {
		          if (p1.second == p2.second)
			          return p1.first < p2.first;

		          return p1.second > p2.second;
	          });

	for (auto &[chr, frq] : elems)
	{
		output_file.write(reinterpret_cast<char*>(&chr), sizeof(uint8_t));
		output_file.write(reinterpret_cast<char*>(&frq), sizeof(uint64_t));
	}
}

static std::pair<huffman_tree*, uint8_t> read_file_header(std::fstream &file)
{
	uint8_t header[2];
	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	const uint16_t unique_bytes = static_cast<uint16_t>(header[0]) + 1;
	freq_map_t byte_freq;
	uint8_t byte = 0;
	uint64_t count = 0;

	uint16_t bytes_read = 2;

	while (bytes_read < ((unique_bytes * 9) + 2) && file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)) && file.read(reinterpret_cast<char*>(&count), sizeof(uint64_t)))
	{
		bytes_read += 9;
		byte_freq[byte] = count;
	}

	return {new huffman_tree(byte_freq), header[1]};
}
