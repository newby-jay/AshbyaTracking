from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from numpy import *
from numpy.random import rand, normal
import scipy
from scipy.linalg import norm
import os
import subprocess
import tensorflow as tf
from itertools import cycle
from itertools import product, cycle, count, permutations, ifilter, repeat
from itertools import izip, imap
import tifffile as tif
import uuid
import pickle
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('set', type=str, help='what set')


dt2 = .03
imageSize = 128 # number of x,y image pixels
Nz = 17 # number of z-slices
zs = 1.8 # the z-scale factor, zs = dz/dxy
rmax = Nz*zs/2. # radius of the cylinder representing the hypha
rmax_inner = rmax/2.
yc = imageSize/2. # center y coordinate of hypha
zc = (Nz - 1.)*zs/2. # center z coordinate of hypha
Vtot = 2.*pi*rmax**2*imageSize # total volume of hypha
# image pixel coordinates
ximg0 = array(
    [[[i, j]
        for i in arange(imageSize)]
            for j in arange(imageSize)])
zimg0 = zs*arange(Nz)


def tfrand(*args):
    """Tensoflow uniform random number"""
    #shape = tf.to_int32(tf.squeeze(args))
    return tf.random_uniform(args)
def getReflectPoint(p1, p2, R):
    x1, y1 = p1
    x2, y2 = p2
    r1 = sqrt(x1**2 + y1**2)
    r2 = sqrt(x2**2 + y2**2)
    assert r1 <= R
    assert r2 > R
    assert r1 < r2
    # if absolute(r1-R) < 1e-14:
    #    xb, yb = R/r1*p1
    #else:
    a = (x2 - x1)**2 + (y2 - y1)**2
    b = 2*(x1*(x2 - x1) + y1*(y2 - y1))
    c = x1**2 + y1**2 - R**2
    tb = (-b + sqrt(b**2 - 4*a*c))/(2.*a)
    xb = x1 + tb*(x2 - x1)
    yb = y1 + tb*(y2 - y1)
    m1 = (-xb*(x2 - xb) - yb*(y2 - yb))/R
    m2 = (yb*(x2 - xb) - xb*(y2 - yb))/R
    dx_refl = m1*xb + m2*yb
    dy_refl = m1*yb - m2*xb
    xr = xb + dx_refl/R
    yr = yb + dy_refl/R
    return array([xr, yr]), array([xb, yb])
def makeTracks(Nparticles, Nframes, D0, Nsamp=16):
    """Generates Brownian motion tracks restricted to a cylindrical region.
    Nparticles: the total number of GEMS tracks
    Nframes: the number of time values for each tracks
    D0: a tuple containing the left half and right half diffusivity
    Returns:
    xi: (Nt, Nparticles, 3) array
    """
    def cylindrical_coords_r(x):
        y, z = x[..., 1], x[..., 2]
        r = sqrt((y - yc)**2 + (z - zc)**2)
        return r
    def D1(x):
        """left half low and right half high"""
        out = D0[1]*ones(Nparticles)
        inds = x[..., 0] < yc
        out[inds] = D0[0]
        return out
    def D2(x):
        """Small low zone in the center"""
        out = D0[1]*ones(Nparticles)
        inds = (
            (x[..., 0] > yc - 0.1*imageSize)
            &(x[..., 0] < yc + 0.1*imageSize)
            &(x[..., 1] > yc - rmax_inner)
            &(x[..., 1] < yc + rmax_inner)
            &(x[..., 2] > zc - rmax_inner)
            &(x[..., 2] < zc + rmax_inner))
        out[inds] = D0[0]
        return out
    if testType == 'testA':
        Dfun = D1
    elif testType == 'testB':
        Dfun = D2
    else:
        assert testType == 'testA' or testType == 'testB'
    DX = normal(0, sqrt(2.), (Nsamp*Nframes, Nparticles, 3))
    yzc = array([yc, zc])
    xmarg = 0.
    x0 = - xmarg + (imageSize + 2*xmarg)*rand(Nparticles, 1)
    r0 = rmax*sqrt(rand(Nparticles, 1))
    th0 = -pi + 2*pi*rand(Nparticles, 1)
    y0 = yc + r0*cos(th0)
    z0 = zc + r0*sin(th0)
    x0 = concatenate([x0, y0, z0], 1)
    out = zeros((Nsamp*Nframes, Nparticles, 3))
    out[0] = x0
    for t in arange(1, Nsamp*Nframes):
        Di = Dfun(out[t-1])
        dxi = (sqrt(Di/Nsamp)*DX[t].T).T
        xnew = out[t-1] + dxi
        out[t] = out[t-1] + (sqrt(Dfun(xnew)/Nsamp)*DX[t].T).T
        r = cylindrical_coords_r(out[t])
        inds = r > rmax
        for ind in arange(Nparticles)[inds]:
            p1 = out[t-1, ind, 1:] - yzc
            assert norm(p1) <= rmax
            p2 = out[t, ind, 1:] - yzc
            assert norm(p2) > rmax
            assert norm(p1) < norm(p2)
            pnew, pb = getReflectPoint(p1, p2, rmax)
            n_iter = 0
            while norm(pnew) >= rmax  and n_iter < 100:
                pold = pnew
                pnew, pb = getReflectPoint((rmax/norm(pb)-1e-14)*pb, pnew, rmax)
                n_iter += 1
            assert(norm(pnew) < rmax)
            out[t, ind, 1:] = yzc + pnew
        x = out[t, ..., 0]
        inds = x < 0. - xmarg
        out[t, inds, 0] = (2.*(0. - xmarg) - out[t, inds, 0])
        inds = x > imageSize + xmarg
        out[t, inds, 0] = (2.*(imageSize + xmarg) - out[t, inds, 0])
    return float32(out[::Nsamp]) # Nt, Nparticles, 3

