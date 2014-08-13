# Rembrandt Bakker, Dhruv Kohli, Piotr Majka, June 2014

import argparse, sys, os, numpy, subprocess, json, scipy
import os.path as op
import scipy
import re
import niitools as nit

SliceDirs = {'x':'saggital','y':'coronal','z':'axial'}

def argument_parser():
  """ Define the argument parser and return the parser object. """
  parser = argparse.ArgumentParser(
    description='description',
    formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
  parser.add_argument('-o','--out', type=str, help="Output folder (existing files will be overwritten)", required=True)
  parser.add_argument('-d','--dim', type=str, default='y', help="Slice direction: x=Saggital, y=Coronal, z=Axial")
  colormapHelp = """
      Colormap supports various formats:
      1. comma separated list of rgb-hex values: #RGB1,#RGB2,...
      2. range of rgb-hex values: #RGB1-#RGB2
      3. constant color with range of alpha values: alpha-#RGB
  """
  parser.add_argument('-c','--colormap', type=str, help=colormapHelp, required=True)
  parser.add_argument('--pctile', type=str, help='Rescale data using these min/max percentile values, e.q. "1,99". If omitted, no rescaling is done and image is saved as png.', required=False)
  return parser

def parse_arguments(raw=None):
  """ Apply the argument parser. """
  args = argument_parser().parse_args(raw)

  """ Basic argument checking """
  if not op.exists(args.nifti_src):
    print 'Nifti file "{}" not found.'.format(args.nifti_src)
    exit(0)

  global SliceDirs
  if SliceDirs[args.dim] is None:
    print 'Invalid slice direction "{}".'.format(args.dim)
    exit(0)

  return args

def run(args):
  print('Input Nifti: '+args.nifti_src)
  print('Colormap to use: '+args.colormap)
  try:
    import nibabel
    nii = nibabel.load(args.nifti_src)
    img = numpy.squeeze(nii.get_data())
    hdr = nii.get_header()
    q = hdr.get_best_affine();
    ornt = nibabel.io_orientation(q)
    img = nibabel.apply_orientation(img,ornt)
    dims = img.shape
    print 'Nifti image loaded, data type "{}"'.format(img.dtype)

    global SliceDirs
    slice_dir_orientation = SliceDirs[args.slice_dir]
    slice_dir_index = {'x':0,'y':1,'z':2}[args.slice_dir]
    numSlices = img.shape[slice_dir_index];
    maxSlices = 2048;
    if numSlices>maxSlices:
      raise Exception('too many slices (more than '+str(maxSlices)+')');

    baseName = op.basename(args.nifti_src)
    baseName = re.sub('.gz$', '',baseName)
    baseName = re.sub('.nii$', '',baseName)
    outFolder = args.out
    if not(op.exists(outFolder)):
      os.makedirs(outFolder)
    print 'Created output folder "{}".'.format(outFolder)

    if len(dims)==4:
      raise Exception('NIFTI file with RGB color data not supported yet.')

    index2rgb = nit.parse_colormap(img,args.colormap)
    if isinstance(index2rgb,dict):
        rgbLen = len(index2rgb[index2rgb.keys()[0]])
    else:
        rgbLen = len(index2rgb[0])

    rescale = bool(args.pctile)
    minmax = [None,None]
    if rescale:
        minmax = nit.get_limits(img,args.pctile)

    fmt = 'png'
    if rescale and rgbLen is 3:
        fmt = 'jpg'
    
    for i in range(0,numSlices):
        dim = args.dim
        slice = nit.get_slice(img,dim,i)
        if index2rgb:
            slice = nit.slice2rgb(slice,index2rgb,rescale,minmax[0],minmax[1])
                    
        # Save image
        outFile = baseName+'_{:04d}.{}'.format(i,fmt)
        scipy.misc.toimage(slice).save(op.join(outFolder,outFile))
            
        if i==0:
            print 'image {}{} saved to {} file "{}".'.format(dim,i,fmt,outFile)
            
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise


if __name__ == '__main__':
  args = parse_arguments()
  run(args)
