#include "huffman_encoder.h"

#include <fstream>
#include <unordered_map>
#include "huffman_tree.h"

static void write_file_header(const std::unordered_map<uint8_t, uint64_t>& freq_map, const std::ofstream output_file);
static huffman_tree* read_file_header();

void huffman_encoder::compress_file()
{
	std::unordered_map<uint8_t, uint64_t> freq_map;

	//https://stackoverflow.com/a/67854635
	std::ifstream input_file("data.txt", std::ios_base::binary);
	int8_t byte;
	while(input_file.read(reinterpret_cast<char *>(&byte), sizeof(int8_t)))
	{
		if(freq_map.count(byte)) freq_map[byte]++;
		else freq_map[byte] = 1;
	}
	input_file.seekg(0, std::ios_base::beg);

	const huffman_tree *tree = new huffman_tree(freq_map);
}