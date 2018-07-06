from __future__ import division
from __future__ import print_function
from pylab import *
import pandas as pd
from scipy.optimize import minimize_scalar
from itertools import product, cycle, count, permutations, ifilter, repeat, izip, imap
from multiprocessing import Pool
from _hungarian import hungarian
# from pims_nd2 import ND2_Reader
# from PIL import Image
# from PIL.ImageSequence import Iterator
# from scipy.sparse import lil_matrix, csc_matrix, coo_matrix
from contextlib import closing
from scipy.io import savemat, loadmat
from numba import jit

def parallel(function, iterable, nprocs, args=(), chunksize=50):
    if len(args) > 0:
        iterable = zip(iterable, repeat(args))
    if nprocs == 1:
        return imap(function, iterable)
    pool = Pool(nprocs)
    # with closing(Pool(nprocs)) as pool:
    rv = pool.imap(function, iterable, chunksize)
    pool.close()
    return rv
def linkFrame(args):
    P, params = args
    D = params[0]
    zscale = params[1]
    pnow, pback, rnow, rback = P
    if rnow.size == 0 or rback.size == 0:
        return array([[]])
    Nb, _ = rback.shape
    Nn, _ = rnow.shape
    def makeCostMatrix():
        Xnow, Xback = meshgrid(rnow[:, 0], rback[:, 0])
        Ynow, Yback = meshgrid(rnow[:, 1], rback[:, 1])
        Znow, Zback = meshgrid(rnow[:, 2], rback[:, 2])
        Pnow, Pback = meshgrid(pnow, pback)
        logL = ((Xnow - Xback)**2 + (Ynow - Yback)**2
                + zscale**2*(Znow - Zback)**2)/(4.*D)
        logL -= log(Pnow) + log(Pback)
        cutOff = 30.**2/(4.*D)
        logL[logL > cutOff] = inf
        Nb, Nn = logL.shape
        death = -log(1.-pback)
        birth = -log(1.-pnow)
        Death, Birth = diag(death), diag(birth)
        Death[Death == 0] = inf
        Birth[Birth == 0] = inf
        min1 = min(death.min(), birth.min())
        R = min(min1, logL.min())*ones((Nn, Nb))
        R[logL.T > cutOff] = inf
        c = r_[c_[logL, Death],
               c_[Birth,    R]]
        return float32(c)
    c = makeCostMatrix()
    assert sum(isnan(c)) == 0
    now = array(hungarian(c))
    back = arange(now.size)
    linkinds = (back<Nb)*(now<Nn)
    MLlinks = array([back[linkinds], now[linkinds]]).T
    return MLlinks
@jit(nopython=True)
def connectedComponents(args):
    ## input is a 4xN array (x, y, z, p)
    data, shape = args
    Ny, Nx, Nz = shape
    _, N = data.shape
    # nn = array([(-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0),
    #             (-1, 0, 1), (1, 0, 1), (0, -1, 1), (0, 1, 1),
    #             (-1, 0, -1), (1, 0, -1), (0, -1, -1), (0, 1, -1)])
    nn = array([(-1, 0, 0), (1, 0, 0), (0, -1, 0), (0, 1, 0),
                (0, 0, -1), (0, 0, 1)])
    locToPlabel = (data[0]*Ny + data[1])*Nz +  data[2]
    pointLabels = argsort(locToPlabel)
    locToPlabel = sort(locToPlabel)
    segments = zeros(N)#, dtype='int16')
    NsegedStart = 0;
    isOpened = zeros(N)#, dtype='uint8')
    currentSegNumber = 1
    openPoints = [0]
    openPoints.pop()
    localizations = [(0, 0, 0, 0.)]
    localizations.pop()
    while (NsegedStart<N):
        ## get starting point to begin new segment
        while ((NsegedStart<N) and (segments[NsegedStart]>0)):
            NsegedStart += 1
            ## add first point to the queue
        if (NsegedStart<N):
            openPoints.append(NsegedStart)
        ## continue segmenting points at the front of the queue, while adding unopened nearest neighbor points to the end of the queue
        pointSet = [0]
        pointSet.pop()
        while (len(openPoints)>0):
            point = openPoints.pop()
            pointSet.append(point)
            segments[point] = currentSegNumber
            for n in arange(nn.shape[0]):
                neighbor = data[:3, point] + nn[n]
                NbrInd = (neighbor[0]*Ny + neighbor[1])*Nz + neighbor[2]#linInd(*neighbor)
                ind = searchsorted(locToPlabel, NbrInd)
                if (ind>=locToPlabel.size) or locToPlabel[ind] != NbrInd:
                    continue
                else:
                    nextPoint = pointLabels[ind]
                if ((nextPoint>-1) and (isOpened[nextPoint]==0)):
                    isOpened[nextPoint] = 1
                    openPoints.append(nextPoint)
        ## compute average position of the point
        psum = 0.
        xave, yave, zave = 0., 0., 0.
        pmax = 0.
        for point in pointSet:
            psum += data[3, point]
            xave += data[0, point]*data[3, point]
            yave += data[1, point]*data[3, point]
            zave += data[2, point]*data[3, point]
            if data[3, point] > pmax:
                pmax = data[3, point]
        if psum>0:
            localizations.append((xave/psum, yave/psum, zave/psum, pmax))
        ## once queue is empty, start a new segment
        currentSegNumber += 1
    return array(localizations)
