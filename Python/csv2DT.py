#!/opt/local/bin/python2.7
from __future__ import division
from __future__ import print_function
from pylab import *
import pandas as pd
import os
import subprocess
import time
import sys
from scipy.io import savemat, loadmat
# tdpath = '/Users/jaynewby/Dropbox/CF/PT/Net-Tracker/TrackingTools/'
# sys.path.append(tdpath)
# from TrackingData import *

filename = ' '.join(sys.argv[1:])

# print('')
# print(filename)
# print('')

Data = pd.read_csv(filename, index_col=0)

N = Data.frame.size
M = Data.particle.max()
paths = zeros((3, N + M))*nan
Na = 0
for p, g in Data.groupby('particle'):
    Np = g.frame.size
    path = array((g[['x', 'y', 'frame']]))
    paths[:, Na:Na+Np] = path.T
    Na += Np + 1

outdict = {'Seq_paths': array([u'Array'], dtype='<U5'),
            'paths': paths}
savemat('csv2DToutput.mat', outdict, format='4')
