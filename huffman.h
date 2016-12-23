#ifndef HUFFMAN_H
#define HUFFMAN_H


#include <memory>
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <map>


class huffman
{
private:
	int max; 
	struct node;
	using shared_ptr_huff = std::shared_ptr<node>; // containers
	struct node // stores nodes in containers
	{
		uint8_t color;		// RGB value
		std::string code;		// binary representation 
		int frequency;			// color
		shared_ptr_huff left;   // child 
		shared_ptr_huff right;	// child

		node(uint8_t color, int frequency);
	};		

	struct compare // operator overloading comparisons (growing)
	{
		bool operator()(const shared_ptr_huff &l, const shared_ptr_huff &r)
		{return l->frequency > r->frequency;} // increasing
	};

	//						color	huffman code
	std::vector <std::pair<uint8_t, std::string>> vecHuffman; // to encode :)
	using queue_huff = std::priority_queue < shared_ptr_huff, std::vector<shared_ptr_huff>, compare>;

	queue_huff huffmanQueue;
	void writeCode(bool &, short &, unsigned char &);
	void makeTree(int *);
	void returnCode(const shared_ptr_huff &root);

	// hold huffman code
	std::vector <bool> codeRepresentation;
public:
	huffman(); 
	void encode(std::string in_file_name, std::string out_file_name);
	void decode(std::string in_file_name, std::string out_file_name);

	void TESTER();
	~huffman();
};

#endif HUFFMAN_H