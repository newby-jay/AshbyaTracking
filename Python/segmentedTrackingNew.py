#!/anaconda3/envs/py2/bin/python2.7
from __future__ import division
from __future__ import print_function
from numpy import *
import os
import time
import scipy
from scipy.io import savemat, loadmat
import sys

import NetTracker as NT
from NetTracker.TrackingData import TrackingData
from lap import lapmod

import pandas as pd

import datatank_py
from datatank_py.DTDataFile import DTDataFile
from datatank_py.DTProgress import DTProgress
from datatank_py.DTSeries import DTSeriesGroup

if sys.version_info.major == 3:
    izip = zip
    imap = map
else:
    from itertools import izip, imap

class SparseArrayFloat32:
    def __init__(self):
        self.I = zeros((0), 'int64')
        self.J = zeros((0), 'int64')
        self.A = zeros((0), 'float32')
        self.size = 0
    def _validateData(self, I, J, A):
        assert all(I.shape == J.shape)
        assert all(J.shape == A.shape)
        assert I.ndim == 1
        assert all(I >= 0)
        assert all(J >= 0)
        assert len(set(zip(I, J))) == I.size, 'indices must be unique'
        assert all(isfinite(I))
        assert all(isfinite(J))
        assert all(isfinite(A))
        return True
    def addData(self, I, J, A):
        I = array(I, 'int64')
        J = array(J, 'int64')
        A = float32(A)
        assert all(I.shape == J.shape)
        assert all(J.shape == A.shape)
        if I.size == 0:
            return self
        # self._validateData(I, J, A)
        if self.size == 0:
            self.I, self.J, self.A = I, J, A
        else:
            self.I = r_[self.I, I]
            self.J = r_[self.J, J]
            self.A = r_[self.A, A]
        self.size = self.I.size
        self._validateData(self.I, self.J, self.A)
        return self
    def convert(self):
        inds = lexsort((self.I, self.J))
        kk = int32(self.I[inds])
        jj = int32(self.J[inds])
        vals = float32(self.A[inds])
        first = int32(r_[0, arange(self.size)[r_[0, diff(jj)] != 0], vals.size])
        assert kk.max() == first.size - 2
        assert kk.min() == 0
        assert jj.max() == first.size - 2
        assert jj.min() == 0
        assert all(first >= 0)
        assert all(first <= vals.size)
        assert len(set(first)) == first.size
        return float64(vals), kk, first

def filterAggs():
    """Filter large, bright localizations from the set of detections."""
    GEMStracks = pd.DataFrame(
        [],
        columns=['x', 'y', 'z', 'frame', 'particle' 'segment', 'r', 'Ipeak'])
    np = 0
    gp = 0
    for s, g in detections.groupby('segment'):
        GEMS = TrackingData(shape=vidshape, zscale=1.)
        GEMS.setDetections(g)
        GEMS.linkParticles(D=1.0)
        GEMS.Data.particle += gp
        GEMS.Data = GEMS.Data.assign(segment=s)
        numParticles = GEMS.Data.particle.max()
        if isfinite(numParticles):
            gp = numParticles + 1
        else:
            gp = 0 + 1
        GEMStracks = GEMStracks.append(GEMS.Data)
    gbp = GEMStracks.groupby('particle')
    Nparticles = gbp.ngroups
    ## Filter tracks based on radius and intensity
    Pfilter = ones(Nparticles)
    NlocsFiltered = 0
    NparticlesFiltered = 0
    n = 0
    for pn, g in gbp:
        Ipeak = g.Ipeak.mean()
        r = g.r.mean()
        # large and bright
        if g.x.size > 4 and r > rMAX and Ipeak > Imax:
            Pfilter[n] = 0
            NlocsFiltered += g.x.size
            NparticlesFiltered += 1
        # very large and dim
        # elif g.x.size > 10 and r > rMAX*1.5:
        #     Pfilter[n] = 0
        #     NlocsFiltered += g.x.size
        #     NparticlesFiltered += 1
        # slightly large and very bright
        elif g.x.size > 4 and r > rMAX/2. and Ipeak > Imax*2:
            Pfilter[n] = 0
            NlocsFiltered += g.x.size
            NparticlesFiltered += 1
        else:
            pass
        n += 1
    Ni, Nc = GEMStracks.shape
    print(NlocsFiltered/Ni, 'of localizations fitered')
    #assert NlocsFiltered/Ni < 0.1, 'safeguard for <5% of localizations filtered'
    ## Combine tracking data into an output array
    Ni -= NlocsFiltered
    aggs = zeros((NlocsFiltered, 3))
    nb = 0
    n = 0
    for pn, g in gbp:
        if Pfilter[n] == 1:
            n += 1
            continue
        xyz = array(g[['x', 'y', 'z']])
        t = array(g.frame)
        # s = array(g.segment)
        # toAdd = c_[xyz, t, s]
        ne = t.size
        aggs[nb:nb+ne] = xyz#toAdd
        nb += ne
        n += 1
    ## Filter out the aggregates from `detections`
    cols = ['x', 'y', 'z', 't', 'p', 'r', 'Ibg', 'Ipeak', 'SNR', 'segment']
    filteredDetectionData = array([
        p for p in array(detections[cols]) if not p[:3] in aggs
    ])
    return pd.DataFrame(filteredDetectionData, columns=cols)

