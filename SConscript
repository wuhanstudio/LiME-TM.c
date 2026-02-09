from building import *
import rtconfig

# get current directory
cwd     = GetCurrentDir()

# The set of source files associated with this SConscript file.
src     = Glob('tsetlin/*.c')
src    += Glob('random/*.c')

path    = [cwd]
path   += [cwd + '/tsetlin']
path   += [cwd + '/random']
path   += [cwd + '/utils']

LOCAL_CCFLAGS = ''

# MNIST Examples (file system)
if GetDepend('LIME_TM_USING_MNIST_FS_EXAMPLE'):
    path   += [cwd + '/mnist']
    src    += Glob('mnist/*.c')

    path   += [cwd + '/protobuf']
    src    += Glob('protobuf/*.c')
    src    += Glob('protobuf-c/*.c')

    src    += Glob('platforms/rt-thread/mnist_fs.c')

    # LOCAL_CCFLAGS = '-D TSETLIN_USING_PROTOBUF'

# MNIST Examples (static header)
if GetDepend('LIME_TM_USING_MNIST_STATIC_EXAMPLE'):
    path   += [cwd + '/mnist']
    src    += Glob('mnist/*.c')

    path   += [cwd + '/model']
    src    += Glob('platforms/rt-thread/mnist_static.c')

    # LOCAL_CCFLAGS = '-D TSETLIN_USING_STATIC_MODEL'

group = DefineGroup('lime-tm', src, depend = ['PKG_USING_LIME_TM'], CPPPATH = path, LOCAL_CCFLAGS = LOCAL_CCFLAGS)

Return('group')
