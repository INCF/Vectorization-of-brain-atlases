#include "codeImage.h"
#include "debug.h"
#include <fstream>
#include "string"
#include "bitmap.h"
#include <queue>
#include <utility>

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


/******************************************************************/
/***********************TEST 3:DEBUGGING MODE**********************/
/******************************************************************/

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

/******************************************************************/
/***********************TEST 3:DEBUGGING MODE END******************/
/******************************************************************/
}

//Destructor
CodeImage::~CodeImage()
{
	for(uint i = 0; i < height; ++i)
	{
		delete[] mat[i];
	}
	delete[] mat;

	colorCode.clear();
}

/*
 * codes the input image using operation similar to flood fill
 * please refer the doc for the complete algorithm
 */
void CodeImage::codeImage(ImageMatrix *m)
{
	uint counter = 0;
	for(uint ix = 0; ix < height; ++ix)
	{
		for(uint jy = 0; jy < width; ++jy)
		{
			//if pixel at (i,j) is not yet coded
			if(mat[ix][jy] == -1)
			{
				mat[ix][jy] = counter;

				pixel *myPixel = new pixel[1];
				myPixel->r = m->pixMap[ix][jy].r;
				myPixel->g = m->pixMap[ix][jy].g;
				myPixel->b = m->pixMap[ix][jy].b;
				colorCode.push_back(*myPixel);

				//similar to flood fill
				std::queue< std::pair<int, int> >Q;
				
				Q.push(std::make_pair(ix,jy));
				while(!Q.empty())
				{
					int i = Q.front().first;
					int j = Q.front().second;

					Q.pop();

					if(j - 1 >= 0)
					{
						if(mat[i][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i][j - 1]))
						{
							mat[i][j - 1] = counter;
							Q.push(std::make_pair(i,j-1));
						}
					}

					if(j + 1 <= (int)width - 1)
					{
						if(mat[i][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i][j + 1]))
						{
							mat[i][j + 1] = counter;
							Q.push(std::make_pair(i,j+1));
						}
					}

					if(i - 1 >= 0)
					{
						if(mat[i - 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j]))
						{
							mat[i - 1][j] = counter;
							Q.push(std::make_pair(i-1,j));
						}

						if(j - 1 >= 0)
						{
							if(mat[i - 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j - 1]))
							{
								mat[i - 1][j - 1] = counter;
								Q.push(std::make_pair(i-1,j-1));
							}
						}

						if(j + 1 <= (int)width- 1)
						{
							if(mat[i - 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j + 1]))
							{
								mat[i - 1][j + 1] = counter;
								Q.push(std::make_pair(i-1,j+1));
							}
						}
					}

					if(i + 1 <= (int)height - 1)
					{
						if(mat[i + 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j]))
						{
							mat[i + 1][j] = counter;
							Q.push(std::make_pair(i+1,j));
						}

						if(j - 1 >= 0)
						{
							if(mat[i + 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j - 1]))
							{
								mat[i + 1][j - 1] = counter;
								Q.push(std::make_pair(i+1,j-1));
							}
						}

						if(j + 1 <= (int)width - 1)
						{
							if(mat[i + 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j + 1]))
							{
								mat[i + 1][j + 1] = counter;
								Q.push(std::make_pair(i+1,j+1));
							}
						}
					}

				}

				counter = counter + 1;
			}
		}
	}
}

//returns the coded image matrix
int** CodeImage::getMatrix()
{
	return mat;
}

//returns the vector of colors of unique regions
std::vector<pixel>* CodeImage::getColCode()
{
	return &colorCode;
}
