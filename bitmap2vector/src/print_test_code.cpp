#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp> 

using namespace std;

struct uchar4
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};


void moveTo(unsigned char *cvPtr, int **code_matrix, int i, int j, int counter, int rows, int cols)
{
	if(j - 1 >= 0)
	{
		if(code_matrix[i][j-1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*i + j - 1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*i + j - 1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*i + j - 1)+2] )
		{
			code_matrix[i][j-1] = counter;
			moveTo(cvPtr, code_matrix, i, j-1, counter, rows, cols);
		}
	}

	if(j + 1 <= cols - 1)
	{
		if(code_matrix[i][j+1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*i + j + 1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*i + j + 1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*i + j + 1)+2] )
		{
			code_matrix[i][j+1] = counter;
			moveTo(cvPtr, code_matrix, i, j+1, counter, rows, cols);
		}
	}
	if(i - 1 >= 0)
	{
		if(code_matrix[i-1][j] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i-1) + j)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i-1) + j)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i-1) + j)+2] )
		{
			code_matrix[i-1][j] = counter;
			moveTo(cvPtr, code_matrix, i-1, j, counter, rows, cols);
		}
		if(j - 1 >= 0)
		{
			if(code_matrix[i-1][j-1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i-1) + j-1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i-1) + j-1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i-1) + j-1)+2] )
		{
				code_matrix[i-1][j-1] = counter;
				moveTo(cvPtr, code_matrix, i-1, j-1, counter, rows, cols);
			}
		}
		if(j + 1 <= cols - 1)
		{
			if(code_matrix[i-1][j+1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i-1) + j+1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i-1) + j+1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i-1) + j+1)+2] )
		{
				code_matrix[i-1][j+1] = counter;
				moveTo(cvPtr, code_matrix, i-1, j+1, counter, rows, cols);
			}
		}
	}
	if(i + 1 <= rows - 1)
	{
		if(code_matrix[i+1][j] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i+1) + j)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i+1) + j)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i+1) + j)+2] )
		{
			code_matrix[i+1][j] = counter;
			moveTo(cvPtr, code_matrix, i+1, j, counter, rows, cols);
		}
		if(j - 1 >= 0)
		{
			if(code_matrix[i+1][j-1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i+1) + j-1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i+1) + j-1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i+1) + j-1)+2] )
		{
				code_matrix[i+1][j-1] = counter;
				moveTo(cvPtr, code_matrix, i+1, j-1, counter, rows, cols);
			}
		}
		if(j + 1 <= cols - 1)
		{
			if(code_matrix[i+1][j+1] == -1 && cvPtr[4*(cols*i + j)] == cvPtr[4*(cols*(i+1) + j+1)] && cvPtr[4*(cols*i + j)+1] == cvPtr[4*(cols*(i+1) + j+1)+1] && cvPtr[4*(cols*i + j)+2] == cvPtr[4*(cols*(i+1) + j+1)+2] )
		{
				code_matrix[i+1][j+1] = counter;
				moveTo(cvPtr, code_matrix, i+1, j+1, counter, rows, cols);
			}
		}
	}
}

void preprocess(char *inputFilename, int *numPixels, int *imageRows, int *imageCols)
{
	/* Defining different regions */

	// Load image
	cv::Mat image = cv::imread(inputFilename, CV_LOAD_IMAGE_COLOR);
	cv::Mat imageRGBA;
  	cv::cvtColor(image, imageRGBA, CV_BGR2RGBA);

  	// Initialize values
	*imageRows = image.rows;
	*imageCols = image.cols;
	*numPixels = *imageRows * *imageCols;

	unsigned char *cvPtr = imageRGBA.ptr<unsigned char>(0);

	cv::Mat M(image.rows, image.cols, CV_8UC3, cv::Scalar(255,255,255));

	int **code_matrix;
	code_matrix = (int**) malloc(image.rows * sizeof(int*));
	uchar4* color_code = (uchar4*) malloc(sizeof(uchar4));


	for(int i = 0; i < image.rows; ++i)
	{
		code_matrix[i] = (int*) malloc(image.cols * sizeof(int));
		for(int j = 0; j < image.cols; ++j)
		{
			code_matrix[i][j] = -1;
		}
		
	}
	
	int counter = 0;
	for(int i = 0; i < image.rows; ++i)
	{
		for(int j = 0; j < image.cols; ++j)
		{
			if(code_matrix[i][j] == -1)
			{
				code_matrix[i][j] = counter + 1;
				color_code[counter].r = cvPtr[4*(image.cols * i + j)];
				color_code[counter].g = cvPtr[4*(image.cols * i + j) + 1];
				color_code[counter].b = cvPtr[4*(image.cols * i + j) + 2];
				moveTo(cvPtr, code_matrix, i, j, counter+1, image.rows, image.cols);
				counter = counter + 1;
				color_code = (uchar4*) realloc(color_code, sizeof(uchar4)*(counter+1));
			}
		}
	}

/* Matlab script debugging */
/*
	cout << "matrix_code = [";
	for(int i = 0; i < image.rows; ++i)
	{
		for(int j = 0; j < image.cols; ++j)
		{
			cout << code_matrix[i][j];
			if(j != image.cols - 1)
				cout << ", ";
			else
				cout << "; ";
		}
	}	
	cout << "];" << endl;

	cout << "col_code = zeros(0,3);" << endl;
	for(int i = 0; i < counter; ++i)
	{
		cout << "col_code(end + 1, :) = [ " << (int)color_code[i].r << " " << (int)color_code[i].g << " " << (int)color_code[i].b << " ];" << endl;
	}
*/

	cout << "myImg = zeros(" << image.rows << ", " << image.cols << ", " << 3 << ");" << endl << "myImg = uint8(myImg);" << endl;
	cout << "for i = 1:" << image.rows << endl << "for j = 1:" << image.cols << endl << "myImg(i, j, 1) = col_code(myMat(i, j) , 1);" << endl;
	cout << "myImg(i, j, 2) = col_code(myMat(i,j) , 2);" << endl;
	cout << "myImg(i, j, 3) = col_code(myMat(i,j) , 3);" << endl;
	cout << "end" << endl << "end" << endl;
	cout << "imwrite(myImg, \"test_code_reverse.png\");" << endl;

}


int main(int argc, char **argv)
{
	//cout << "sudo permission required to increase stack size" << endl;
	int i=system ("ulimit -s unlimited");
	char *input_file;
	char *output_file;
	int numSegments;
	int numPix;
	int imageRows;
	int imageCols;
	input_file = argv[1];

	preprocess(input_file, &numPix, &imageRows, &imageCols);
	
	return 0;

}