# Rembrandt Bakker, June 2014

import argparse, sys, os, numpy, subprocess
import os.path as op
import scipy
import re

SliceDirs = {'x':'saggital','y':'coronal','z':'axial'}

def argument_parser():
  """ Define the argument parser and return the parser object. """
  parser = argparse.ArgumentParser(
    description='description',
    formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
  parser.add_argument('-o','--svg_dest', type=str, help="Output SVG folder", required=True)
  parser.add_argument('-d','--slice_dir', type=str, default='y', help="Slice direction: x=Saggital, y=Coronal, z=Axial")
  return parser

def parse_arguments(raw=None):
  """ Apply the argument parser. """
  args = argument_parser().parse_args(raw)

  """ Basic argument checking """
  if not op.exists(args.nifti_src):
    print 'Nifti file "{}" not found.'.format(args.nifti_src)
    exit(0)

  global SliceDirs
  if SliceDirs[args.slice_dir] is None:
    print 'Invalid slice direction "{}".'.format(args.slice_dir)
    exit(0)

  return args
           
def run(args):
  print('Input Nifti: '+args.nifti_src)
  try:
    import nibabel
    nii = nibabel.load(args.nifti_src)
    img = numpy.squeeze(nii.get_data())
    dims = img.shape
    hdr = nii.get_header()
    q = hdr.get_best_affine();
    ornt = nibabel.io_orientation(q)
    img = nibabel.apply_orientation(img,ornt)
    print 'Nifti image loaded, data type "{}"'.format(img.dtype)

    global SliceDirs
    slice_dir_orientation = SliceDirs[args.slice_dir]
    slice_dir_index = {'x':0,'y':1,'z':2}[args.slice_dir]
    numSlices = img.shape[slice_dir_index];
    maxSlices = 1024;
    if numSlices>maxSlices:
      raise RuntimeError('too many slices (more than '+str(maxSlices)+')');

    baseName = op.basename(args.nifti_src)
    baseName = re.sub('.gz$', '',baseName)
    baseName = re.sub('.nii$', '',baseName)
    pngFolder = op.join(args.svg_dest,'{}_png'.format(slice_dir_orientation))
    svgFolder = op.join(args.svg_dest,'{}_svg'.format(slice_dir_orientation))
    if not(op.exists(pngFolder)):
      os.makedirs(pngFolder)
    if not(op.exists(svgFolder)):
      os.makedirs(svgFolder)
    print 'Created output folders "{}" and "{}".'.format(pngFolder,svgFolder)

    if len(dims)==4:
      print 'Error: NIFTI file with RGB color data not supported yet.'
      exit(0)

    for i in range(0,numSlices):
      """
         In the code below, the transpose is needed because images are written with the 
         first dimension as rows, second as columns. This must be flipped to align with
         the common definition of x- and y axes .
         The ::-1 part is a mirroring operation on the y-axis, which is needed because
         images are written top to bottom, reversed from the common y-axis direction.
      """
      if args.slice_dir is 'x':
        slice = img[i,:,::-1].squeeze().transpose();
      elif args.slice_dir is 'y':
        slice = img[:,i,::-1].squeeze().transpose();
      elif args.slice_dir is 'z':
        slice = img[:,::-1,i].squeeze().transpose();
  
      # Save image to PNG
      pngFile = baseName+'{:04d}.png'.format(i)
      scipy.misc.toimage(slice).save(op.join(pngFolder,pngFile))
      print 'image {} saved to png file "{}".'.format(i,pngFile)

      # Convert image to SVG
      """
      bmpimage = 'test.bmp'
      im.save(bmpimage)
      svgfile = 'test'+str(i)+'.svg'
      ans = subprocess.call([r"c:\users\rbakker\documents\htdocs\incf\autotrace\autotrace.exe",'-output-file',svgFolder+'/'+svgfile,'-background-color','000000','-corner-surround','0.5','-corner-threshold','180','-corner-always-threshold','180','-error-threshold','0.0','-tangent-surround','1','-filter-iterations','0',bmpimage])
      """
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise


if __name__ == '__main__':
  args = parse_arguments()
  run(args)
