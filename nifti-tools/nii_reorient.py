# Rembrandt Bakker, June 2014

import argparse, sys, os, numpy, subprocess
import os.path as op
import re

def argument_parser():
    """ Define the argument parser and return the parser object. """
    parser = argparse.ArgumentParser(
        description='description',
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
    parser.add_argument('-o','--nifti_dest', type=str, help="Output Nifti file (otherwise *_ras.nii.gz)", required=False)
    parser.add_argument('-r','--reorient', type=str, default='+i+j+k', help="Requested flipping and permutation")
    return parser

def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    """ Basic argument checking """
    if not op.exists(args.nifti_src):
        print 'Nifti file "{}" not found.'.format(args.nifti_src)
        exit(0)
    if not args.nifti_dest:
        args.nifti_dest = re.sub('\.nii($)|\.nii(\.gz$)','_ras.nii.gz',args.nifti_src)
    print 'Parse_Arguments {}'.format(args.nifti_dest)
    args.reorient = args.reorient.lower()

    return args
           
def run(args):
    print('Input Nifti: '+args.nifti_src)
    try:
        import nibabel
        nii = nibabel.load(args.nifti_src)
        hdr = nii.get_header()
        dims = hdr.get_data_shape()
        dims = [x for x in dims if x > 1]
        print "Dims {}".format(dims)
    
        perm_str = args.reorient
        if len(perm_str) is not 2*len(dims):
            raise Exception('Permutation string must be twice the length of the data, e.g. "+i+j+k"')
    
        Multiply = {'+':1,'-':-1}
        DirIndex = {'i':0,'j':1,'k':2}
        flip = []
        perm = []
        odd = True
        for ch in perm_str:
            if odd: 
                flip.append(Multiply[ch])
            else: 
                perm.append(DirIndex[ch])
            odd = not odd
        perm.append(len(dims))
        flip.append(1)
    
        print('Permute: {}'.format(perm))
        print('Flip: {}'.format(flip))
    
        qf,qfc = hdr.get_qform(coded=True)
        sf,sfc = hdr.get_sform(coded=True)
    
        print('Qform before:\n{}'.format(qf))
        if (qf is None):
            qf = hdr.get_qform(coded=False)
            print('Qform reset to:\n{}'.format(qf))
        if (qf is not None):
            qf = qf[perm,:]
            print 'qf.T {}'.format(qf.T)
            qf = (flip*qf.T).T
            hdr.set_qform(qf)
            print('Qform after:\n{}'.format(qf))
    
        print('Sform before:\n{}'.format(sf))
        if (sf is not None):
            sf = sf[perm,:]
            sf = (flip*sf.T).T
            hdr.set_sform(sf)
            print('Sform after:\n{}'.format(sf))
    
        outFile = args.nifti_dest
        nibabel.nifti1.save(nibabel.nifti1.Nifti1Image(nii.get_data(),None,hdr),outFile)
        print 'Saved image with reoriented header to "{}" '.format(outFile)
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise


if __name__ == '__main__':
    args = parse_arguments()
    run(args)
