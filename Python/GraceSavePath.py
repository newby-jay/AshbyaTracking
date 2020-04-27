#!/anaconda3/bin/python
import os
from numpy import array
from scipy.io import savemat, loadmat


var = loadmat('input.mat')
path = var['path'].flatten()[0]
apath = os.path.expanduser('~/Dropbox/Gladfelter_Gems_project/')
spath1, vidName = os.path.split(path)
relPath0 = os.path.relpath(spath1, apath) # path with set folder at root
setFolder = relPath0.split('/')[0]
# _, name = os.path.split(spath1)
axialPath = os.path.join(apath, 'Axial_Projections/')
axialPath = os.path.join(axialPath, setFolder + ' (axialDensityData)/')
relPath1 = os.path.relpath(relPath0, setFolder)
axialPath = os.path.join(axialPath, relPath1)
# axialPath = os.path.join(axialPath, 'x10t10/')
if not os.path.isdir(axialPath):
    os.makedirs(axialPath)

savePath = os.path.join(axialPath, vidName)

outdict = {'Seq_Var': array([u'String'], dtype='<U6'),
            'Var': array([savePath])}
savemat('output.mat', outdict, format='4')
