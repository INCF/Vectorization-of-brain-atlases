#include "util.h"

int ifEqualPixel(pixel x, pixel y)
{
	if(x.r == y.r && x.g == y.g && x.b == y.b)
		return 1;
	else
		return 0;
}