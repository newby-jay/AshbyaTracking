#!/opt/local/bin/python2.7
import os
import sys
from numpy import array
from scipy.io import savemat, loadmat


# var = loadmat('input.mat')
# filename = var['filename'].flatten()[0]

filename = ' '.join(sys.argv[1:])
path, nameRaw = os.path.split(filename)
name = nameRaw.split('-videoMeta')[0]
outpath = os.path.join(path, name)


outdict = {'Seq_Var': array([u'String'], dtype='<U6'),
            'Var': array([outpath])}
savemat('output.mat', outdict, format='4')