def estimateRadii(args):
    locs, volume = args
    if locs.size==0:
        return array([])
    Nlocs, _ = locs.shape
    radii = zeros(Nlocs)
    rmax = 20.
    s = arange(-rmax, rmax+1)
    near = array([xn for xn in product(s, s) if norm(xn)<=rmax], dtype='int')
    N1, N2 = volume[..., 0].shape
    def estimate(xave, frame):
        xinds = around(xave).astype(int)
        patchInds = array([ij for ij in xinds + near
                           if (ij[1]>0) and (ij[1]<N1-1)
                           and (ij[0]>0) and (ij[0]<N2-1)])
        I = frame[patchInds[:, 1], patchInds[:, 0]]
        EI = mean(I)
        d = sum((xave - patchInds)**2, axis=1)
        def minFun(C):
            zeta = exp(-0.5*(d/C)**2)
            Ezeta = mean(zeta)
            B = (mean(I*zeta) - EI*Ezeta)/var(zeta)
            A = EI - B*Ezeta
            return mean((A + B*zeta - I)**2)
        res = minimize_scalar(minFun, bounds=(1, rmax), method='bounded', tol=1e-1)
        return res.x
    ####################
    for n in arange(Nlocs):
        xave = locs[n]
        zn = int(around(xave[2]))
        frame = volume[..., zn]
        radii[n] = estimate(xave[:2], frame)
    return radii

