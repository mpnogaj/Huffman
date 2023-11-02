#include "huffman_tree.h"

#include <queue>
#include <utility>

static void delete_tree(const huffman_node* root);
static void fill_codes(huffman_node* root, std::unordered_map<uint8_t, std::vector<bool>>& codes, std::vector<bool> current);


huffman_tree::huffman_tree(std::unordered_map<uint8_t, uint64_t> chars_freq) : chars_freq_(std::move(chars_freq))
{
	std::priority_queue<std::pair<uint64_t, uint8_t>, std::vector<std::pair<uint64_t, uint8_t>>, std::greater<>> pq;
	for (const auto& [chr, freq] : this->chars_freq_)
	{
		pq.push({ freq, chr });
	}

	while (!pq.empty())
	{
		const auto& [frq, chr] = pq.top();
		if (tree_root_ == nullptr)
		{
			tree_root_ = new huffman_leaf(frq, chr);
		}
		else
		{
			auto* new_node = new huffman_leaf(frq, chr);
			huffman_node* l_child = new_node, * r_child = tree_root_;
			if (l_child->get_frequency() > r_child->get_frequency())
				std::swap(l_child, r_child);

			tree_root_ = new huffman_node(l_child->get_frequency() + r_child->get_frequency(), l_child, r_child);
		}
		pq.pop();
	}
}

huffman_tree::~huffman_tree()
{
	delete_tree(this->tree_root_);
}

std::unordered_map<uint8_t, std::vector<bool>> huffman_tree::calculate_codes() const
{
	std::unordered_map<uint8_t, std::vector<bool>> codes;
	fill_codes(this->tree_root_, codes, {});
	return codes;
}

static void fill_codes(huffman_node *root, std::unordered_map<uint8_t, std::vector<bool>>& codes, std::vector<bool> current)
{
	if(auto leaf = dynamic_cast<huffman_leaf*>(root))
	{
		codes[leaf->get_value()] = std::move(current);
		return;
	}
	else
	{
		auto cpy = std::vector<bool>(current);
		current.push_back(false);
		fill_codes(root->get_left_child(), codes, current);
		cpy.push_back(true);
		fill_codes(root->get_right_child(), codes, cpy);
	}
}

static void delete_tree(const huffman_node *root)
{
	if (root == nullptr) return;
	if (root->get_left_child() != nullptr) delete_tree(root->get_left_child());
	if (root->get_right_child() != nullptr) delete_tree(root->get_right_child());
	delete root;
}

