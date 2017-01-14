#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <memory>
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <map>
#include "../coder/coder.hpp"


class Huffman : public Coder
{
private:
	int max;
	struct node;
	using shared_ptr_huff = std::shared_ptr<node>; // containers
	struct node // stores nodes in containers
	{
		uint8_t color; // RGB value
		std::string code; // binary representation 
		int frequency; // color
		shared_ptr_huff left; // child 
		shared_ptr_huff right; // child

		node(uint8_t color, int frequency);
	};

	struct compare // operator overloading comparisons (growing)
	{
		bool operator()(const shared_ptr_huff& lhs, const shared_ptr_huff& rhs) const
		{
			return lhs->frequency > rhs->frequency;
		} // increasing
	};

	//						color	huffman code
	std::vector<std::pair<uint8_t, std::string>> vecHuffman; // to encode :)
	using queue_huff = std::priority_queue<shared_ptr_huff, std::vector<shared_ptr_huff>, compare>;

	queue_huff huffmanQueue;
	inline void writeCode(bool&, short&, unsigned char&);
	std::string decToBin(unsigned short n);
	void makeTree(int*);
	void returnCode(const shared_ptr_huff& root);

	// ENCODE
	void saveEncode(std::string, std::string);

	// hold huffman code
	std::vector<bool> codeRepresentation;
public:
	Huffman();
	~Huffman() = default;

	void encode(const std::string& filename, const bool& grayscale) override;
	void decode(const std::string& filename) override;
};

#endif HUFFMAN_H
