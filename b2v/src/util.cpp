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

//r,g,b values are the aregument and return value is corresponding hexcode
std::string RGBToHex(uint rNum, uint gNum, uint bNum)
{
    std::string result;

    result.append("#");

    char buff[16];
    sprintf(buff, "%02X%02X%02X",rNum,gNum,bNum);
    result.append(buff);
    return result;
}
