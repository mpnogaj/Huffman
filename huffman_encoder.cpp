#include "huffman_encoder.h"

#include <algorithm>
#include <fstream>
#include <unordered_map>
#include "huffman_tree.h"
#include "bit_file_io.h"

static void write_file_header(const std::unordered_map<uint8_t, uint64_t>& freq_map, std::fstream& output_file);
static huffman_tree* read_file_header(std::fstream& file);

void huffman_encoder::compress_file() const
{
	std::fstream input_file(this->input_file_, std::ios::in, std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out, std::ios_base::binary);
	bit_file_io output_file_bit_io(output_file);

	std::unordered_map<uint8_t, uint64_t> freq_map;

	//https://stackoverflow.com/a/67854635
	uint8_t byte;
	while(input_file.read(reinterpret_cast<char *>(&byte), sizeof(uint8_t)))
	{
		if(freq_map.count(byte)) freq_map[byte]++;
		else freq_map[byte] = 1;
	}

	write_file_header(freq_map, output_file);

	input_file.clear(); //clear eof flag
	input_file.seekg(0, std::ios_base::beg);

	const huffman_tree *tree = new huffman_tree(freq_map);
	auto codes = tree->calculate_codes();

	uint8_t bits_in_buffer = 0;

	while (input_file.read(reinterpret_cast<char*>(&byte), sizeof(uint8_t)))
	{
		auto code = codes[byte];
		for (bool bit : code)
		{
			output_file_bit_io.write_bit(bit ? 1 : 0);
		}
	}
	output_file_bit_io.flush_bit_buffer();

	input_file.close();
	output_file.close();

	delete tree;
}

void huffman_encoder::decompress_file() const
{
	std::fstream input_file(this->input_file_, std::ios::in, std::ios_base::binary);
	std::fstream output_file(this->output_file_, std::ios::out, std::ios_base::binary);
	bit_file_io input_file_bit_io(input_file);

	huffman_tree* tree = read_file_header(input_file);

	uint8_t bit = 0, byte = 0;
	while(input_file_bit_io.read_bit(bit))
	{
		if (tree->try_get_byte(byte, bit))
			output_file.write(reinterpret_cast<char*>(&byte), 1);
	}

	input_file.close();
	output_file.close();

	delete tree;
}

static void write_file_header(const std::unordered_map<uint8_t, uint64_t>& freq_map, std::fstream& output_file)
{
	uint16_t header_size = static_cast<uint16_t>(freq_map.size()) + static_cast<uint16_t>(sizeof(header_size)); //unique bytes + header_size

	output_file.write(reinterpret_cast<char *>(&header_size), sizeof(header_size));

	std::vector<std::pair<uint8_t, uint64_t>> elems(freq_map.begin(), freq_map.end());
	
	std::sort(elems.begin(), elems.end(), [](const std::pair<uint8_t, uint64_t>& p1, const std::pair<uint8_t, uint64_t>& p2) {
		if (p1.second == p2.second)
			return p1.first < p2.first;

		return p1.second > p2.second;
	});

	for (auto& [chr, frq] : elems)
	{
		output_file.write(reinterpret_cast<char *>(&chr), 1);
	}
}

static huffman_tree* read_file_header(std::fstream &file)
{
	uint16_t header_size;
	file.read(reinterpret_cast<char *>(&header_size), sizeof(uint16_t));
	uint16_t artificial_count = header_size - 1;
	freq_map_t byte_freq;
	uint8_t byte;
	while(file.tellg() < header_size && file.read(reinterpret_cast<char *>(&byte), sizeof(uint8_t)))
	{
		byte_freq[byte] = artificial_count;
		artificial_count--;
	}

	return new huffman_tree(byte_freq);
}

