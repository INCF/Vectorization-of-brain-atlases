#include <iostream>
#include "lodepng.h"
#include <string>
#include <cstdlib>
#include <vector>
#include "debug.h"
#include <fstream>

typedef unsigned char uchar;
typedef uint uint;

struct pixel
{
	uchar r;
	uchar g;
	uchar b;
};

struct ImageMatrix
{
	pixel **pixMap;
	int height;
	int width;
};

int ifEqualPixel(pixel, pixel);

/*
Class to store all bitmaps assosciated with an image.
orig: original bitmap

pre: preprocessed bitmap which is same as the original image with a 
row at top and bottom and a col at left and right of black pixels(0,0,0)

pop: popped out boundaries bitmap is an image with boundaries of different 
regions popped out and represented as white pixels (255,255,255)

Note: pop image has height and width twice that of pre image (See Algorithm 
for popping out boundary for more details).
*/
class Bitmap
{
	private:
		ImageMatrix* orig;
		ImageMatrix* pre;
		ImageMatrix* pop;

	public:
		Bitmap(std::string);
		~Bitmap();
		void del(ImageMatrix*);
		void decodeOneStep(const char*, uint&, uint&, std::vector<uchar>&);
		void intialize(ImageMatrix**, uint, uint);
		void preprocess();
		void popoutBoundaries();
};

// Constructor
Bitmap::Bitmap(std::string filename)
{
	uint h, w;
	std::vector<uchar> v;	//the raww pixel

	decodeOneStep(filename.c_str(), h, w, v);
	//the pixels are now in the vector "v", 4 bytes per pixel, ordered RGBARGBA

	intialize(&orig, h, w);
	intialize(&pre, h + 2, w + 2);
	intialize(&pop, 2 * (h + 2), 2 * (w + 2));

	#ifdef _TEST_1_
	//file to store r g b values of each pixel in row major order under test 1
	std::ofstream ofsTest1("testing/test_1_cpp.txt", std::ofstream::out);
	ofsTest1 << h << std::endl << w << std::endl;
	#endif

	//Assigning values to the pixels of original image matrix
	for(uint i = 0; i < h; ++i)
	{
		for(uint j = 0; j < w; ++j)
		{
			orig->pixMap[i][j].r = v[4 * (i * w + j)];
			orig->pixMap[i][j].g = v[4 * (i * w + j) + 1];
			orig->pixMap[i][j].b = v[4 * (i * w + j) + 2];

			#ifdef _TEST_1_
			//print r g b int values in new lines
			ofsTest1 << (uint)v[4 * (i * w + j)] << std::endl << (uint)v[4 * (i * w + j) + 1] << std::endl << (uint)v[4 * (i * w + j) + 2] << std::endl;
			#endif
		}
	}

	#ifdef _TEST_1_
	//close test_1_cpp.txt file
	ofsTest1.close();
	#endif

	//preprocess original image and store values in pre
	preprocess();
	popoutBoundaries();
}

//Destructor
Bitmap::~Bitmap()
{
	//delete orig
	del(orig);

	//delete pre
	del(pre);

	//delete pop
	del(pop);
}

//deletes an image matrix
void Bitmap::del(ImageMatrix *a)
{
	for(uint i = 0; i < a->height; ++i)
	{
		delete[] a->pixMap[i];
	}
	delete[] a->pixMap;
	delete a;
}

