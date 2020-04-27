#!/anaconda3/bin/python
import os
from numpy import array
from scipy.io import savemat, loadmat


var = loadmat('input.mat')
filename = var['filename'].flatten()[0]

# filename = ' '.join(sys.argv[1:])
path, name = os.path.split(filename)


outdict = {'Seq_Var': array([u'String'], dtype='<U6'),
            'Var': array([path])}
savemat('output.mat', outdict, format='4')
