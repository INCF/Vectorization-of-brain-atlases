#include "util.h"
#include "lodepng.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cctype>

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

//hash/hex coded rgb value is argument and return value is its corresponding pixel
pixel hexToRGB(std::string hexCode)
{
    std::string hex = "";
    pixel ret;
    std::string legit = "0123456789abcdefABCDEF";

    //will always be true
    if(hexCode.substr(0, 1) == "#")
    {
      hex = hexCode.substr(1);
    }

    for(uint i = 0; i < hex.size(); ++i)
    {
      if(std::string::npos == legit.find_first_of(hex[i]))
      {
        std::cout << "Please check your hex color code characters" << std::endl;
        exit(1);
      }
    }

    std::string r = "", g = "", b = "";
    r = hex.substr(0, 2);
    g = hex.substr(2, 2);
    b = hex.substr(4, 2);

    ret.r = (unsigned char)strtol(r.c_str(), NULL, 16);
    ret.g = (unsigned char)strtol(g.c_str(), NULL, 16);
    ret.b = (unsigned char)strtol(b.c_str(), NULL, 16);

    return ret;
}

pixel parseRGB(std::string rgb)
{
    std::string trimRGB = "";
    char *pEnd;
    pixel ret;

    //will always be true
    if(rgb.substr(0, 3) == "rgb")
    {
      trimRGB = rgb.substr(3);
    }

    for(uint i = 0; i < trimRGB.size(); ++i)
    {
      if(!isdigit(trimRGB[i]) && trimRGB[i] != '-')
      {
        trimRGB[i] = ' ';
      }
    }

    uint r = strtol(trimRGB.c_str(), &pEnd, 10);
    uint g = strtol(pEnd, &pEnd, 10);
    uint b = strtol(pEnd, NULL, 10);

    if(!(r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255))
    {
      std::cout << "Please check your R G B values: must lie in [0, 255]" << std::endl;
      exit(1);
    }
    ret.r = (unsigned char)r;
    ret.g = (unsigned char)g;
    ret.b = (unsigned char)b;

    return ret;
}