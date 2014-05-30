#include "codeImage.h"
#include "debug.h"
#include <fstream>
#include "string"
#include "bitmap.h"

//Constructor
CodeImage::CodeImage(ImageMatrix* m)
{
	height = m->height;
	width = m->width;

	mat = new int*[height];
	for(uint i = 0; i < height; ++i)
	{
		mat[i] = new int[width];
		for(uint j = 0; j < width; ++j)
			mat[i][j] = -1;	//-1 represents not coded yet
	}

	codeImage(m);
	numDisjointRegions = colorCode.size();

	#ifdef _TEST_3_
	//generate some image
	std::string path = ROOT_DIR;
	std::vector<unsigned char> image;
	image.resize(width * height * 4);
	for(uint y = 0; y < height; y++)
	{
		for(uint x = 0; x < width; x++)
		{
			image[4 * width * y + 4 * x + 0] = colorCode[mat[y][x]].r;
			image[4 * width * y + 4 * x + 1] = colorCode[mat[y][x]].g;
			image[4 * width * y + 4 * x + 2] = colorCode[mat[y][x]].b;
			image[4 * width * y + 4 * x + 3] = 255;
		}
	}

	encodeOneStep((path + "/check/test_3_revertCoded.png").c_str(), image, width, height);
	image.clear();
	#endif

}

//Destructor
CodeImage::~CodeImage()
{
	for(uint i = 0; i < height; ++i)
	{
		delete[] mat[i];
	}
	delete[] mat;
}

//codes the input image using operation similar to flood fill
void CodeImage::codeImage(ImageMatrix *m)
{
	uint counter = 0;
	for(uint i = 0; i < height; ++i)
	{
		for(uint j = 0; j < width; ++j)
		{
			//if pixel at (i,j) is not yet coded
			if(mat[i][j] == -1)
			{
				mat[i][j] = counter;

				pixel *myPixel = new pixel[1];
				myPixel->r = m->pixMap[i][j].r;
				myPixel->g = m->pixMap[i][j].g;
				myPixel->b = m->pixMap[i][j].b;
				colorCode.push_back(*myPixel);

				//move to pixel(i,j) and flood fill
				moveTo(m, i, j, counter);
				counter = counter + 1;
			}
		}
	}
}

/*
Move to pixel(i,j) and do an operation like bfs and assign counter value to
all the adjacent pixels of same color
*/
void CodeImage::moveTo(ImageMatrix *m, int i, int j, uint counter)
{
	if(j - 1 >= 0)
	{
		if(mat[i][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i][j - 1]))
		{
			mat[i][j - 1] = counter;
			moveTo(m, i, j - 1, counter);
		}
	}

	if(j + 1 <= (int)width - 1)
	{
		if(mat[i][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i][j + 1]))
		{
			mat[i][j + 1] = counter;
			moveTo(m, i, j + 1, counter);
		}
	}

	if(i - 1 >= 0)
	{
		if(mat[i - 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j]))
		{
			mat[i - 1][j] = counter;
			moveTo(m, i - 1, j, counter);
		}

		if(j - 1 >= 0)
		{
			if(mat[i - 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j - 1]))
			{
				mat[i - 1][j - 1] = counter;
				moveTo(m, i - 1, j - 1, counter);
			}
		}

		if(j + 1 <= (int)width- 1)
		{
			if(mat[i - 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j + 1]))
			{
				mat[i - 1][j + 1] = counter;
				moveTo(m, i - 1, j + 1, counter);
			}
		}
	}

	if(i + 1 <= (int)height - 1)
	{
		if(mat[i + 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j]))
		{
			mat[i + 1][j] = counter;
			moveTo(m, i + 1, j, counter);
		}

		if(j - 1 >= 0)
		{
			if(mat[i + 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j - 1]))
			{
				mat[i + 1][j - 1] = counter;
				moveTo(m, i + 1, j - 1, counter);
			}
		}

		if(j + 1 <= (int)width - 1)
		{
			if(mat[i + 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j + 1]))
			{
				mat[i + 1][j + 1] = counter;
				moveTo(m, i + 1, j + 1, counter);
			}
		}
	}
}

int** CodeImage::getMatrix()
{
	return mat;
}

std::vector<pixel>* CodeImage::getColCode()
{
	return &colorCode;
}
