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
    description='description',
    formatter_class=argparse.RawTextHelpFormatter)
  parser.add_argument('-i','--nifti_json', nargs='+', type=json.loads, help="Input Nifti json struct with fields 'file','title' and 'colormap'")
  parser.add_argument('-o','--path_dest', type=str, help="Output snapshot folder", required=True)
  parser.add_argument('-sx','--slices_x', type=str, help="Slices in the x-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sy','--slices_y', type=str, help="Slices in the y-dimension, start%:step%:stop%", required=False)
  parser.add_argument('-sz','--slices_z', type=str, help="Slices in the z-dimension, start%:step%:stop%", required=False)
  return parser

def parse_arguments(raw=None):
    """ Apply the argument parser. """
    args = argument_parser().parse_args(raw)

    try:
        print "nifti_json {}".format(args.nifti_json)

        """ Basic argument checking """
        args.layers = args.nifti_json
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
    print('Layer specification: {}'.format(args.layers))
    try:
        destFolder = args.path_dest
        if not(op.exists(destFolder)):
            os.makedirs(destFolder)
            print 'Created destination folder "{}".'.format(destFolder)
        destFolder = op.realpath(destFolder)
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
            mpc_cmap = None
            if "colormap" in lr:
                cmap = lr["colormap"]
                matches = re.search('^(#[0-9a-fA-F]+)-(#[0-9a-fA-F]+)$',cmap)
                if matches:
                    import matplotlib.colors as mpc
                    c1 = mpc.hex2color(matches.group(1))
                    c2 = mpc.hex2color(matches.group(2))
                    mpc_cmap = mpc.LinearSegmentedColormap(baseName,{
                        'red':[(0.0,c1[0],c1[0]),(1.0,c2[0],c2[0])],
                        'green':[(0.0,c1[1],c1[1]),(1.0,c2[1],c2[1])],
                        'blue':[(0.0,c1[2],c1[2]),(1.0,c2[2],c2[2])],
                    })
                    #print('mpc_cmap {}'.format(mpc_cmap(numpy.arange(256))))
                else:
                    fmt = 'png'
                    if cmap[0] is '#':
                        cmap = cmap.split(',')
                        cmap = dict((str(i),val) for i,val in enumerate(cmap))
                    else:
                        if not op.exists(cmap):
                            raise('JSON colormap file "{}" not found.'.format(cmap))
                      
                        with open(cmap,'r') as fp:
                            cmap = json.load(fp)
                  
                    index2rgb = {};
                    for a in cmap:
                        index2rgb[a] = hex2rgba(cmap[a])
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
                    
                    pngFile = baseName+'_{}{:d}.{}'.format(dim,i,fmt)
                    if mpc_cmap:
                        import matplotlib as mpl
                        import matplotlib.cm as cm
                        norm = mpl.colors.Normalize(vmin=img_min, vmax=img_max)
                        m = cm.ScalarMappable(norm=norm, cmap=mpc_cmap)
                        slice = m.to_rgba(slice)
                        #import Image
                        #slice = (slice-img_min)/(img_max-img_min)
                        #if i==0: print 'A {}'.format(numpy.uint8(mpc_cmap(slice)*255))
                        #im = Image.fromarray(numpy.uint8(mpc_cmap(slice)*255))
                        #im.save(pngFile)
                        
                        #print 'VIA MPC_MAP'

                    else:
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
                    scipy.misc.toimage(slice).save(op.join(destFolder,pngFile))
                        
                    if i==sliceStart:
                        print 'image {}{} saved to png file "{}".'.format(dim,i,pngFile)

            # update parsedLayers
            pl = {
              "name": baseName,
              "ext": fmt
            }
            if "title" in lr:
                pl["title"] = lr["title"];
            parsedLayers.append(pl);

        inspectFile = '{}/nii_inspect.html'.format(scriptDir);
        with open(inspectFile, 'r') as fp:
            html = fp.read()
            html = html.replace(r"var defaultLayers = [];",
                r"var defaultLayers = {};".format(json.dumps(parsedLayers)))
                            
            #for lr in [1,2,3]:
            #    try:
            #        nifti_src = getattr(args,'nifti_lr{}'.format(lr));
            #        if nifti_src is not None:
            #            
            #            html = html.replace(r"/*$ if (!lr{}) lr{} = '' $*/".format(lr,lr),
            #                r"if (!lr{}) lr{} = '{}';".format(lr,lr,redirect['lr{}'.format(lr)]))
            #        title = getattr(args,'title_lr{}'.format(lr))
            #        if title is not None:
            #            html = html.replace(r"/*$ if (!ti{}) ti{} = 'layer{}' $*/".format(lr,lr,lr),
            #                r"if (!ti{}) ti{} = '{}';".format(lr,lr,title))
            #            
            #    except NameError:
            #        raise
                    
            html = html.replace(r"var defaultSliceRange = [];",
                "var defaultSliceRange = {};".format(json.dumps(sliceRange)))
  
        htmlFile = op.join(destFolder,'index.html')
        with open(htmlFile, 'w') as fp:
            fp.write(html)
                
        print 'HTML viewer saved as "{}"'.format(htmlFile)
        
    except:
        print "Unexpected error:", sys.exc_info()[0]
        raise


if __name__ == '__main__':
    args = parse_arguments()
    run(args)
