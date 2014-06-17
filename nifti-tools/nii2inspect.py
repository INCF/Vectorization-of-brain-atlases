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
  parser.add_argument('-i','--nifti_scan', type=str, help="Input Nifti file (scan)", required=True)
  parser.add_argument('-j','--nifti_label', type=str, help="Input Nifti file (label)", required=False)
  parser.add_argument('-k','--nifti_other', type=str, help="Input Nifti file (other overlay)", required=False)
  parser.add_argument('-o','--path_dest', type=str, help="Output snapshot folder", required=True)
  parser.add_argument('-c','--colormap_scan', type=str, help="JSON colormap file (scan)", required=False)
  parser.add_argument('-d','--colormap_label', type=str, help="JSON colormap file (label)", required=False)
  parser.add_argument('-sx','--slices_x', type=str, help="Slices in the x-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sy','--slices_y', type=str, help="Slices in the y-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sz','--slices_z', type=str, help="Slices in the z-dimension, start%:step%:stop%", required=False)
  return parser

def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    try:
        """ Basic argument checking """
        if not args.nifti_scan and not args.nifti_label:
            raise Exception('At least one Nifti file must be specified (scan or label)')
      
        if args.nifti_scan and not op.exists(args.nifti_scan):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_scan))

        if args.nifti_label and not op.exists(args.nifti_label):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_label))

        if args.nifti_other and not op.exists(args.nifti_other):
            raise Exception('Nifti file "{}" not found.'.format(args.nifti_other))

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
    print('Input Nifti scan: {}'.format(args.nifti_scan))
    print('Input Nifti label: {}'.format(args.nifti_label))
    try:
        destFolder = args.path_dest
        if not(op.exists(destFolder)):
            os.makedirs(destFolder)
            print 'Created destination folder "{}".'.format(destFolder)
        destFolder = op.realpath(destFolder)
        scriptDir = op.realpath(op.dirname(__file__))

        redirect = {}
        for mode in ['scan','label','other']:
            nifti_src = getattr(args,'nifti_'+mode);
            if not nifti_src: continue
            colormap = getattr(args,'colormap_'+mode);
                
            import nibabel
            nii = nibabel.load(nifti_src)
            img = numpy.squeeze(nii.get_data())
            hdr = nii.get_header()
            q = hdr.get_best_affine();
            ornt = nibabel.io_orientation(q)
            print 'Orientation: {}'.format(ornt)
            img = nibabel.apply_orientation(img,ornt)
            dims = img.shape
            print 'Nifti image loaded, data type "{}"'.format(img.dtype)

            if len(dims)==4:
                print 'Error: NIFTI file with RGB color data not supported yet.'
                exit(0)

            baseName = op.basename(nifti_src)
            baseName = re.sub('.gz$', '',baseName)
            baseName = re.sub('.nii$', '',baseName)
            if colormap: fmt = 'png' 
            else: fmt = 'jpg'
            #redirect[mode] = op.relpath(op.join(destFolder,baseName),scriptDir)+'.'+fmt;
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
                sliceStart = int(args.sliceRangePct[d][0]*numSlices/100)
                sliceEnd = int(args.sliceRangePct[d][2]*numSlices/100)
                sliceRange[d] = [sliceStart,sliceStep,sliceEnd]
                for i in range(sliceStart,sliceEnd,sliceStep):
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
                    print 'image {}{} saved to png file "{}".'.format(dim,i,pngFile)

        inspectFile = '{}/nii_inspect.html'.format(scriptDir);
        with open(inspectFile, 'r') as fp:
            html = fp.read()
            if args.nifti_scan:
                html = html.replace("/*$ if (!lr1) lr1 = '' $*/","if (!lr1) lr1 = '{}';".format(redirect['scan']))
            if args.nifti_label:
                html = html.replace("/*$ if (!lr2) lr2 = '' $*/","if (!lr2) lr2 = '{}';".format(redirect['label']))
            if args.nifti_other:
                html = html.replace("/*$ if (!lr3) lr3 = '' $*/","if (!lr3) lr3 = '{}';".format(redirect['other']))
            #html = html.replace("/*$ if (!bins) bins = 0 $*/","if (!bins) bins = {}".format(numSamples))
            html = html.replace("/*$ if (!slicesX) slicesX = '0:1:2' $*/","if (!slicesX) slicesX = '{}';".format(':'.join(str(s) for s in sliceRange[0])))
            html = html.replace("/*$ if (!slicesY) slicesY = '0:1:2' $*/","if (!slicesY) slicesY = '{}';".format(':'.join(str(s) for s in sliceRange[1])))
            html = html.replace("/*$ if (!slicesZ) slicesZ = '0:1:2' $*/","if (!slicesZ) slicesZ = '{}';".format(':'.join(str(s) for s in sliceRange[2])))
  

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
