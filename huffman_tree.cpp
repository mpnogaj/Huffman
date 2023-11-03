#include "huffman_tree.h"

#include <queue>
#include <utility>

static void delete_tree(const huffman_node *root);
static void fill_codes(huffman_node *root, std::unordered_map<uint8_t, std::vector<bool>>& codes,
                       std::vector<bool> current);
static huffman_leaf* get_left_most_leaf(huffman_node* node);


huffman_tree::huffman_tree(std::unordered_map<uint8_t, uint64_t> chars_freq) : chars_freq_(std::move(chars_freq))
{
	//special case
	if(this->chars_freq_.size() == 1)
	{
		const auto& [chr, frq] = *this->chars_freq_.begin();
		this->tree_root_ = new huffman_node(frq, new huffman_leaf(frq, chr), nullptr);
		return;
	}

	//https://stackoverflow.com/a/5808171
	//min heap comparator
	auto comp = [](huffman_node *n1, huffman_node *n2) {
		const uint64_t freq1 = n1->get_frequency();
		const uint64_t freq2 = n2->get_frequency();
		
		//if booth of nodes have equal frequency and are leafs, choose one with smaller value
		if(freq1 == freq2)
		{
			const huffman_leaf *leaf1 = dynamic_cast<huffman_leaf*>(n1), *leaf2 = dynamic_cast<huffman_leaf*>(n2);
			if(leaf1 != nullptr && leaf2 != nullptr)
			{
				return leaf1->get_value() > leaf2->get_value();
			}
			else if(leaf1 != nullptr)
			{
				return false;
			}
			else if(leaf2 != nullptr)
			{
				return true;
			}
			else
			{
				leaf1 = get_left_most_leaf(n1);
				leaf2 = get_left_most_leaf(n2);
				return leaf1->get_value() > leaf2->get_value();
			}
		}
		return freq1 > freq2;
	};
	std::priority_queue<huffman_node*, std::vector<huffman_node*>, decltype(comp)> pq(comp);

	for (const auto& [chr, freq] : this->chars_freq_)
	{
		pq.push(new huffman_leaf(freq, chr));
	}

	while (pq.size() > 1)
	{
		auto node1 = pq.top();
		pq.pop();
		auto node2 = pq.top();
		pq.pop();

		auto parent = new huffman_node(node1->get_frequency() + node2->get_frequency(), node1, node2);
		pq.push(parent);
	}

	tree_root_ = pq.top();
	pq.pop();
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

bool huffman_tree::try_get_byte(uint8_t& byte, uint8_t code_bit) const
{
	static huffman_node* current_node = this->tree_root_;
	if (code_bit)
		current_node = current_node->get_right_child();
	else
		current_node = current_node->get_left_child();

	if(const auto leaf = dynamic_cast<huffman_leaf*>(current_node))
	{
		byte = leaf->get_value();
		current_node = this->tree_root_;
		return true;
	}
	return false;
}

static void fill_codes(huffman_node *root, std::unordered_map<uint8_t, std::vector<bool>>& codes,
                       std::vector<bool> current)
{
	if (auto leaf = dynamic_cast<huffman_leaf*>(root))
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

static huffman_leaf* get_left_most_leaf(huffman_node* node)
{
	huffman_leaf *leaf = nullptr; 
	while((leaf = dynamic_cast<huffman_leaf*>(node)) == nullptr)
	{
		node = node->get_left_child();
	}
	return leaf;
}