class KDE3D:
    """A 3D kernel density estimator for diffusivity."""
    def __init__(self, points, values, sigma):
        Npoints, _ = points.shape
        self.sigma = sigma
        assert points.shape[1] == 3
        assert Npoints == values.size
        self.points = points
        self.values = values
        assert all(isfinite(self.values))
        # self.D = zeros(Npoints)
        # self.Z = zeros(Npoints)
        # for n in arange(Npoints):
        #     x = points[n]
        #     v = values[n]
        #     D = 0.
        #     Z = 0.
        #     for m in arange(Npoints):
        #         xm = points[m]
        #         vm = values[m]
        #         w = exp(-((x - xm)**2).sum()/(2.*self.sigma**2))
        #         D += v*w
        #         Z += w
        #     self.D[n] = D
        #     self.Z[n] = Z
    # def __call__(self, n):
    #     n = int(n)
    #     assert 0 <= n < self.D.size
    #     return self.D[n]/self.Z[n]
    def evalAtPoint(self, x):
        """Estimate diffusivity at location x."""
        # x = array(x)
        # assert x.size == 3
        w = exp(-((x - self.points)**2).sum(axis=1)/(2.*self.sigma**2))
        out = (self.values*w).sum()/w.sum()
        if isfinite(out):
            return out
        else:
            assert all(isfinite(self.values))
            assert self.values.size > 0
            return self.values.mean()
        # for p, v in zip(self.points, self.values):
        #     w = exp(-((x - p)**2).sum()/(2.*self.sigma**2))
        #     D += v*w
        #     Z += w
        # return D/Z
def linkCost(dr2, dt, dframe, D, tscale):
    tau = dt - dframe + 1
    return dr2/D/tau + D*(dframe-1)/tscale
    # return dr2/D + D*(dt-1)/tscale

