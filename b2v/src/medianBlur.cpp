#include "medianBlur.h"

void medianBlur(int kernelRadius, ImageMatrix *m)
{
    uint h, w;
    long unsigned int L2Cache = 512 * 1024;

    h = m->height;
    w = m->width;
    
    uchar *src = new uchar[3*h*w];
    uchar *dst = new uchar[3*h*w];

    for(uint i = 0; i < h; ++i)
    {
        for(uint j = 0; j < w; ++j)
        {
            src[3 * (i * w + j)+0] = m->pixMap[i][j].r;
            src[3 * (i * w + j)+1] = m->pixMap[i][j].g;
            src[3 * (i * w + j)+2] = m->pixMap[i][j].b;
        }
    }

    ctmf(src, dst, w, h, 3*w, 3*w, kernelRadius, 3, L2Cache);

    for(uint y = 0; y < h; y++)
    {
        for(uint x = 0; x < w; x++)
        {
            m->pixMap[y][x].r = dst[3 * (y * w + x)];
            m->pixMap[y][x].g = dst[3 * (y * w + x)+1];
            m->pixMap[y][x].b = dst[3 * (y * w + x)+2];
        }
    }

    delete[] src;
    delete[] dst;
}