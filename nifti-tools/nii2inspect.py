# Rembrandt Bakker, June 2014

import argparse, sys, os, numpy, subprocess
import os.path as op
import scipy
import re
import json

SliceDirs = {'x':'Left-Right','y':'Posterior-Anterior','z':'Inferior-Superior'}

def argument_parser():
  """ Define the argument parser and return the parser object. """
  parser = argparse.ArgumentParser(
      description="""
          Colormap supports various formats:
          1. comma separated list of rgb-hex values: #RGB1,#RGB2,...
          2. range of rgb-hex values: #RGB1-#RGB2
          3. constant color with range of alpha values: alpha-#RGB
      """,
      formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-lr','--layers', type=json.loads, help="Input json struct with fields 'file','title','colormap','pctile'", action='append')
  parser.add_argument('-o','--out', type=str, help="Html output file", required=True)
  parser.add_argument('-oi','--out-images', type=str, help="Image output folder", required=False)
  parser.add_argument('-sx','--slices_x', type=str, help="Slices in the x-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sy','--slices_y', type=str, help="Slices in the y-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sz','--slices_z', type=str, help="Slices in the z-dimension, start%:step%:stop%", required=False)
  return parser

def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    try:
        print "layers {}".format(args.layers)

        """ Basic argument checking """
        for lr in args.layers:
            if not op.exists(lr["file"]):
                raise Exception('Nifti file "{}" not found.'.format(lr["file"]))

        args.sliceRangePct = [[],[],[]]
        for d in [0,1,2]:
            dim = ['x','y','z'][d]
            s = getattr(args,'slices_'+dim)
            if s:
                s = s.split(':');
                args.sliceRangePct[d] = [int(r.rstrip('%')) for r in s]
            else:
                args.sliceRangePct[d] = [0,10,100]
      
    except Exception as e:
        print('Error in parse_arguments {}'.format(e.args))
        exit(1)
    
    return args
         
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
    for rgb in index2rgb:
        rgbLen = len(rgb)
        break
        
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
        
    
def run(args):
    print('Layer specification: {}'.format(args.layers))
    try:
        htmlFile = args.out
        if op.isdir(htmlFile):
            htmlFile = op.join(htmlFile,'index.html')
        htmlFolder = op.dirname(htmlFile)
        if args.out_images:
            imgFolder = args.out_images
        else:
            htmlName,htmlExt = op.splitext(op.basename(htmlFile))
            imgFolder = op.join(htmlFolder,htmlName+'_files')
        
        if not(op.exists(htmlFolder)):
            os.makedirs(htmlFolder)
            print 'Created html output folder "{}".'.format(htmlFolder)
        if not(op.exists(imgFolder)):
            os.makedirs(imgFolder)
            print 'Created image output folder "{}".'.format(imgFolder)
        imgFolder = op.realpath(imgFolder)
        scriptDir = op.realpath(op.dirname(__file__))

        parsedLayers = []
        for i,lr in enumerate(args.layers):
            nifti_src = lr["file"]
            if not nifti_src: continue
            baseName = re.sub('(\.nii|\.nii.gz)$','',op.basename(nifti_src))

            import nibabel
            nii = nibabel.load(nifti_src)
            img = numpy.squeeze(nii.get_data())
            img_min = numpy.amin(img)
            img_max = numpy.amax(img)
            print 'Image type: {} {}-{}'.format(img.dtype,img_min,img_max)
            if "pctile" in lr:
                pctile = numpy.uint8(re.split('[,\- ]+',str(lr["pctile"])))
                if len(pctile)<1:
                    pctile = [0,100]
                elif len(pctile<2):
                    pctile = [0,pctile[0]]
                if pctile[1]<=pctile[0]:
                    raise('Max percentile must be larger than min percentile, not {},{}'.format(pctile[0],pctile[1]))
                elif pctile[0]<0:
                    raise('Min percentile must be >=0, not {}'.format(pctile[0]))
                elif pctile[1]>100:
                    raise('Max percentile must be <=100, not {}'.format(pctile[1]))
                if pctile[0]>0:
                    img_min = numpy.percentile(img,pctile[0])
                if pctile[1]>0:
                    img_max = numpy.percentile(img,pctile[1])
                print 'Percentile {}-{} range: {}-{}'.format(pctile[0],pctile[1],img_min,img_max)
            hdr = nii.get_header()
            q = hdr.get_best_affine();
            ornt = nibabel.io_orientation(q)
            print 'Orientation: {}'.format(ornt)
            img = nibabel.apply_orientation(img,ornt)
            dims = img.shape
            print 'Nifti image for layer {} loaded, data type "{}"'.format(i,img.dtype)

            if len(dims)==4:
                print 'Error: NIFTI file with RGB color data not supported.'
                exit(0)
            
            # apply colormap
            fmt = 'jpg'
            index2rgb = None
            hasAlpha = 1
            rescale = False
            if "colormap" in lr:
                cmap = lr["colormap"]
                matches = re.search('^(#[0-9a-fA-F]+)-(#[0-9a-fA-F]+)$',cmap)
                if matches:
                    rescale = True
                    hasAlpha = False
                    rgb1 = hex2rgb(matches.group(1))
                    rgb2 = hex2rgb(matches.group(2))
                    index2rgb = [numpy.uint8([rgb1[0]+i/255.0*(rgb2[0]-rgb1[0]),rgb1[1]+i/255.0*(rgb2[1]-rgb1[1]),rgb1[2]+i/255.0*(rgb2[2]-rgb1[2])]) for i in range(256)]
                elif cmap.startswith('alpha'):
                    fmt = 'png'
                    rescale = True
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
                        index2rgb = json.load(fp)
                else:
                    raise('Do not know how to parse colormap "{}".'.format(cmap))
                  
            sliceRange = [[],[],[]]
            for d in [0,1,2]:
                dim = ['x','y','z'][d]
                numSlices = dims[d];
                sliceStep = int(args.sliceRangePct[d][1]*numSlices/100)
                sliceStart = int(args.sliceRangePct[d][0]*(numSlices-1)/100)
                sliceEnd = int(args.sliceRangePct[d][2]*(numSlices-1)/100)
                sliceRange[d] = [sliceStart,sliceStep,sliceEnd]
                for i in range(sliceStart,sliceEnd+1,sliceStep):
                    slice = get_slice(img,dim,i)
                    
                    pngFile = baseName+'_{}{:d}.{}'.format(dim,i,fmt)
                    if index2rgb:
                        slice = slice2rgb(slice,index2rgb,rescale,img_min,img_max)

                    # Save image to PNG
                    scipy.misc.toimage(slice).save(op.join(imgFolder,pngFile))
                        
                    if i==sliceStart:
                        print 'image {}{} saved to png file "{}".'.format(dim,i,pngFile)

            # update parsedLayers
            pl = {
              "name": baseName,
              "ext": fmt,
              "src": nifti_src
            }
            if "title" in lr:
                pl["title"] = lr["title"];
            parsedLayers.append(pl);

        inspectFile = '{}/nii_inspect.html'.format(scriptDir);
        with open(inspectFile, 'r') as fp:
            html = fp.read()
            html = html.replace(r"var defaultLayers = [];",
                r"var defaultLayers = {};".format(json.dumps(parsedLayers)))                            
            html = html.replace(r"var defaultSliceRange = [];",
                "var defaultSliceRange = {};".format(json.dumps(sliceRange)))
            html = html.replace(r"var imgDir = '';",
                "var imgDir = '{}/';".format(op.relpath(imgFolder,htmlFolder)))
  
        with open(htmlFile, 'w') as fp:
            fp.write(html)
                
        print 'HTML viewer saved as "{}"'.format(htmlFile)
        
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise


if __name__ == '__main__':
    args = parse_arguments()
    run(args)