def makeCostMatrix(Data, KDE, fLinkScale=1., birth=0, death=0, distMax=70, maxFrameSkip=4):
    """Generate cost matrix for linking.
    Data: array of shape Nparticles x 4 (x, y, z, t).
    fLinkScale: timescale for linking.
    birth: parameter, weight for cost of not linking the start of a track.
    death: parameter, weight for cost of not linking the end of a track."""
    Nparticles, _ = Data.shape
    # if twoD:
    #     xyz = Data[:, :3]
    # else:
    #     xyz = Data[:, :3]
    xyz = Data[:, :3]
    frame = Data[:, 3]
    time = Data[:, 4]
    C = SparseArrayFloat32()
    D = array([KDE.evalAtPoint(xn) for xn in xyz])
    I, J, L = [], [], []
    for end in arange(Nparticles):
        tend = time[end]
        fend = frame[end]
        xend = xyz[end]
        for s in arange(Nparticles):
            if s == end:
                continue
            ts = time[s]
            fs = frame[s]
            xs = xyz[s]
            Ds = D[s]#0.5*(D[end] + D[s])
            if not fend < fs <= fend + maxFrameSkip:
                continue
            d = ((xend - xs)**2).sum()
            dt = ts - tend
            df = fs - fend
            assert isfinite(d) and d >= 0
            if sqrt(d) > distMax:
                continue
            assert isfinite(Ds)
            assert dt > 0 and isfinite(dt)
            if Ds == 0:
                l = 1e6-0.1
            else:
                l = min(1e6-0.1, 1. + linkCost(d, dt, df, Ds, fLinkScale))
            I.append(end)
            J.append(s)
            L.append(l)
    I, J, L = array(I), array(J), array(L)
    C.addData(I, J, L)
    C.addData(Nparticles + J, Nparticles + I, 0*L)
    I = arange(Nparticles)
    J = Nparticles + I
    C.addData(I, J, death*ones(Nparticles))
    C.addData(J, I, birth*ones(Nparticles))
    assert all(isnan(C.A) == False)
    assert all(C.A >= 0)
    C.A += 1. # Arbitrary shift
    C.A[C.A >= 1e6] = 1e6-0.1
    return C

def updateCostMatrix(C, Data, KDE, fLinkScale=0):
    """Update link costs in upper left Nparticles X Nparticles block.
    C: sparse matrix to update
    Data: array of shape Nparticles x 4 (x, y, z, t).
    KDE: diffusivity estimate to base the updated link costs
    fLinkScale: timescale for linking.
    """
    Nparticles, _ = Data.shape
    xyz = Data[:, :3]
    frame = Data[:, 3]
    time = Data[:, 4]
    D = array([KDE.evalAtPoint(xn) for xn in xyz])
    assert all(isfinite(D))
    inds = (C.I < Nparticles)&(C.J < Nparticles)
    for n in arange(C.A.size)[inds]:
        end = C.I[n]
        s = C.J[n]
        tend = time[end]
        fend = frame[end]
        xend = xyz[end]
        ts = time[s]
        fs = frame[s]
        xs = xyz[s]
        Ds = D[s]#0.5*(D[end] + D[s])
        d = ((xend - xs)**2).sum()
        dt = ts - tend
        df = fs - fend
        assert isfinite(d) and d >= 0
        # assert Ds > 0
        assert isfinite(Ds)
        if Ds == 0:
            C.A[n] = 1e6-0.1
        else:
            C.A[n] = min(1e6-0.1, 1. + linkCost(d, dt, df, Ds, fLinkScale))
    return C

def collectGroups(E, S):
    """Given a set of links, search through the detections and collect those
    belonging to the same track (by following the links). Each group
    corresponds to a track."""
    Nlinks = S.size
    r = arange(Nlinks)
    Groups = []
    collectedParticles = []
    for j in arange(Nlinks):
        e, s = E[j], S[j]
        if e in collectedParticles or s in collectedParticles:
            continue
        group = [e, s]
        collectedParticles.append(e)
        collectedParticles.append(s)
        while s in E:
            ind = r[s == E][0]
            s = S[ind]
            if s in collectedParticles:
                continue
            group.append(s)
            collectedParticles.append(s)
        Groups.append(group)
    return Groups

def linker(Nparticles, Cs):
    """Link a set of detections."""
    assert all(Cs.A > 0)
    assert all(isfinite(Cs.A))
    assert all(isnan(Cs.A) == False)
    vals, kk, offsets = Cs.convert()
    _, start = array(lapmod(2*Nparticles, vals, offsets, kk, return_cost=False))
    end = arange(start.size)
    linkinds = (start < Nparticles) & (end < Nparticles)
    S, E = start[linkinds], end[linkinds]
    return S, E

