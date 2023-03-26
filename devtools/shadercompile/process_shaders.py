#!/usr/bin/env python3 

import subprocess 
import os
import argparse
import sys

# Pretty much only here for verification of parameters. The powershell script is a bit nicer
parser = argparse.ArgumentParser(description='Processes shaders')
parser.add_argument('-Threads', type=int, default=1, help='Number of threads to run with')
parser.add_argument('-Version', type=str, choices=['20b', '30', '40', '41', '50', '51'], help='shader model to build with')
parser.add_argument('-Dynamic', action='store_true', dest='DYNAMIC', help='Dynamic combos')
parser.add_argument('-List', action='store_true', dest='LIST', help='Use a list of shaders passed to the command line instead of a shader list file')
parser.add_argument('file', metavar='F', type=str, nargs='+', help='Shader list file or list of shaders to build')

args = parser.parse_args()

scriptdir = os.path.dirname(os.path.realpath(__file__))

cmdline = []
# For windows use the exe directly, otherwise use wine
if sys.platform == 'win32' or sys.platform == 'cygwin':
    cmdline += [f'{scriptdir}/ShaderCompile.exe']
else:
    cmdline += ['wine', f'{scriptdir}/ShaderCompile.exe']

cmdline += [
    '-threads', f'{args.Threads}',
    '-ver', f'{args.Version}'
]

if args.DYNAMIC:
    cmdline += ['-dynamic']
    
dirname = os.path.dirname(os.path.realpath(args.file[0]))
cmdline += ['-shaderpath', dirname, '-verbose_preprocessor']

# For each line in the file, compile that specific shader 
if args.LIST:
    for s in args.file:
        actualcmd = cmdline.copy()
        actualcmd += [s]
        subprocess.run(actualcmd, env={'WINEPREFIX': os.environ['WINEPREFIX'], 'WINEDEBUG': '-all'})
else:
    with open(args.file[0]) as fp:
        lines = fp.readlines()
        for l in lines:
            # Nuke comments
            line = l.split('//', 1)[0].rstrip()
            if len(line) <= 0: 
                continue
            actualcmd = cmdline.copy()
            actualcmd += [line]
            subprocess.run(actualcmd, env={'WINEPREFIX': os.environ['WINEPREFIX'], 'WINEDEBUG': '-all'})
