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
import datatank_py
from datatank_py.DTDataFile import DTDataFile
from datatank_py.DTProgress import DTProgress
from datatank_py.DTSeries import DTSeriesGroup


with DTDataFile('Input.dtbin') as hfile:
    tracks = array(hfile['tracks']).squeeze()
# tdpath = '/Users/jaynewby/Dropbox/CF/PT/Net-Tracker/TrackingTools/'
# sys.path.append(tdpath)
# from TrackingData import *


# print('')
# print(filename)
# print('')

Data = pd.DataFrame(tracks, columns=['x', 'y', 'z', 'frame', 'r', 'particle'])

N = int(Data.frame.size)
M = int(Data.particle.max())
paths = zeros((4, N + M))*nan
Na = 0
for p, g in Data.groupby('particle'):
    path = array((g[['x', 'y', 'z', 'frame']])).T
    _, Np = path.shape
    paths[:, Na:Na+Np] = path
    Na += Np + 1

outdict = {'Seq_output var': array([u'Array'], dtype='<U5'),
           'Var': paths}
savemat('Output.mat', outdict, format='4')
