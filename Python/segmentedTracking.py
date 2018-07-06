#!/Users/jaynewby/anaconda/bin/python2.7
from __future__ import division
from __future__ import print_function
from pylab import *
from itertools import product, cycle, count, permutations, ifilter, repeat, izip, imap
import os
import time
from scipy.io import savemat, loadmat
sys.path.append('/Users/jaynewby/Dropbox/Gladfelter/segmentedTracking/')
import TrackingData as TD
# from vidUtils import *
# from batchUtils import *
# import netTrackerRNN as RNNnet
# import netTrackerTC as TCnet
import pandas as pd

import datatank_py
from datatank_py.DTDataFile import DTDataFile
from datatank_py.DTProgress import DTProgress
from datatank_py.DTSeries import DTSeriesGroup


with DTDataFile('Input.dtbin') as hfile:
    detectionData = array(hfile['localizations']).squeeze()
    segments = array(hfile['segments']).squeeze()
    rMAX = array(hfile['rMAX']).squeeze()
detectionsRaw = pd.DataFrame(detectionData.T,
                columns=['x', 'y', 'z', 't', 'p', 'r'])\
                .assign(segment=segments)\
                .assign(p=0.999)
detections = detectionsRaw[detectionsRaw.segment > 0]
vidshape = (int(detectionsRaw.t.max() + 1),
            int(ceil(detectionsRaw.y.max())),
            int(ceil(detectionsRaw.x.max())),
            int(ceil(detectionsRaw.z.max()))
            )
GEMStracks = pd.DataFrame([], columns=['x', 'y', 'z', 'frame', 'particle' 'segment', 'r'])
# NucleiTracks = pd.DataFrame([], columns=['x', 'y', 'z', 'frame', 'particle' 'segment', 'r'])


print(set(detections.segment))
np = 0
gp = 0
for s, g in detections.groupby('segment'):
    print('segment:', s)
    GEMS = TD.TrackingData(shape=vidshape, zscale=1.)
    GEMS.particleSet = g[g.r < rMAX]
    print(s, GEMS.particleSet.r.size)
    GEMS.particleSetGrouped = GEMS._makePSgrouped()
    GEMS.linkParticles(D=2., nprocs=2, skipLink=True)
    GEMS.Data.particle += gp
    GEMS.Data = GEMS.Data.assign(segment=s)
    numParticles = GEMS.Data.particle.max()
    if isfinite(numParticles):
        gp = numParticles + 1
    else:
        gp = 1
    GEMStracks = GEMStracks.append(GEMS.Data)
gbp = GEMStracks.groupby('particle')
Nparticles = gbp.ngroups
Ni, Nc = GEMStracks.shape
out1 = zeros((Ni, 7))
# assert False
print(out1.shape)
nb = 0
for pn, g in gbp:
    xyz = array(g[['x', 'y', 'z']])
    t = array(g.frame)
    s = array(g.segment)
    r = array(g.r)
    toAdd = c_[xyz, t, pn*ones_like(t), r, s]
    ne = t.size
    out1[nb:nb+ne] = toAdd
    nb += ne
print(out1.shape)
##################################################
# np = 0
# gp = 0
# for s, g in detections.groupby('segment'):
#     print('segment:', s)
#     Nuclei = TD.TrackingData(shape=vidshape, zscale=zscale)
#     Nuclei.particleSet = g[g.r>14.999995].copy()
#     Nuclei.particleSetGrouped = Nuclei._makePSgrouped()
#     Nuclei.linkParticles(D=1., nprocs=2, skipLink=True)#.filterPathsByLength(5)
#     Nuclei.Data.particle += np
#     Nuclei.Data = Nuclei.Data.assign(segment=s)
#     np = Nuclei.Data.particle.max() + 1
#     NucleiTracks = NucleiTracks.append(Nuclei.Data)
# gbp = NucleiTracks.groupby('particle')
# Nparticles = gbp.ngroups
# Ni, Nc = NucleiTracks.shape
# out2 = zeros((Ni, 6))
# nb = 0
# for p, g in gbp:
#     xyz = array(g[['x', 'y', 'z']])
#     t = array(g.frame)
#     s = array(g.segment)
#     toAdd = c_[xyz, t, p*ones_like(t), s]
#     ne = t.size
#     out2[nb:nb+ne] = toAdd
#     nb += ne


# with DTDataFile('Output.dtbin', truncate=True) as hfile:
#     dts = DTSeriesGroup(hfile, 'Var', {'GEMSdata':'Array', 'nucleiData':'Array'})
#     dts.add(0, {'GEMSdata': out1, 'nucleiData': zeros((2, 2))})

outdict = {'Seq_output var': array([u'Array'], dtype='<U5'),
           'Var': out1.T}
savemat('Output.mat', outdict, format='4')





# # gbp = GEMStracks.groupby('particle')
# # Nparticles = gbp.ngroups
# # Ni, Nc = GEMStracks.shape
# # out = zeros((Ni-2*Nparticles, 13))
# # nb = 0
# # for p, g in gbp:
# #     xyz = array(g[['x', 'y', 'z']])
# #     t = array(g.frame)
# #     s = array(g.segment)
# #     dxdydz = diff(xyz, axis=0)
# #     dt = diff(t)
# #     forward = c_[xyz[1:-1, :], t[1:-1], dxdydz[:-1, :], dt[:-1], dxdydz[1:, :], dt[1:], s[1:-1]]
# #     # out = r_[out, forward]
# #     # out = r_[out, nan*ones((1, 13))]
# #     ne = t.size-2
# #     out[nb:nb+ne] = forward
# #     nb += ne
# # outdict = {'Seq_paths': array([u'Array'], dtype='<U5'),
# #            'paths': out}
# # savemat(fn + '/trackingDataWithSegments.mat', outdict, format='4')