################################################################
################################################################
################################################################
################################################################
class TrackingData:
    def _LocateParticlesConnectedComponents(self, pixelProb, thresh, nprocs):
        """Given the localization probabilities, compute the set
           of most likely particle locations for each frame."""
        x, y, z, p, times = [], [], [], [], []
        ###########################
        gt = pixelProb.groupby('t')
        def getVolume(t):
            try:
                Pt = gt.get_group(t)
            except KeyError:
                return array([[0, 0, 0, 0]]).T
            return array(Pt[['x', 'y', 'z', 'p']]).T
        probs = imap(getVolume, arange(self.NF))
        shape = (self.Ny, self.Nx, self.Nz)
        iterable = izip(probs, repeat(shape))
        with closing(Pool(nprocs)) as pool:
            localizer = pool.imap(connectedComponents, iterable, chunksize=2)
            for t, sol in enumerate(localizer):
                Nnew, _ = sol.shape
                times.extend(t*ones(Nnew))
                x.extend(sol[:, 0])
                y.extend(sol[:, 1])
                z.extend(sol[:, 2])
                p.extend(sol[:, 3])
        # localizer = imap(connectedComponents, iterable)
        # for t, sol in enumerate(localizer):
        #     if sol.size==0:
        #         P[int(t)] = {'loc': array([[], [], []]), 'prob': array([])}
        #     else:
        #         # P[int(t)] = {'loc': sol[:, :3], 'prob': sol[:, 3]}
        #         Nnew, _ = sol.shape
        #         times.extend(t*ones(Nnew))
        #         x.extend(sol[:, 0])
        #         y.extend(sol[:, 1])
        #         z.extend(sol[:, 2])
        #         p.extend(sol[:, 3])
        print(len(p), 'particle localizations found')
        DF = pd.DataFrame(array([times, x, y, z, p, zeros_like(p)]).T, columns=['t', 'x', 'y', 'z', 'p', 'r'])
        return DF
    def _estimateAllRadii(self, vid, nprocs, ds):
        """Gaussian fit to local region, limit 15 pixel radius.
           Minimizes (A + B*exp(-0.5*(d_ij/r)**2) - I_ij)**2.
           Output is r.
        """
        if nprocs == 1:
            for t in arange(self.NF):
                volume = float32(vid.getVolume(t)[::ds, ::ds, :])
                locs = self.particleSetGrouped[t][:, :3]
                radii = estimateRadii((locs, volume))
                self.particleSetGrouped[int(t)][:, 4] = radii
        else:
            getVolume = lambda t: float32(vid.getVolume(t)[::ds, ::ds, :])
            getLocs = lambda t: self.particleSetGrouped[t][:, :3]
            args = izip(imap(getLocs, arange(self.NF)),
                        imap(getVolume, arange(self.NF)))
            with closing(Pool(nprocs)) as pool:
                radiusEst = pool.imap(estimateRadii, args, chunksize=5)
                for t, radii in enumerate(radiusEst):
                    self.particleSetGrouped[int(t)][:, 4] = radii
    def _linkParticles(self, D, nprocs):
        """find most likely links between pixels in adjacent frames.
           Uses linear assignment via the Hungarian algorithm.
        """
        LINKS = dict((int(t), {}) for t in arange(self.NF))
        def getLocs(t):
            return self.particleSetGrouped[t][:, :3]
        def getProbs(t):
            return self.particleSetGrouped[t][:, 3]
        pool = Pool(min(5, nprocs))
        locs = izip(imap(lambda t: getProbs(t), arange(1, self.NF)),
                    imap(lambda t: getProbs(t), arange(self.NF-1)),
                    imap(lambda t: getLocs(t), arange(1, self.NF)),
                    imap(lambda t: getLocs(t), arange(self.NF-1)))
        #linker = parallel(self.linkFn, locs, min(5, nprocs), args=(self.D, self.zscale), chunksize=1)
        linker = pool.imap(self.linkFn, zip(locs, repeat((D, self.zscale))), chunksize=1)
        for t, MLlinks in enumerate(linker):
            Nlinks, _ = MLlinks.shape if MLlinks.size>0 else (0, 0)
            LINKS[t+1]['Nlinks'] = Nlinks
            LINKS[t+1]['links'] = MLlinks
        skiplocs = izip(imap(lambda t: getProbs(t), arange(2, self.NF)),
                        imap(lambda t: getProbs(t), arange(self.NF-2)),
                        imap(lambda t: getLocs(t), arange(2, self.NF)),
                        imap(lambda t: getLocs(t), arange(self.NF-2)))
        skiplinker = pool.imap(self.linkFn, zip(skiplocs, repeat((D, self.zscale))), chunksize=1)
        for t, MLlinks in enumerate(skiplinker):
            Nlinks, _ = MLlinks.shape if MLlinks.size>0 else (0, 0)
            LINKS[t+2]['Nskiplinks'] = Nlinks
            LINKS[t+2]['skiplinks'] = MLlinks
        pool.close()
        pool.join()
        return LINKS
    def _collectTrajectories(self, P, skipLink):
        """Given links, collect all of the absolute positions for each particle.
           This step is just data extraction and formatting; no statistical computations happen here."""
        dataOut = []
        pn = 0
        #keep track of what particle positions we've already extracted
        collectedParticles = dict((k, []) for k in arange(self.NF))
        def pSet(t, n):
            return self.particleSetGrouped[t][n, :3]
        def getRad(t, n):
            return self.particleSetGrouped[t][n, 4]
        def getProb(t, n):
            return self.particleSetGrouped[t][n, 3]
        def loopFn(k, pn, b_in):
            def loopflag(k, b_in):
                if k<self.NF-1:
                    oneFrame = (P[k]['links'].size>0) and (b_in in P[k]['links'][:, 0])
                    if skipLink:
                        twoFrame = (P[k+1]['skiplinks'].size>0) and (b_in in P[k+1]['skiplinks'][:, 0])
                        return oneFrame or twoFrame
                    return oneFrame
                if k==self.NF-1:
                    return (P[k]['links'].size>0) and (b_in in P[k]['links'][:, 0])
                else:
                    return False
            while loopflag(k, b_in):
                if (P[k]['links'].size>0 and (b_in in P[k]['links'][:, 0])):
                    inds = P[k]['links'][:, 0] == b_in
                    ind = arange(P[k]['Nlinks'])[inds][0]
                    b, n = P[k]['links'][ind]
                    if n in collectedParticles[k]:
                        return
                    xnow, ynow, znow = pSet(k, n)
                    radnow = getRad(k, n)
                    pnow = getProb(k, n)
                    dataOut.append({'particle': pn, 'frame': k,
                                    'x': xnow, 'y': ynow, 'z': znow,
                                    'p': pnow, 'r': radnow})
                    collectedParticles[k].append(n)
                    k += 1
                    b_in = n
                elif skipLink:
                    inds = P[k+1]['skiplinks'][:, 0] == b_in
                    ind = arange(P[k+1]['Nskiplinks'])[inds][0]
                    b, n = P[k+1]['skiplinks'][ind]
                    if n in collectedParticles[k+1]:
                        return
                    xnow, ynow, znow = pSet(k+1, n)
                    radnow = getRad(k+1, n)
                    pnow = getProb(k+1, n)
                    dataOut.append({'particle': pn, 'frame': k+1,
                                    'x': xnow, 'y': ynow, 'z': znow,
                                    'p': pnow, 'r': radnow})
                    collectedParticles[k+1].append(n)
                    k += 2
                    b_in = n
        ###############################
        for k in arange(1, self.NF):
            for j in arange(P[k]['Nlinks']):
                b, n = P[k]['links'][j]
                if (b in collectedParticles[k-1]) or (n in collectedParticles[k]):
                    continue
                xback, yback, zback = pSet(k-1, b)
                xnow, ynow, znow = pSet(k, n)
                radback, radnow = getRad(k-1, b), getRad(k, n)
                pback, pnow = getProb(k-1, b), getProb(k, n)
                dataOut.append({'particle': pn,
                                'frame': k-1,
                                'x': xback,
                                'y': yback,
                                'z': zback,
                                'p': pback,
                                'r': radback})
                dataOut.append({'particle': pn,
                                'frame': k,
                                'x': xnow,
                                'y': ynow,
                                'z': znow,
                                'p': pnow,
                                'r': radnow})
                collectedParticles[k-1].append(b)
                collectedParticles[k].append(n)
                loopFn(k+1, pn, n)
                pn += 1
        names = ['particle', 'frame', 'x', 'y', 'z', 'p', 'r']
        dataOut = pd.DataFrame(dataOut, columns=names).sort_values(['particle', 'frame'])
        return dataOut
    def _LocateParticlesGreyMorph(self, I, particlesIn):
        R = 5.
        near = array([xi for xi in product(arange(-2*R, 2*R+1), arange(-2*R, 2*R+1)) if norm(xi)<=R])
        particlesOut = dict((int(t), {'loc': [], 'prob': []}) for t in arange(self.NF))
        for xc, yc, t, p in particlesIn:
            x, y = xc + near[..., 0], yc + near[..., 1]
            inds = (x>=0)*(x<self.Nx)*(y>=0)*(y<self.Ny)
            rset = zip(x[inds], y[inds])
            u = sum([I[t, yn, xn] for xn, yn in rset])
            assert(u>0.)
            xnew = sum([I[int(t), yn, xn]*xn for xn, yn in rset])/u
            ynew = sum([I[int(t), yn, xn]*yn for xn, yn in rset])/u
            particlesOut[int(t)]['loc'].append([xnew, ynew])
            particlesOut[int(t)]['prob'].append(p)
        for t in arange(self.NF):
            particlesOut[int(t)]['loc'] = array(particlesOut[int(t)]['loc'])
            particlesOut[int(t)]['prob'] = array(particlesOut[int(t)]['prob'])
        return particlesOut
    def _trajectoryStats(self, output=True):
        td = self.Data.groupby('particle')
        self.Nparticles = td.particle.ngroups
        self.lengths = array(td.size())
        if output:
            print(self.Nparticles, 'paths')
            print('mean path length:', mean(self.lengths),
                  ', std:', std(self.lengths))
    def save(self, filename, withParticleSet=True):
        # self.Data.to_excel(filename+'.xlsx')
        try:
            self.Data.to_csv(filename+' (tracks).csv')
        except:
            pass
        if withParticleSet:
            # np.save(filename, self.particleSet)
            self.particleSet.to_csv(filename+' (localizations).csv')
        return self
    def tracksToDict(self):
        dictData = {}
        for p, g in self.Data.groupby('particle'):
            dx, dy, dz = array(diff(g.x)), array(diff(g.y)), array(diff(g.z))
            dt = array(diff(g.frame))
            N = g.x.size
            D2 = sum((dx**2 + dy**2)/(4.*dt*N))
            D3 = sum((dx**2 + dy**2 + self.zscale*dz**2)/(6.*dt*N))
            dictData[p] = {'x': array(g.x),
                           'y': array(g.y),
                           'z': array(g.z),
                           't': array(g.frame),
                           'dx': dx, 'dy': dy, 'dz': dz, 'dt': dt,
                           'p': array(g.p),
                           'r': array(g.r),
                           'N': N, 'Deff (xy)': D2, 'Deff (xyz)': D3}
        return dictData
    def detectionsToDict(self):
        dictData = {'x': array(self.particleSet.x),
                    'y': array(self.particleSet.y),
                    'z': array(self.particleSet.z),
                    't': array(self.particleSet.t),
                    'p': array(self.particleSet.p),
                    'r': array(self.particleSet.r),
                    'N': self.particleSet.x.size}
        return dictData
    def savePathsJSON(self, filename=''):
        pd.Series(self.tracksToDict()).to_json(fn+'-gemsTracks.json')
        return self
    def DTsave(self, filename):
        """save data for easy DataTank consumption"""
        NF, Ny, Nx, Nz = self.shape
        N = self.Data.frame.size
        M = self.Data.particle.max()
        paths = zeros((5, N + M))*nan
        increments = zeros((4, N + M))*nan
        Na = 0
        for p, g in self.Data.groupby('particle'):
            Np = g.frame.size
            path = array((g[['x', 'y', 'z', 'frame', 'r']]))
            #if g['r'].mean() < 12:
            paths[:, Na:Na+Np] = path.T
            dx, dy, dz = diff(g.x), diff(g.y), diff(g.z)
            incs = array([dx, dy, dz, array(g['frame'])[1:]])
            increments[:, Na:Na+Np-1] = incs
            Na += Np + 1
        incrementStats = zeros((5, NF))
        for t in arange(1, NF):
            inds = increments[3, :] == t
            dx, dy, dz = increments[0, inds], increments[1, inds], increments[2, inds]
            D = mean((dx - mean(dx))**2 + (dy - mean(dy))**2 + self.zscale**2*(dz - mean(dz))**2)/6.
            incrementStats[:, t] = array([mean(dx), mean(dy), mean(dz), D, t]).T
        outdict = {'Seq_paths': array([u'Array'], dtype='<U5'),
                   'paths': paths,
                   'Seq_lengths': array([u'Array'], dtype='<U5'),
                   'lengths': array([self.lengths]),
                   'Seq_incStats': array([u'Array'], dtype='<U5'),
                   'incStats': incrementStats}
        savemat(filename + '.mat', outdict, format='4')
        return self
    def particleColor(self, p, cmap='jet'):
        c = mod(p, 256)
        colorMap = matplotlib.pylab.get_cmap(cmap)
        try:
            return colorMap(self._shuffled[c])
        except:
            self._shuffled = np.random.permutation(arange(256))
            return colorMap(self._shuffled[c])
    def filterPathsByLength(self, n):
        "Filter all paths that have less than n increments"
        g = self.Data.groupby('particle')
        NparticlesOld = self.Nparticles
        Data = g.filter(lambda x: x.frame.max()-x.frame.min() >= n-1)
        ## renumber index
        # d = Data.pivot('particle', 'frame').stack()
        # pn = array(d.axes[0].levels[0])
        nl = {}
        n = 0
        for p, g in Data.groupby('particle'):
            nl[p] = n
            n += 1
        # nl = dict((n, m) for n, m in zip(pn, count(0)))
        Data.particle = Data.particle.apply(lambda x: nl[x])
        ## update data
        Nparticles = Data.particle.max()
        print(NparticlesOld - Nparticles-1, 'particle paths filtered')
        return self.setData(Data)
    def filterParticlesByRadius(self, rCutOff, keep='<'):
        print('filtering particles by radius')
        r = self.particleSet.r
        inds = r<rCutOff if keep=='<' else r>rCutOff
        Nfiltered = sum(inds==False)
        PSnew = array(self.particleSet[inds][['t', 'x', 'y', 'z', 'p', 'r']])
        self.particleSet = pd.DataFrame(PSnew, columns=['t', 'x', 'y', 'z', 'p', 'r'])
        self.particleSetGrouped = self._makePSgrouped()
        print(Nfiltered, 'particle localizations filtered')
        return self
    def _makePSgrouped(self):
        PSG = {}
        groups = self.particleSet.groupby('t')
        for t in arange(self.NF):
            try:
                g = groups.get_group(t)
                PSG[t] = array(g[['x', 'y', 'z', 'p', 'r']])
            except KeyError:
                PSG[t] = array([[], [], [], [], []]).T
        return PSG
    def segmentParticles(self, pixelProb, vidFile=None,
                         thresh=0.5, nprocs=2, ds=1):
        print('identifying probable particles')
        self.particleSet = self._localizer(pixelProb, thresh, nprocs)
        if ds>1:
            self.particleSet.x *= ds
            self.particleSet.y *= ds
        self.particleSetGrouped = self._makePSgrouped()
        return self
    def estimateRadii(self, vidFile, ds=1, nprocs=2):
        print('estimating PSF radii')
        self._estimateAllRadii(vidFile, nprocs, ds)
        r = []
        for k, v in self.particleSetGrouped.iteritems():
            r.extend(v[:, 4])
        self.particleSet = self.particleSet.assign(r=array(r))
        self.particleSetGrouped = self._makePSgrouped()
        return self
    def linkParticles(self, D=10., nprocs=2, skipLink=True):
        print('linking particles')
        P = self._linkParticles(D, nprocs)
        print('collecting paths')
        self.Data = self._collectTrajectories(P, skipLink)
        self._trajectoryStats()
        return self
    def addDetections(self, filename):
        self.particleSet = pd.read_csv(filename, index_col=0)
        self.particleSetGrouped = self._makePSgrouped()
    def setData(self, data):
        self.Data = data.sort_values(['particle', 'frame'], axis=0)
        self._trajectoryStats(output=False)
        return self
    def __init__(self, filename='', metafile='', shape=(), zscale=1, linking=True):
        self.linkFn = linkFrame
        self._localizer = self._LocateParticlesConnectedComponents
        self.shape = shape
        self.zscale = zscale
        self.Data = pd.DataFrame([], columns=['particle', 'frame', 'x', 'y', 'z', 'p', 'r'])
        if len(metafile)>0:
            try:
                self.metadata = loadmat(metafile+'.mat')
                self.NF = self.metadata['NF'].ravel()[0]
                #self.Ny = self.metadata['Ny'].ravel()[0]
                #self.Nx = self.metadata['Nx'].ravel()[0]
                self.Nz = self.metadata['Nz'].ravel()[0]
                #self.shape = (self.NF, self.Ny, self.Nx, self.Nz)
                self.zscale = self.metadata['dz'].ravel()[0]/self.metadata['dxy'].ravel()[0]
            except:
                print('Could not load video metadata')
        elif len(shape)>0: ## shape overrides metadata
            self.NF, self.Ny, self.Nx, self.Nz = shape
            self.shape = shape
        if len(filename)>0:
            try:
                self.Data = pd.read_csv(filename+' (tracks).csv', index_col=0)
                self.NF = self.Data.frame.max()
                self._trajectoryStats(output=True)
            except:
                print('particle paths not found')
                if linking:
                    raise
            try:
                # self.particleSet = np.load(filename+'.npy').flatten()[0]
                self.particleSet = pd.read_csv(filename+' (localizations).csv', index_col=0)
                self.particleSetGrouped = self._makePSgrouped()
            except:
                print('particle set not found')
                raise
            return None
        else:
            return None
        ###############