def makeVid(xi, uamp, Nframes, Nparticles, kappa=5., a=0.6, IbackLevel=100):
    """Generates a testing video.
    xi: tracks
    uamp: max intensity of the GEMS PSF
    Nframes: number of video frames (volumes)
    kappa: the standard deviation of the white noise added to the video
    a: the spatial scale factor for the GEMS PSFs
    IbackLevel: background intensity
    """
    def makeBG0():
        """Make constant video background."""
        Lbg = 80.
        X0, Y0, Z0 = meshgrid(
            arange(imageSize),
            arange(imageSize),
            arange(Nz)*zs)
        R0 = sqrt((Y0 - yc)**2 + (Z0 - zc)**2)
        V = 0.25*cos(2*pi/30.*(Y0-yc+rmax))
        # V *= sin(2*pi/30.*(Z0-zc+rmax))
        V *= sin(2*pi/Lbg*X0)
        V += 1.1
        out = 5.*V*(R0 <= rmax*1.1)
        return out
    def imgCoords(xi):
        """Generates two arrays used to create the PSFs for particles located at
         positions contained in the input array, xi."""
        XYofZ = tf.tile(
            xi[..., :2].reshape(1, Nframes, 5, 2),
            [Nz, 1, 1, 1]) # Nz, Nt, Np, 2
        Z = xi[..., 2]
        ### randomly shift PSFs in each slice (skip if 2D)
        if arg != '2D':
            sliceMove = tf.random_normal(
                (Nz, Nframes, 5, 2),
                mean=0,
                stddev=1.) # 0.75
            XYofZ += sqrt(0.75)*sliceMove
        ZALL = tf.transpose(
            tf.tile(zimg, [Nframes, 5, 1]),
            # Nz, Nt, Np
            [2, 0, 1]) \
            - Z
        XALL = tf.transpose(
            tf.tile(ximg, [Nframes, 5, Nz, 1, 1, 1]),
            # Nx, Ny, Nz, Nt, Np, 2
            [3, 4, 2, 0, 1, 5]) \
            - XYofZ
            #- xi[..., :2]
        r = tf.sqrt(XALL[..., 0]**2 + XALL[..., 1]**2)
        return ZALL, r
    def genVideo(xi, kappa, a, uamp, IbackLevel):
        """Generates a video array for a given set of tracks contained in the
        input array, xi"""
        z, r = imgCoords(xi)
        ufade = 1.5*zs # psf will fade out after 1.5*zscale distance (in z)
        fade = uamp*(1. - tf.abs(tf.tanh(z/ufade)))
        PSF = tf.exp(-(r**(3./2.)/(8.*a**2))**2)
        I = tf.transpose(
            tf.reduce_sum(
                fade*PSF,
                axis=4), # sum of Nparticles
            [3, 0, 1, 2]) # Nt, Ny, Nx, Nz
        return I
    ximg = tf.reshape(
        tf.constant(float32(ximg0)),
        [1, 1, 1, imageSize, imageSize, 2]
        )
    zimg = tf.reshape(
        tf.constant(float32(zimg0)),
        [1, 1, Nz]
        )
    BG0 = tf.constant(float32(makeBG0()))
    # Ncut = tf.to_int32(tfrand(1)[0]*tf.to_float(Np/4))
    # uamp = tf.concat([10*tfrand(1)*uamp[:Ncut], uamp[Ncut:]], 0)
    I = tf.zeros((Nframes, imageSize, imageSize, Nz))
    print('Nparticles:', Nparticles)
    Np = 0
    while Np < Nparticles: # generate video in batches of 5 frames (for memory)
        I += genVideo(xi[:, Np:Np+5], kappa, a, uamp, IbackLevel)
        Np += 5
    I += BG0
    I += tf.random_normal(
        (Nframes, imageSize, imageSize, Nz),
        mean=IbackLevel,
        stddev=kappa) # adds noise to the video
    return I

