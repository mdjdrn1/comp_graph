#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <memory>	//shared_ptr
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <map>

class huffman
{
public:
	//@in_file_name - input file name
	//@out_file_name - output file name
	huffman(std::string in_file_name, std::string out_file_name);
private:
	class encode
	{
	public:
		encode(std::string in_file_name, std::string out_file_name);
	protected:
		// hold huffman code
		std::vector <bool> codeRepresentation;

		struct node;	// stores nodes in containers
		struct compare;	// operator overloading comparisons (growing)
		using shared_ptr_huff = std::shared_ptr<node>;
		struct node 
		{
			uint8_t color;		// RGB value
			std::string code;		// binary representation 
			int frequency;			// color
			shared_ptr_huff left;   // child 
			shared_ptr_huff right;	// child

			node(uint8_t color, int frequency);
		};
		struct compare 
		{
			bool operator()(const shared_ptr_huff &l, const shared_ptr_huff &r)
			{
				return l->frequency > r->frequency;
			} // increasing
		};
		//				    index/color	| huffman code
		std::string huffmanCode[256];
		using queue_huff = std::priority_queue < shared_ptr_huff, std::vector<shared_ptr_huff>, compare>;

		queue_huff huffmanQueue;

		////////////////////////////////////////////////////////////
		///////////////       METHODS     //////////////////////////
		////////////////////////////////////////////////////////////
		void makeTree(unsigned long long *);
		void returnCode(const shared_ptr_huff &root);
	};

	class decode
	{
	public:
		decode(std::string in_file_name, std::string out_file_name);
	protected:


	};
};

#endif HUFFMAN_H