//Decode from disk to raw pixels with a single function call
void Bitmap::decodeOneStep(const char* filename, uint &height, uint &width, std::vector<uchar> &image)
{
  //decode
  unsigned error = lodepng::decode(image, width, height, filename);

  //if there's an error, display it
  if(error) 
  	std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

//Initialize image matrix
void Bitmap::intialize(ImageMatrix **m, uint h, uint w)
{
	*m = new ImageMatrix;
	(*m)->height = h;
	(*m)->width = w;
	(*m)->pixMap = new pixel*[h];
	for(uint i = 0; i < h; ++i)
	{
		(*m)->pixMap[i] = new pixel[w];
	}
}

/*
Appends row at top and bottom and col at left and right of 
orig image of black (0,0,0) pixels
*/
void Bitmap::preprocess()
{
	uint h = pre->height;
	uint w = pre->width;

	for(uint i = 0; i < h; ++i)
	{
		for(uint j = 0; j < w; ++j)
		{
			if(i == 0 || j == 0 || i == h - 1 || j == w - 1)
			{
				pre->pixMap[i][j].r = pre->pixMap[i][j].g = pre->pixMap[i][j].b = 0;
			}
			else
			{
				pre->pixMap[i][j].r = orig->pixMap[i - 1][j - 1].r;
				pre->pixMap[i][j].g = orig->pixMap[i - 1][j - 1].g;
				pre->pixMap[i][j].b = orig->pixMap[i - 1][j - 1].b;
			}
		}
	}
}

void Bitmap::popoutBoundaries()
{
	uint h = pop->height;
	uint w = pop->width;
	
	for(uint i = 0; i < h; ++i)
	{
		for(uint j = 0; j < w; ++j)
		{
			pop->pixMap[i][j].r = pop->pixMap[i][j].g = pop->pixMap[i][j].b = 0;
		}
	}

	for(uint i = 1; i < h * 0.5; ++i)
	{
		uint focus_i = 2 * i + 1;
		for(uint j = 1; j < w * 0.5; ++j) 
		{
			uint focus_j = 2 * j + 1;

			pop->pixMap[focus_i][focus_j].r = pre->pixMap[i][j].r;
			pop->pixMap[focus_i][focus_j].g = pre->pixMap[i][j].g;
			pop->pixMap[focus_i][focus_j].b = pre->pixMap[i][j].b;

			if(!ifEqualPixel(pre->pixMap[i][j], pre->pixMap[i][j-1]))
			{
				pop->pixMap[focus_i][focus_j-1].r = pop->pixMap[focus_i][focus_j-1].g = pop->pixMap[focus_i][focus_j-1].b = 255;
			}
			else
			{
				pop->pixMap[focus_i][focus_j-1].r = pre->pixMap[i][j-1].r;
				pop->pixMap[focus_i][focus_j-1].g = pre->pixMap[i][j-1].g;
				pop->pixMap[focus_i][focus_j-1].b = pre->pixMap[i][j-1].b;
			}
			if(!ifEqualPixel(pre->pixMap[i][j], pre->pixMap[i-1][j-1]))
			{
				pop->pixMap[focus_i-1][focus_j-1].r = pop->pixMap[focus_i-1][focus_j-1].g = pop->pixMap[focus_i-1][focus_j-1].b = 255;
			}
			else
			{
				pop->pixMap[focus_i-1][focus_j-1].r = pre->pixMap[i-1][j-1].r;
				pop->pixMap[focus_i-1][focus_j-1].g = pre->pixMap[i-1][j-1].g;
				pop->pixMap[focus_i-1][focus_j-1].b = pre->pixMap[i-1][j-1].b;
			}
			if(!ifEqualPixel(pre->pixMap[i][j], pre->pixMap[i-1][j]))
			{
				pop->pixMap[focus_i-1][focus_j].r = pop->pixMap[focus_i-1][focus_j].g = pop->pixMap[focus_i-1][focus_j].b = 255;
			}
			else
			{
				pop->pixMap[focus_i-1][focus_j].r = pre->pixMap[i-1][j].r;
				pop->pixMap[focus_i-1][focus_j].g = pre->pixMap[i-1][j].g;
				pop->pixMap[focus_i-1][focus_j].b = pre->pixMap[i-1][j].b;
			}
		}
	}

	#ifdef _TEST_2_
	//file to store r g b values of each pixel in row major order under test 2
	std::ofstream ofsTest2("testing/test_2_cpp.txt", std::ofstream::out);
	ofsTest2 << h << std::endl << w << std::endl;

	for(uint i = 0; i < h; ++i)
	{
		for(uint j = 0; j < w; ++j)
		{
			//print r g b int values in new lines
			ofsTest2 << (int)pop->pixMap[i][j].r << std::endl << (int)pop->pixMap[i][j].g << std::endl << (int)pop->pixMap[i][j].b << std::endl;
		}
	}

	//close test_2_cpp.txt file
	ofsTest2.close();
	#endif
}

int ifEqualPixel(pixel x, pixel y)
{
	if(x.r == y.r && x.g == y.g && x.b == y.b)
		return 1;
	else
		return 0;
}

int main(int argc, char **argv)
{
	std::string filename = "";

	switch(argc)
	{
		case 2:
			filename.assign(argv[1]);
			break;
		default:
			std::cout << "Usage: ./main filename.png" << std::endl;
			exit(1);
			break;
	}

	Bitmap *inputBitmap = new Bitmap(filename);

	return 0;
}

