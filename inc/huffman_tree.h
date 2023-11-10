#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>


/**
 * @brief Helper class for storing byte frequency`
 */
class freq_map
{
private:
	std::vector<uint64_t> freq_;

public:
	freq_map(): freq_(std::vector<uint64_t>(UINT8_MAX + 1, 0)) {}

	/**
	 * @brief Gets count of the given byte
	 * 
	 * @param byte byte
	 * @return uint64_t byte count
	 */
	uint64_t get(uint8_t byte) const { return freq_[byte]; }
	
	/**
	 * @brief Sets byte count to given value
	 * 
	 * @param byte byte 
	 * @param value count
	 */
	void set(uint8_t byte, uint64_t value) {freq_[byte] = value; }
	
	/**
	 * @brief Increments byte count
	 * 
	 * @param byte byte
	 */
	void inc(uint8_t byte) { ++freq_[byte]; }

	/**
	 * @brief Gets count of stored bytes (unique bytes count)
	 * 
	 * @return uint16_t count
	 */
	uint16_t size() const
	{
		uint16_t cnt = 0;
		for(uint16_t i = 0; i <= UINT8_MAX; i++)
			if(freq_[i])
				cnt++;
		return cnt;
	};
};

/**
 * @brief Huffman tree node. Containing frequency info
 */
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

/**
 * @brief Huffman tree leaf. Contains frequency and stored value
 */
class huffman_leaf : public huffman_node
{
private:
	uint8_t value_ = 0;
public:
	huffman_leaf(const uint64_t frequency, const uint8_t value) : huffman_node(frequency, nullptr, nullptr), value_(value) {}

	uint8_t get_value() const { return this->value_; }
};

/**
 * @brief Huffman tree representation
 */
class huffman_tree
{
private:
	huffman_node* tree_root_ = nullptr;
	const freq_map& chars_freq_;
	std::vector<uint8_t> *codes_;
	void fill_codes(huffman_node* root, std::vector<uint8_t> current);

public:
	huffman_tree(const freq_map& chars_freq);
	~huffman_tree();

	const std::vector<uint8_t>* get_codes() const { return this->codes_; }
	bool try_get_byte(uint8_t &byte, uint8_t code_bit) const;
};

