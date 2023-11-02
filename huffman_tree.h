#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>

class huffman_node
{
private:
	uint64_t freq_ = 0;
	huffman_node* left_node_{}, * right_node_{};

public:
	huffman_node(const uint64_t frequency, huffman_node* left_child, huffman_node* right_child) : freq_(frequency), left_node_(left_child), right_node_(right_child) {}
	virtual ~huffman_node() = default;
	uint64_t get_frequency() const { return this->freq_; }

	huffman_node* get_left_child() const { return this->left_node_; }
	huffman_node* get_right_child() const { return this->right_node_; }
};

class huffman_leaf : public huffman_node
{
private:
	uint8_t value_ = 0;
public:
	huffman_leaf(const uint64_t frequency, const uint8_t value) : huffman_node(frequency, nullptr, nullptr), value_(value) {}

	uint8_t get_value() const { return this->value_; }
};


class huffman_tree
{
private:
	huffman_node* tree_root_ = nullptr;
	const std::unordered_map<uint8_t, uint64_t> chars_freq_;
public:
	explicit huffman_tree(std::unordered_map<uint8_t, uint64_t> chars_freq);
	~huffman_tree();

	std::unordered_map<uint8_t, std::vector<bool>> calculate_codes() const;
};

