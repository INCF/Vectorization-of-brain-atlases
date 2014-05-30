#ifndef __BITMAP_H__
#define __BITMAP_H__ 1

#include "util.h"
#include "codeImage.h"
#include "graph.h"
#include "lodepng.h"
#include <vector>
#include <string>

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
		CodeImage *codedImage;
		Graph *graph;
		int** pixToNodeMap;
		uint numControlPoints;
	public:
		Bitmap(std::string);
		~Bitmap();
		void del(ImageMatrix*);
		void decodeOneStep(const char*, uint&, uint&, std::vector<uchar>&);
		void intialize(ImageMatrix**, uint, uint);
		void preprocess();
		void popoutBoundaries();
		void detectControlPoints();
		int checkUniqueRegionPixel(pixel, std::vector<pixel>&);
		void formAdjacencyList();
		void processImage();
		void writeOuputSVG(std::string);
};

#endif