#include "codeImage.h"
#include "debug.h"
#include <fstream>

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

	//file to store r g b values of each pixel in row major order under test 3
	std::ofstream ofsTest3("testing/test_3_cpp.txt", std::ofstream::out);
	ofsTest3 << height << std::endl;
	ofsTest3 << width << std::endl;

	for(uint i = 0; i < height; ++i)
	{
		for(uint j = 0; j < width; ++j)
		{
			//print r g b int values in new lines
			ofsTest3 << (int)colorCode[mat[i][j]].r << std::endl << (int)colorCode[mat[i][j]].g << std::endl << (int)colorCode[mat[i][j]].b << std::endl;
		}
	}

	//close file
	ofsTest3.close();
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
