from setuptools import setup, Extension
import os
import posixpath
import sys

THIS_DIR = os.path.dirname(__file__)
# avoid loading the package before requirements are installed:

package = posixpath.join(THIS_DIR, 'xdelta3')
package_data = ['_xdelta3.c']

if os.path.exists(posixpath.join(package, 'lib')):
	package_data += ['lib/' + f for f in os.listdir(posixpath.join(package, 'lib')) if 'main' not in f]

print package_data

setup(
	name='xdelta3',
	version='0.0.5',
	description='Fast delta encoding using xdelta3',
	long_description="long_description",
	author='Samuel Colvin',
	author_email='s@muelcolvin.com',
	url='https://github.com/samuelcolvin/xdelta3-python',
	license='Apache License, Version 2.0',
	packages=['xdelta3'],
	package_data={
		'xdelta3': package_data
	},
	zip_safe=True,
	ext_modules=[
		Extension(
			'_xdelta3',
			sources=['xdelta3/_xdelta3.c'],
			library_dirs=['C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\lib', 'C:\Python27\include'],
			include_dirs=['./xdelta3/lib', 'C:\Python27\include'],
			define_macros=[
				('SIZEOF_SIZE_T', '8'),
				('SIZEOF_UNSIGNED_LONG_LONG', '8'),
				('XD3_USE_LARGEFILE64', '1'),
				('EXTERNAL_COMPRESSION', '0'),
				('_WIN32', 1),
			],
			extra_compile_args=['/MACHINE:x64'],
			extra_link_args=['/MACHINE:x64']
		)
	],
	package_dir= {'': '.'},
	classifiers=[
		'Development Status :: 4 - Beta',
		'Programming Language :: Python',
		'Programming Language :: Python :: 3',
		'Programming Language :: Python :: 3.5',
		'Programming Language :: Python :: 3.6',
		'License :: OSI Approved :: Apache Software License',
		'Operating System :: Unix',
		'Operating System :: POSIX :: Linux',
		'Topic :: System :: Archiving :: Compression',
	],
)
