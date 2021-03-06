#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <memory>	//shared_ptr
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <map>
#include <functional>
#include "../coder/coder.hpp"

////////////////////////////////////
// Class huffman
////////////////////////////////////
class Huffman : public Coder
{
public:
	////////////////////////////////////
	// Constructors huffman
	////////////////////////////////////
	Huffman();

private:
	////////////////////////////////////
	// Structs huffman
	////////////////////////////////////
	struct node; // stores nodes in containers
	struct Huff_header; // huffman header without code map

	//////////////////////////////////
	// Variables huffman
	//////////////////////////////////
	using uint = unsigned int;
	using ushort = unsigned short;
	using ulong = unsigned long;
	using ullong = unsigned long long;
	using uchar = unsigned char;
	using uint8 = uint8_t;

	std::vector<bool> code; // hold huffman code
	std::vector<bool> codeRepresentation[256]; //
	ushort size_codeRepresentation; // how many code is in vector
	using shared_ptr_huff = std::shared_ptr<node>;

	ushort size_map_code;

	// Priority_queue with huffman tree
	std::function<bool(const shared_ptr_huff&, const shared_ptr_huff&)> compare;
	using queue_huff = std::priority_queue<shared_ptr_huff, std::vector<shared_ptr_huff>, decltype(compare)>;
	queue_huff huffmanQueue;

	////////////////////////////////////
	// Methods huffman
	////////////////////////////////////
	void encode(const std::string& filename, const bool& grayscale) override;
	void decode(const std::string& filename, const bool& grayscale) override;

	// Encode methods
	void makeTree(unsigned int*); // create huffman tree
	void returnCode(const shared_ptr_huff& root); // return huffman code from tree
};

#endif HUFFMAN_H

