#ifndef __CODEIMAGE_H__
#define __CODEIMAGE_H__ 1

#include "util.h"
#include <vector>

/*
Classs to create and store the coded image matrix
i.e. assigning different unsigned integers to different
/disjoint regions of image
*/
class CodeImage
{
	private:
		int** mat;	//Stores image coded with ints from {0, 1, .. n}; n = number of regions
		uint height;
		uint width;
		uint numDisjointRegions;
		std::vector<pixel> colorCode;	//Stores the rgb value value corresponding to the region/code
	public:
		CodeImage(ImageMatrix*);
		~CodeImage();
		void codeImage(ImageMatrix*);
		void moveTo(ImageMatrix*, int, int, uint);
		int **getMatrix();
		std::vector<pixel>* getColCode();
};

#endif