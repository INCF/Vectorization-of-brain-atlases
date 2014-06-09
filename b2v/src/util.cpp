#include "util.h"
#include "lodepng.h"
#include <iostream>
#include <cstdio>
#include <string>

int ifEqualPixel(pixel x, pixel y)
{
	if(x.r == y.r && x.g == y.g && x.b == y.b)
		return 1;
	else
		return 0;
}

//The image argument has width * height RGBA pixels or width * height * 4 bytes
void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

std::string RGBToHex(uint rNum, uint gNum, uint bNum)
{
    std::string result;

    result.append("#");

    char r[255];   
    sprintf(r, "%.2X", rNum);
    result.append(r );

    char g[255];   
    sprintf(g, "%.2X", gNum);
    result.append(g );

    char b[255];   
    sprintf(b, "%.2X", bNum);
    result.append(b ); 

    return result;
}
