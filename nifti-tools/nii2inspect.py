# Rembrandt Bakker, June 2014

import argparse, sys, os, numpy, subprocess
import os.path as op
import scipy
import re

SliceDirs = {'x':'Left-Right','y':'Posterior-Anterior','z':'Inferior-Superior'}

def argument_parser():
  """ Define the argument parser and return the parser object. """
  parser = argparse.ArgumentParser(
    description='description',
    formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-i','--nifti_lr1', type=str, help="Input Nifti file layer 1", required=True)
  parser.add_argument('-j','--nifti_lr2', type=str, help="Input Nifti file layer 2", required=False)
  parser.add_argument('-k','--nifti_lr3', type=str, help="Input Nifti file layer 3", required=False)
  parser.add_argument('-o','--path_dest', type=str, help="Output snapshot folder", required=True)
  parser.add_argument('-c','--colormap_lr1', type=str, help="JSON colormap layer 1", required=False)
  parser.add_argument('-d','--colormap_lr2', type=str, help="JSON colormap layer 2", required=False)
  parser.add_argument('-e','--colormap_lr3', type=str, help="JSON colormap layer 3", required=False)
  parser.add_argument('-sx','--slices_x', type=str, help="Slices in the x-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sy','--slices_y', type=str, help="Slices in the y-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sz','--slices_z', type=str, help="Slices in the z-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-t1','--title_lr1', type=str, help="Title layer 1", required=False)
  parser.add_argument('-t2','--title_lr2', type=str, help="Title layer 2", required=False)
  parser.add_argument('-t3','--title_lr3', type=str, help="Title layer 3", required=False)
  return parser

def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    try:
        """ Basic argument checking """
        if not args.nifti_lr1:
            raise Exception('Nifti file for layer 1 must be specified')
      
        if args.nifti_lr1 and not op.exists(args.nifti_lr1):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_lr1))

        if args.nifti_lr2 and not op.exists(args.nifti_lr2):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_lr2))

        if args.nifti_lr3 and not op.exists(args.nifti_lr3):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_lr3))

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
    if dim is 'x':
        slice = img[i,:,::-1].squeeze().transpose();
    elif dim is 'y':
        slice = img[:,i,::-1].squeeze().transpose();
    elif dim is 'z':
        slice = img[:,::-1,i].squeeze().transpose();
    return slice

def hex2rgba(v):
    v = v.lstrip('#')
    lv = len(v)
    rgba = [int(v[i:i+lv/3], 16) for i in range(0, lv, lv/3)];
    if all(v==0 for v in rgba):
        rgba.append(0)
    else: 
        rgba.append(255)
    return tuple(rgba)

def run(args):
    print('Input Nifti layer 1: {}'.format(args.nifti_lr1))
    print('Input Nifti layer 2: {}'.format(args.nifti_lr2))
    print('Input Nifti layer 3: {}'.format(args.nifti_lr3))
    try:
        destFolder = args.path_dest
        if not(op.exists(destFolder)):
            os.makedirs(destFolder)
            print 'Created destination folder "{}".'.format(destFolder)
        destFolder = op.realpath(destFolder)
        scriptDir = op.realpath(op.dirname(__file__))

        redirect = {}
        for mode in ['lr1','lr2','lr3']:
            nifti_src = getattr(args,'nifti_'+mode);
            if not nifti_src: continue
            colormap = getattr(args,'colormap_'+mode);

            import nibabel
            nii = nibabel.load(nifti_src)
            img = numpy.squeeze(nii.get_data())
            print 'Image type: {} {}-{}'.format(img.dtype,numpy.amin(img),numpy.amax(img))
            hdr = nii.get_header()
            q = hdr.get_best_affine();
            ornt = nibabel.io_orientation(q)
            print 'Orientation: {}'.format(ornt)
            img = nibabel.apply_orientation(img,ornt)
            dims = img.shape
            print 'Nifti image for layer {} loaded, data type "{}"'.format(mode,img.dtype)

            if len(dims)==4:
                print 'Error: NIFTI file with RGB color data not supported yet.'
                exit(0)

            baseName = op.basename(nifti_src).rstrip('.gz').rstrip('.nii')
            if colormap: fmt = 'png' 
            else: fmt = 'jpg'
            redirect[mode] = baseName+'.'+fmt;

            index2rgb = None
            if colormap:
                if colormap[0] is '#':
                    colormap = colormap.split(',')
                    colormap = dict((str(i),val) for i,val in enumerate(colormap))
                else:
                    if not op.exists(colormap):
                        raise('JSON colormap file "{}" not found.'.format(colormap))
                  
                    with open(colormap,'r') as fp:
                        colormap = json.load(fp)
                  
                index2rgb = {};
                for a in colormap:
                    index2rgb[a] = hex2rgba(colormap[a])
                print "Color map {}".format(index2rgb)

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
                    if index2rgb:
                        shape = slice.shape
                        slice = slice.reshape(-1)
                        rgbImg = numpy.zeros(shape=(slice.shape[0],4), dtype=numpy.uint8)
                        for grayvalue in numpy.unique(slice):
                            mask = (slice == grayvalue)
                            val = index2rgb[str(grayvalue)]
                            rgbImg[mask] = val
                        slice = rgbImg.reshape(shape[0],shape[1],4)

                    # Save image to PNG
                    #pngFile = baseName+'_{}{:02d}.{}'.format(dim,100*(sample+1)/(numSamples+1),fmt)
                    pngFile = baseName+'_{}{:d}.{}'.format(dim,i,fmt)
                    scipy.misc.toimage(slice).save(op.join(destFolder,pngFile))
                    if i==sliceStart:
                        print 'image {}{} saved to png file "{}".'.format(dim,i,pngFile)

        inspectFile = '{}/nii_inspect.html'.format(scriptDir);
        with open(inspectFile, 'r') as fp:
            html = fp.read()
            for lr in [1,2,3]:
                try:
                    nifti_src = getattr(args,'nifti_lr{}'.format(lr));
                    if nifti_src is not None:
                        html = html.replace(r"/*$ if (!lr{}) lr{} = '' $*/".format(lr,lr),
                            r"if (!lr{}) lr{} = '{}';".format(lr,lr,redirect['lr{}'.format(lr)]))
                    title = getattr(args,'title_lr{}'.format(lr))
                    if title is not None:
                        html = html.replace(r"/*$ if (!ti{}) ti{} = 'layer{}' $*/".format(lr,lr,lr),
                            r"if (!ti{}) ti{} = '{}';".format(lr,lr,title))
                        
                except NameError:
                    raise
                    
            for d in [0,1,2]:
                dim = ['X','Y','Z'][d]
                html = html.replace("/*$ if (!slices{}) slices{} = '0:1:2' $*/".format(dim,dim),
                    "if (!slices{}) slices{} = '{}';".format(dim,dim,':'.join(str(s) for s in sliceRange[d])))
  
        htmlFile = op.join(destFolder,'index.html')
        with open(htmlFile, 'w') as fp:
            fp.write(html)
                
        print 'HTML viewer saved as "{}"'.format(htmlFile)

        """
        try:
          os.symlink(op.join(op.dirname(__file__),'snapshot.html'),op.join(destFolder,'snapshot.html'))
        except:
          import shutil
          shutil.copyfile(op.join(op.dirname(__file__),'snapshot.html'),op.join(destFolder,'snapshot.html'))
        """
        
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise


if __name__ == '__main__':
    args = parse_arguments()
    run(args)
