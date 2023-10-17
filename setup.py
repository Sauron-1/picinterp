import os
import re
import sys
import sysconfig
import platform
import subprocess
from pathlib import Path

from distutils.version import LooseVersion
from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from setuptools.command.test import test as TestCommand


def print_sep(msg, width=60):
    num_dash = width - len(msg) - 2
    num_left = num_dash // 2
    num_right = num_dash - num_left
    print('-'*num_left, msg, '-'*num_right)


class CMakeExtension(Extension):
    def __init__(self, name):
        Extension.__init__(self, name, sources=[])


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError(
                "CMake must be installed to build the following extensions: " +
                ", ".join(e.name for e in self.extensions))

        build_directory = os.path.abspath(self.build_temp)

        cmake_args = [
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + build_directory,
            '-DPYTHON_EXECUTABLE=' + sys.executable,
            '-DPYTHON_INCLUDE_DIR=' + sysconfig.get_path('include'),
            '-DPYTHON_LIBRARY=' + sysconfig.get_config_var('LIBDIR')
        ]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        # Assuming Makefiles
        num_job = max(os.cpu_count()-1, 1)
        build_args += ['--', f'-j{num_job}']

        self.build_args = build_args

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(
            env.get('CXXFLAGS', ''),
            self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)

        cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
        march = env.pop('MARCH', 'native')
        cmake_args += [f'-DMARCH={march}', '-DBUILD_PYTHON=ON', '-DINSTALL_CPP=OFF']

        # CMakeLists.txt is in the same directory as this setup.py file
        cmake_list_dir = os.path.abspath(os.path.dirname(__file__))
        print_sep('Running CMake prepare')
        subprocess.check_call(['cmake', cmake_list_dir] + cmake_args,
                              cwd=self.build_temp, env=env)

        print_sep('Building extensions')
        cmake_cmd = ['cmake', '--build', '.'] + self.build_args
        subprocess.check_call(cmake_cmd,
                              cwd=self.build_temp)

        # Move from build temp to final position
        for ext in self.extensions:
            self.move_output(ext)

    def move_output(self, ext):
        build_temp = Path(self.build_temp).resolve()
        dest_path = Path(self.get_ext_fullpath(ext.name)).resolve()
        source_path = build_temp / self.get_ext_filename(ext.name)
        dest_directory = dest_path.parents[0]
        dest_directory.mkdir(parents=True, exist_ok=True)
        self.copy_file(source_path, dest_path)
        
        
ext_modules = [
    CMakeExtension('picinterp'),
]

setup(
    name = 'picinterp',
    version = '0.1',
    description = 'PIC shape function interpolate',
    author = 'Junyi Ren',
    packages = find_packages(),
    ext_modules = ext_modules,
    cmdclass = dict(build_ext=CMakeBuild),
    zip_safe = False,
)