if __name__ == '__main__':
    # Script is contained below
    Nframes = 50 # video length
    kappa = 5. # noise level (STDev)
    args = parser.parse_args()
    arg = args.set
    ### synthTest1
    if arg == '1':
        Nframes = 100 # video length
        testType = 'testA'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2)
        # PAMP, NP = meshgrid(
        #     linspace(2, 100, 20),
        #     arange(5, 70, 10))
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test1/'

    ### synthTest2
    if arg == '2':
        testType = 'testA'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.1*dt/dxy**2)
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test2/'

    ### synthTest3
    if arg == '3':
        testType = 'testA'
        dt = 2.
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test3/'

    ### synthTest4
    if arg == '4':
        testType = 'testA'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test4/'

    ### synthTest5
    if arg == '5':
        testType = 'testA'
        dt = 0.3
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test5/'

    ### synthTest6
    if arg == '6':
        testType = 'testA'
        dt = 0.85
        dxy = 0.11
        D0 = (0.001*dt/dxy**2, 0.02*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test6/'

    ### synthTest1b
    if arg == '1b':
        Nframes = 100 # video length
        testType = 'testB'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2)
        # PAMP, NP = meshgrid(
        #     linspace(2, 100, 20),
        #     arange(5, 70, 10))
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test1b/'

    ### synthTest2b
    if arg == '2b':
        testType = 'testB'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.1*dt/dxy**2)
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test2b/'

    ### synthTest3b
    if arg == '3b':
        testType = 'testB'
        dt = 2.
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test3b/'

    ### synthTest4b
    if arg == '4b':
        testType = 'testB'
        dt = 0.85
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test4b/'

    ### synthTest5b
    if arg == '5b':
        testType = 'testB'
        dt = 0.3
        dxy = 0.11
        D0 = (0.01*dt/dxy**2, 0.2*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test5b/'

    ### synthTest6b
    if arg == '6b':
        testType = 'testB'
        dt = 0.85
        dxy = 0.11
        D0 = (0.001*dt/dxy**2, 0.02*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [5, 10, 15, 20, 25, 45, 65, 85, 105, 125, 145, 165, 185, 205])
        basePath = 'gs://gems-drive/nt-test/test6b/'

    ### synthTest6b
    if arg == '2D':
        testType = 'testA'
        Nframes = 100
        dt = 0.03
        dxy = 0.11
        D0 = (0.001*dt/dxy**2, 0.1*dt/dxy**2) # left and right half diffusivities
        PAMP, NP = meshgrid(
            [25., 33.75],
            [45, 65, 85, 105, 125])
        basePath = 'gs://gems-drive/nt-test/test2D_2/'

    PAMP = PAMP.flatten()
    NP = int64(NP.flatten())

    allParams = {}
    for n in arange(0, PAMP.size):
        print('-------')
        print('video:', n)
        print('-------')
        pamp = PAMP[n]
        Nparticles = NP[n]
        assert Nparticles % 5 == 0
        fn = basePath + '{0}'.format(n)
        xi = makeTracks(Nparticles, Nframes + 100, D0)[100:]
        with tf.Graph().as_default():
            V = makeVid(xi, pamp, Nframes, Nparticles, kappa)
            sess = tf.InteractiveSession()
            init = tf.global_variables_initializer()
            vid = uint16(sess.run(V))
            sess.close()
            out = {
                'vid': vid,
                'tracks': xi,
                'Nparticles': Nparticles,
                'pamp': pamp,
                'D': D0,
                'kappa': kappa,
                'Nz': Nz,
                'Nt': Nframes,
                'Ny': imageSize,
                'Nx': imageSize,
                'zscale': zs
            }
            if arg == '2D':
                out['Nz'] = 1
                out['vid'] = vid[..., 8].reshape(Nframes, imageSize, imageSize, 1)
                out['zscale'] = 1.
        ### save video data and tracks
        # with tf.gfile.Open(fn+'.dat', mode='w+b') as sfile:
        #     pickle.dump(out, sfile)
        with tf.io.gfile.GFile(fn+'.dat', mode='w+b') as sfile:
            pickle.dump(out, sfile)
        ### save a max projection video as a tif file
        # with tf.gfile.Open(fn+'.nd2.tif', mode='w+b') as sfile:
        #     tif.imsave(
        #         sfile,
        #         out['vid'].max(axis=3))
        ### save a copy of the 3D video as a tif file (testing only)
        # tif.imsave(
        #     basePath + '{0}-full.tif'.format(n),
        #     out['vid'].transpose(0, 3, 1, 2))
    ## save params in a dictionary
    # r = out['Nz']*1.8/2.
    # vol = 2.*pi*r**2*out['Nx']
    # toSave = dict(
    #     (k, out[k]) for k in out if not k in ['vid'])
    # toSave['SNR'] = out['pamp']/out['kappa']
    # toSave['ParticleDensity'] = out['Nparticles']/vol
    # toSave['dxy'] = 0.11
    # toSave['dt'] = 0.65
    # toSave['dz'] = 0.2
    # toSave['D'] = (0.4, 6.)
    # allParams[n] = toSave
# paramsFileName = os.path.join(basePath, 'truth_Data.pickle')
# with tf.gfile.Open(paramsFileName, 'w+b') as sfile:
#     pickle.dump(allParams, sfile)
