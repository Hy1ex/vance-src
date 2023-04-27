#!/usr/bin/env python3
import sys
import os
import shutil
import multiprocessing
import subprocess
from pathlib import Path

MOD='vance'

def process_shaders(input, threads=1, version="30", list=False, dynamic=False):
	SHADERCOMPILE_PATH = str(Path(__file__).parent.absolute() / 'ShaderCompile.exe')
	dirname = str(Path(input[0]).parent)

	args = []
	env = {}

	# For windows use the exe directly, otherwise use wine
	if sys.platform == 'win32' or sys.platform == 'cygwin':
		args += [SHADERCOMPILE_PATH]
	else:
		args += ['/usr/bin/wine', SHADERCOMPILE_PATH]
		env['WINEPREFIX'] = os.environ['WINEPREFIX']
		env['WINEDEBUG'] = '-all' 

	args += [
		'-threads', f'{threads}',
		'-ver', f'{version}',
		'-verbose_processor',
		'-shaderpath', str(dirname)
	]

	if dynamic:
		args += '-dynamic'
		
	# For each line in the file, compile that specific shader 
	if list:
		for shader in input:
			shader_args = args.copy()
			shader_args += [shader]

			subprocess.run(args=shader_args, cwd=dirname, env=env)
	else:
		with open(input[0]) as fp:
			lines = fp.readlines()
			for l in lines:
				# Nuke comments
				shader = l.split('//', 1)[0].rstrip()
				if len(shader) <= 0: 
					continue

				shader_args = args.copy()
				shader_args += [shader]

				subprocess.run(args=shader_args, cwd=dirname, env=env)


def main():
	version = '20b'
	use_list = False
	jobs = multiprocessing.cpu_count()
	dynamic = False

	args = sys.argv[1:]

	for arg in args:
		match arg:
			case '-20b':
				args.remove(arg)
				version = '20b'
			case '-30':
				args.remove(arg)
				version = '30'
			case '-l':
				args.remove(arg)
				use_list = True
			case '-dynamic':
				args.remove(arg)
				dynamic = True
			case _:
				if arg.startswith("-j"):
					args.remove(arg)
					jobs = int(arg.removeprefix("-j"))

	input = args
	if use_list and len(args) <= 1:
		raise "No shaders to compile!"		
		
	include_dir = Path('include')
	shaders_dir = Path('shaders')

	if not include_dir.exists():
		os.mkdir(include_dir)

	if not shaders_dir.exists():
		os.mkdir(shaders_dir)
	
	process_shaders(input, jobs, version, use_list, dynamic)
	
	for file in shaders_dir.glob('fxc/*.vcs'):
		dst = Path.cwd() / '../../../game' / MOD / 'shaders/fxc' / file.name
		dst = dst.resolve()
		shutil.copyfile(file, dst)

if __name__ == '__main__':
	main()
