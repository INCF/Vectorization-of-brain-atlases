# Rembrandt Bakker, June 2014

import argparse, sys, os, numpy, subprocess
import os.path as op
import re

SliceDirs = {'x':'saggital','y':'coronal','z':'axial'}

def argument_parser():
  """ Define the argument parser and return the parser object. """
  parser = argparse.ArgumentParser(
    description='description',
    formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
  parser.add_argument('-r','--reorient', type=str, default='+X+Y+Z', help="Requested flipping and permutation")
  return parser

def parse_arguments(raw=None):
  """ Apply the argument parser. """
  args = argument_parser().parse_args(raw)

  """ Basic argument checking """
  if not op.exists(args.nifti_src):
    print 'Nifti file "{}" not found.'.format(args.nifti_src)
    exit(0)
    
  args.reorient = args.reorient.upper()

  return args
           
def run(args):
  print('Input Nifti: '+args.nifti_src)
  try:
    import nibabel
    nii = nibabel.load(args.nifti_src)
    hdr = nii.get_header()
    dims = hdr.get_data_shape()
    qf = hdr.get_qform()
    sf = hdr.get_sform()
    
    perm_str = args.reorient
    if len(perm_str) is not 2*len(dims):
      raise Exception('Permutation string must be twice the length of the data, e.g. "+x+y+z"')
    
    Multiply = {'+':1,'-':-1}
    DirIndex = {'X':0,'Y':1,'Z':2}
    flip = []
    perm = []
    odd = True
    for ch in perm_str:
      if odd: flip.append(Multiply[ch])
      else: perm.append(DirIndex[ch])
      odd = not odd
    perm.append(len(dims))
    flip.append(1)
    
    print('Permute: {}'.format(perm))
    print('Flip: {}'.format(flip))
    print('Qform before: {}'.format(qf))
    qf = qf[perm,:]
    qf = (flip*qf.T).T
    hdr.set_qform(qf)
    print('Qform after: {}'.format(qf))
    
    print('Sform before: {}'.format(sf))
    sf = sf[perm,:]
    sf = (flip*sf.T).T
    hdr.set_sform(sf)
    print('Sform after: {}'.format(sf))
    
    outFile = re.sub('\.nii($)|\.nii(\.gz$)','_ras.nii.gz',args.nifti_src)
    nibabel.nifti1.save(nibabel.nifti1.Nifti1Image(nii.get_data(),None,hdr),outFile)
    print 'Saved image with reoriented header to "{}" '.format(outFile)
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise


if __name__ == '__main__':
  args = parse_arguments()
  run(args)
