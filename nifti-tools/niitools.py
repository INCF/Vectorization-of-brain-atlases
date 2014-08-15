# Rembrandt Bakker, June 2014

import os.path as op
import re,json,numpy

def get_limits(img,pctile=False):
    minmax = [ numpy.amin(img), numpy.amax(img) ]
    if pctile: 
        if isinstance(pctile,str):
            pctile = re.split('[,\- ]+',pctile)
            pctile = [float(p) for p in pctile]
        if len(pctile)<1:
            pctile = [0,100]
        elif len(pctile)<2:
            pctile = [0,pctile[0]]
        if pctile[1]<=pctile[0]:
            raise Exception('Max percentile must be larger than min percentile, not {},{}'.format(pctile[0],pctile[1]))
        elif pctile[0]<0:
            raise Exception('Min percentile must be >=0, not {}'.format(pctile[0]))
        elif pctile[1]>100:
            raise Exception('Max percentile must be <=100, not {}'.format(pctile[1]))
        if pctile[0]>0:
            minmax[0] = numpy.percentile(img,pctile[0])
        if pctile[1]>0:
            minmax[1] = numpy.percentile(img,pctile[1])
    return minmax


"""
In the code below, the transpose is needed because images are written with the 
first dimension as rows, second as columns. This must be flipped to align with
the common definition of x- and y axes .
The ::-1 part is a mirroring operation on the y-axis, which is needed because
images are written top to bottom, reversed from the common y-axis direction.
"""
def get_slice(img,dim,i):
    if dim == 'x':
        slice = img[i,:,::-1].squeeze().transpose();
    elif dim == 'y':
        slice = img[:,i,::-1].squeeze().transpose();
    elif dim == 'z':
        slice = img[:,::-1,i].squeeze().transpose();
    return slice

def hex2rgb(v):
    v = v.lstrip('#')
    lv = len(v)
    if lv==3:
        rgb = [int(v[i]+v[i], 16) for i in range(0,lv)];
    elif lv==6:
        rgb = [int(v[i:i+2], 16) for i in range(0,lv,2)];
    else:
        raise Exception('Invalid hex color {}'.format(v))
    return rgb

def hex2rgba(v):
    rgba = hex2rgb(v)

    # transparent background
    if all(v==0 for v in rgba):
        rgba.append(0)
    else: 
        rgba.append(255)
    return rgba

def slice2rgb(slice,index2rgb,rescale,minLevel=None,maxLevel=None):
    if isinstance(index2rgb,dict):
        rgbLen = len(index2rgb[index2rgb.keys()[0]])
    else:
        rgbLen = len(index2rgb[0])
        
    shape = slice.shape
    slice = slice.reshape(-1)
    if rescale: 
        slice = 255.9999*(slice-minLevel)/(maxLevel-minLevel)
        slice[slice<0] = 0
        slice[slice>255] = 255
        slice = numpy.uint8(slice)
    rgbImg = numpy.zeros(shape=(slice.shape[0],rgbLen), dtype=numpy.uint8)
    for idx in numpy.unique(slice):
        mask = (slice == idx)
        try:
            val = index2rgb[numpy.uint8(idx)]
        except KeyError:
            val = index2rgb[str(idx)]

        rgbImg[mask] = val
    return rgbImg.reshape(shape[0],shape[1],rgbLen)

def parse_colormap(img,args_colormap):
    fmt = 'png'
    hasAlpha = False
    index2rgb = None
    if args_colormap:
        cmap = args_colormap
        matches = re.search('^(#[0-9a-fA-F]+)-(#[0-9a-fA-F]+)$',cmap)
        if matches:
            fmt = 'jpg'
            hasAlpha = False
            rgb1 = hex2rgb(matches.group(1))
            rgb2 = hex2rgb(matches.group(2))
            index2rgb = [
                numpy.array([
                    rgb1[0]+i/255.0*(rgb2[0]-rgb1[0]),
                    rgb1[1]+i/255.0*(rgb2[1]-rgb1[1]),
                    rgb1[2]+i/255.0*(rgb2[2]-rgb1[2])
                ],numpy.uint8) for i in range(256)
            ]
        elif cmap.startswith('alpha'):
            fmt = 'png'
            hasAlpha = True
            matches = re.search('^alpha-(#[0-9a-fA-F]+)$',cmap)
            if matches:
                rgb = hex2rgb(matches.group(1))
            else:
                rgb = list([255,255,255])
            index2rgb = [[rgb[0],rgb[1],rgb[2],i] for i in range(256)]
        elif cmap[0] == '#':
            fmt = 'png'
            hasAlpha = True
            cmap = cmap.split(',')
            index2rgb = {};
            for i,a in enumerate(cmap):
                index2rgb[i] = hex2rgba(a)
            print "Color map {}".format(index2rgb)
        elif op.exists(cmap):
            fmt = 'png'
            hasAlpha = False
            with open(cmap,'r') as fp:
                cmap = json.load(fp)
            print('{}'.format(cmap))
            index2rgb = {};
            if isinstance(cmap,dict):
                for i in cmap:
                    print cmap[i]
                    index2rgb[i] = hex2rgba(cmap[i])
            else:
                raise Exception('Colormap file must contain a json-encoded map with color index as keys and #RRGGBB-colors as values.')
        else:
            raise Exception('Do not know how to parse colormap "{}".'.format(cmap))
    return index2rgb
