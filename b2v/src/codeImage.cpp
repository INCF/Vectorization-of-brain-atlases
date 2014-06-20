#include "codeImage.h"
#include "debug.h"
#include <fstream>
#include "string"
#include "bitmap.h"
#include <queue>
#include <utility>
#include "GraphicsGems.h"
#include <cstdlib>
#include <vector>
#include <iostream>
#include "util.h"

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
				std::vector<Point2> temp;
				Point2 *tempCoord = new Point2[1];
				tempCoord->x = ix;
				tempCoord->y = jy;
				temp.push_back(*tempCoord);

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
							tempCoord = new Point2[1];
							tempCoord->x = i;
							tempCoord->y = j - 1;
							temp.push_back(*tempCoord);
							Q.push(std::make_pair(i,j-1));
						}
					}

					if(j + 1 <= (int)width - 1)
					{
						if(mat[i][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i][j + 1]))
						{
							mat[i][j + 1] = counter;
							tempCoord = new Point2[1];
							tempCoord->x = i;
							tempCoord->y = j + 1;
							temp.push_back(*tempCoord);
							Q.push(std::make_pair(i,j+1));
						}
					}

					if(i - 1 >= 0)
					{
						if(mat[i - 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j]))
						{
							mat[i - 1][j] = counter;
							tempCoord = new Point2[1];
							tempCoord->x = i - 1;
							tempCoord->y = j;
							temp.push_back(*tempCoord);
							Q.push(std::make_pair(i-1,j));
						}

						if(j - 1 >= 0)
						{
							if(mat[i - 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j - 1]))
							{
								mat[i - 1][j - 1] = counter;
								tempCoord = new Point2[1];
								tempCoord->x = i - 1;
								tempCoord->y = j - 1;
								temp.push_back(*tempCoord);
								Q.push(std::make_pair(i-1,j-1));
							}
						}

						if(j + 1 <= (int)width- 1)
						{
							if(mat[i - 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i - 1][j + 1]))
							{
								mat[i - 1][j + 1] = counter;
								tempCoord = new Point2[1];
								tempCoord->x = i - 1;
								tempCoord->y = j + 1;
								temp.push_back(*tempCoord);
								Q.push(std::make_pair(i-1,j+1));
							}
						}
					}

					if(i + 1 <= (int)height - 1)
					{
						if(mat[i + 1][j] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j]))
						{
							mat[i + 1][j] = counter;
							tempCoord = new Point2[1];
							tempCoord->x = i + 1;
							tempCoord->y = j;
							temp.push_back(*tempCoord);
							Q.push(std::make_pair(i+1,j));
						}

						if(j - 1 >= 0)
						{
							if(mat[i + 1][j - 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j - 1]))
							{
								mat[i + 1][j - 1] = counter;
								tempCoord = new Point2[1];
								tempCoord->x = i + 1;
								tempCoord->y = j - 1;
								temp.push_back(*tempCoord);
								Q.push(std::make_pair(i+1,j-1));
							}
						}

						if(j + 1 <= (int)width - 1)
						{
							if(mat[i + 1][j + 1] == -1 && ifEqualPixel(m->pixMap[i][j], m->pixMap[i + 1][j + 1]))
							{
								mat[i + 1][j + 1] = counter;
								tempCoord = new Point2[1];
								tempCoord->x = i + 1;
								tempCoord->y = j + 1;
								temp.push_back(*tempCoord);
								Q.push(std::make_pair(i+1,j+1));
							}
						}
					}
				}
				regionPixelCoord.push_back(temp);
				temp.clear();
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

void CodeImage::processRegions(uint threshold)
{
	int pass = 5;
	while(pass > 0)
	{
		for(uint i = 0; i < regionPixelCoord.size(); ++i)
		{
			//std::cout << "Region Num: " << i << "  " << "its size: " << regionPixelCoord[i].size() << std::endl;
			if(regionPixelCoord[i].size() <= threshold)
			{
				recursiveMerge(i);
			}
		}
		pass--;
	}
}

void CodeImage::recursiveMerge(int ind)
{
	while(!regionPixelCoord[ind].empty())
	{
		for(uint i = 0; i < regionPixelCoord[ind].size(); ++i)
		{
			int ix = regionPixelCoord[ind][i].x;
			int jy = regionPixelCoord[ind][i].y;

			std::vector<int> boundElem, boundCount;
			for(int l = ix - 1; l <= ix + 1; ++l)
			{
				for(int m = jy - 1; m <= jy + 1; ++m)
				{
					if(l >= 0 && l < (int)height && m >= 0 && m < (int)width && l != ix && m != jy)
					{
						if(mat[l][m] != ind)
						{
							bool check = false;
							for(uint k = 0; k < boundElem.size(); ++k)
							{
								if(boundElem[k] == mat[l][m])
								{
									check = true;
									boundCount[k]++;
									break;
								}
							}
							if(!check)
							{
								boundElem.push_back(mat[l][m]);
								boundCount.push_back(1);
							}
						}
					}
				}
			}

			if(boundCount.size() != boundElem.size())
			{
				std::cout << "Error size of boundElem and boundCount not equal" << std::endl;
				exit(1);
			}

			if(!boundElem.empty())
			{
				uint max = 0;
				for(uint k = 1; k < boundElem.size(); ++k)
				{
					if(boundCount[k] > boundCount[max])
					{
						max = k;
					}
				}
				mat[ix][jy] = boundElem[max];
				Point2 temp = 
				{
					.x = (double)ix,
					.y = (double)jy
				};
				regionPixelCoord[boundElem[max]].push_back(temp);
				regionPixelCoord[ind].erase(regionPixelCoord[ind].begin() + i);
			}
			boundCount.clear();
			boundElem.clear();
		}
	}
}

ImageMatrix *CodeImage::getFinalImage()
{
	ImageMatrix *m = new ImageMatrix[1];

	m->height = height;
	m->width = width;

	m->pixMap = new pixel*[height];
	for(uint i = 0; i < height; ++i)
	{
		m->pixMap[i] = new pixel[width];
	}

	for(uint i = 0; i < height; ++i)
	{
		for(uint j = 0; j < width; ++j)
		{
			m->pixMap[i][j] = colorCode[mat[i][j]];
		}
	}

	return m;
}