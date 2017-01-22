#include <string>
#include <vector>

using namespace std;

typedef struct SPixel {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}PIXEL;

class RLE{
private:
	bool compare(SPixel pixel1, SPixel pixel2) {
		if (pixel1.red == pixel2.red && pixel1.green == pixel2.green && pixel1.blue == pixel2.blue)
			return true;
		else return false;
	};
	PIXEL next;
	PIXEL current;
	uint8_t repetition = 1;
	vector<PIXEL>vcopy;
	uint8_t vstart = 0;
	uint8_t vsize = vcopy.size();

public:
	RLE();
	virtual ~RLE();
	void encode(string &input, string &output);
	void decode(string &input, string &output);
};