def sliceTimeDelay(z):
    n = floor(z/zscale)
    return n*sliceScale
################################################################################
################################################################################
#### Init
with DTDataFile('Input.dtbin') as hfile:
    detectionData = array(hfile['localizations']).squeeze()
    segments = array(hfile['segments']).squeeze()
    rMAX = array(hfile['rMAX']).squeeze()
    Imax = array(hfile['Imax']).squeeze()
    zscale = array(hfile['zscale']).squeeze()
    #Nz = array(hfile['Nz']).squeeze()
    #dtSlice = array(hfile['dtSlice']).squeeze()
    DT = array(hfile['dt']).squeeze()
    sliceScale = array(hfile['sliceScale']).squeeze()
    DXY = array(hfile['dxy']).squeeze()
detectionsRaw = pd.DataFrame(detectionData.T,
                columns=[
                    'x', 'y', 'z', 't', 'p',
                    'r', 'Ibg', 'Ipeak', 'SNR'])\
                .assign(segment=segments)\
                .assign(p=0.999)
detections = detectionsRaw[detectionsRaw.segment > 0]
vidshape = (int(detectionsRaw.t.max() + 1),
            int(ceil(detectionsRaw.y.max())),
            int(ceil(detectionsRaw.x.max())),
            int(ceil(detectionsRaw.z.max()))
            )
detections = filterAggs()
detections = detections.rename(columns={'t': 'frame'})
delays = sliceTimeDelay(array(detections.z))
detections = detections.assign(t = array(detections.frame) + delays)
sigma = 20.
class KDEconst:
    def __init__(self):
        self.D = 0.1*DT*0.11**2/DXY**2
    def evalAtPoint(self, x):
        return self.D
### Process each segment
outlist = []
pn = 0
for seg, g in detections.groupby('segment'):
    assert seg > 0
    Data = array(g[['x', 'y', 'z', 'frame', 't']])
    assert Data.size > 1
    if Data.size < 2:
        continue
    radii = array(g.r)
    AllPoints = Data[:, :3]
    frames = Data[:, 3]
    times = Data[:, 4]
    Nparticles, _ = Data.shape
    #### Initialization
    C = makeCostMatrix(
        Data,
        KDEconst(),
        fLinkScale=1.,
        birth=3.,
        death=3.,
        distMax=35,
        maxFrameSkip=1)
    S, E = linker(Nparticles, C)
    #### EM iterations
    C = makeCostMatrix(
        Data,
        KDEconst(),
        birth=20.,
        death=20.,
        distMax=40,
        maxFrameSkip=10)
    for i in arange(12):
        points = AllPoints[S]
        xy = AllPoints[:, :2]
        values = ((xy[S] - xy[E])**2).sum(axis=1)/(4.*(times[S] - times[E]))
        assert all(values >= 0.) and all(isfinite(values))
        assert points.shape[0] == values.size
        ####
        if values.size > 0:
            KDE = KDE3D(points, values, sigma)
        else:
            KDE = KDEconst()
        ####
        C = updateCostMatrix(C, Data, KDE, fLinkScale=0.5)
        S, E = linker(Nparticles, C)
        ####
    ############################################################################
    ############################################################################
    ### Format for DataTank output
    Nnew = S.size
    if Nnew == 0:
        continue
    groups = collectGroups(E, S)
    Nnew = 0
    for group in groups:
        Nnew += len(group)
    formated_out = zeros((Nnew, 7))
    n = 0
    for group in groups:
        for ind in group:
            formated_out[n] = r_[
                AllPoints[ind],
                times[ind],
                pn,
                radii[ind],
                seg]
            n += 1
        pn += 1
    outlist.append(formated_out)
out = concatenate(outlist, axis=0)

outdict = {'Seq_output var': array([u'Array'], dtype='<U5'),
           'Var': out.T}
savemat('Output.mat', outdict, format='4')
