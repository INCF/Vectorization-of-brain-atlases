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
  parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
  parser.add_argument('-o','--path_dest', type=str, help="Output snapshot folder", required=True)
  return parser

def parse_arguments(raw=None):
  """ Apply the argument parser. """
  args = argument_parser().parse_args(raw)

  """ Basic argument checking """
  if not op.exists(args.nifti_src):
    print 'Nifti file "{}" not found.'.format(args.nifti_src)
    exit(0)

  return args
         
def get_slice(img,dim,i):
  if dim is 'x':
    slice = img[i,:,::-1].squeeze().transpose();
  elif dim is 'y':
    slice = img[:,i,::-1].squeeze().transpose();
  elif dim is 'z':
    slice = img[:,::-1,i].squeeze().transpose();
  return slice

def run(args):
  print('Input Nifti: '+args.nifti_src)
  try:
    import nibabel
    nii = nibabel.load(args.nifti_src)
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

    baseName = op.basename(args.nifti_src)
    baseName = re.sub('.gz$', '',baseName)
    baseName = re.sub('.nii$', '',baseName)
    destFolder = args.path_dest
    if not(op.exists(destFolder)):
      os.makedirs(destFolder)
      print 'Created destination folder "{}".'.format(destFolder)

    global SliceDirs
    numSamples = 9
    html = '<html><h2>Row labels assume that NIFTI has correct R.A.S. orientation</h2><table border="1">'
    html += '<tr><td></td>'
    for sample in range(0,numSamples):
      html += '<td>{}%</td>'.format(100*(sample+1)/(numSamples+1))
		
    for dim in ['x','y','z']:
      html += '<tr><td>{}</td>'.format(SliceDirs[dim])
      dim_orientation = SliceDirs[dim]
      dim_index = {'x':0,'y':1,'z':2}[dim]
      numSlices = dims[dim_index];
      for sample in range(0,numSamples):
        i = numSlices*(sample+1)/(numSamples+1)
        slice = get_slice(img,dim,i)

        # Save image to PNG
        pngFile = baseName+'_{}{:02d}.png'.format(dim,100*(sample+1)/(numSamples+1))
        scipy.misc.toimage(slice).save(op.join(destFolder,pngFile))
        print 'image {}{} saved to png file "{}".'.format(dim,sample,pngFile)
        html += '<td><img src="{}" style="padding:5px;width:{}%"></td>'.format(pngFile,90)
      
      html += '</tr>'
      
    html += '</table></html>'
    htmlFile = op.join(destFolder,baseName+'_snapshot.html')
    with open(htmlFile, "w") as fp:
      fp.write(html)
    print 'Snapshot saved as "{}"'.format(htmlFile)
    
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise


if __name__ == '__main__':
  args = parse_arguments()
  run(args)
