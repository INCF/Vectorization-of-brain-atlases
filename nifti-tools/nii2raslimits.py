# Rembrandt Bakker, Dhruv Kohli, Piotr Majka, June 2014

import argparse, sys, numpy, json
import os.path as op
import re

def argument_parser():
    """ Define the argument parser and return the parser object. """
    parser = argparse.ArgumentParser(
        description="""
            Returns the limits in real world coordinates occupied by the nifti volume. 
            In voxel coordinates, this is spanned by the left-posterior-inferior corner (-0.5,-0.5,0.5)
            and the right-anterior-superior corner (dims-1)+(0.5,0.5,0.5) 
            These values are transformed to real world coordinates using the get_best_affine() matrix.
        """,
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-i','--nifti_src', type=str, help="Input Nifti file", required=True)
    parser.add_argument('-o','--out', type=str, help="Output rasLimits.json file", required=False)
    parser.add_argument('-v','--verbose', action="store_true", help="Be verbose", required=False)
    return parser


def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    """ Basic argument checking """
    if not op.exists(args.nifti_src):
        raise Exception('Nifti file "{}" not found.'.format(args.nifti_src))

    return args


def run(args):
    if args.verbose:
        def verbose(msg):
            print(msg)
    else:
        def verbose(msg):
            pass

    verbose('Input Nifti: '+args.nifti_src)
    try:
        import nibabel
        nii = nibabel.load(args.nifti_src)
        hdr = nii.get_header()
        q = hdr.get_best_affine();
        ornt = nibabel.io_orientation(q)
        verbose('q {}'.format(q))
        verbose('ornt {}'.format(ornt))
        #print 'hdr {}'.format(hdr)
        dims = hdr.get_data_shape()
        rasLimitsT = numpy.array([
            [-0.5,-0.5,-0.5],
            [dims[0]-0.5,dims[1]-0.5,dims[2]-0.5]
        ])
        rasLimits = nibabel.affines.apply_affine(q,rasLimitsT).T
        verbose('rasLimits {}'.format(rasLimits))
        for lim in rasLimits:
            if lim[1]<lim[0]: 
                tmp = lim[0]
                lim[0] = lim[1]
                lim[1] = tmp
                
        verbose('rasLimits {}'.format(rasLimits))
        if args.out:
            with open(args.out, 'w') as fp:
                json.dump(rasLimits.tolist(),fp)
        
        return rasLimits
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise
      

if __name__ == '__main__':
  args = parse_arguments()
  run(args)
