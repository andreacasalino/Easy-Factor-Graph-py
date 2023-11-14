import argparse, os

class Paths:
    SOURCES = os.path.dirname(__file__)
    DEST = os.path.join(SOURCES, 'cases')

def gatherSources():
    res = []
    for name in os.listdir(Paths.SOURCES):
        if not name.find('Test') == -1:
            res.append(name[:-3])
    return res

def show():
    print('===> test sources')
    for s in gatherSources():
        print(s)

class TestMainFile:
    def __init__(self, files):
        self.files = files
    
    def multiple(files):
        return TestMainFile(files)
    
    def single(file):
        return TestMainFile([file])

    def __str__(self):
        TEMPLATE  = """
import sys
sys.path.append('{}')
{}

import unittest
if __name__ == '__main__':
    unittest.main()
        """
        imports = ['from {} import *'.format(filename) for filename in self.files]
        return TEMPLATE.format(Paths.SOURCES, '\n'.join(imports))
    
    def print(self, path):
        with open(path, 'w') as stream:
            stream.write(str(self))

class CMakeLists:
    def __init__(self, binPath):
        self.tests = []
        self.copySoDep_(binPath)

    def copySoDep_(self, binPath):
        libName = os.path.basename(binPath)
        with open(os.path.join(Paths.DEST, '__copy_so__.py'), 'w') as stream:
            content = """
import shutil, os

SRC='{}'
TRG='{}'

if os.path.exists(TRG):
  os.remove(TRG)
shutil.copy(SRC, TRG) 
            """.format(binPath, os.path.join(Paths.DEST, libName))
            stream.write(content)

        with open(os.path.join(Paths.DEST, 'CMakeLists.txt'), 'w') as stream:
                cmakeCmd = """
add_custom_target(__copy_so__ ALL 
    COMMAND "{}" "{}"
    WORKING_DIRECTORY {}
    DEPENDS {}
)      
                """.format('${Python3_EXECUTABLE}', '__copy_so__.py', '${CMAKE_CURRENT_SOURCE_DIR}', '${WRAPPER_NAME}')
                stream.write(cmakeCmd)

    def add(self, target_name, filename, src : TestMainFile):
        self.tests.append((
            target_name,
            os.path.join(Paths.DEST, filename),
            src
        ))

    def print(self):
        with open(os.path.join(Paths.DEST, 'CMakeLists.txt'), 'a') as stream:
            for target, filename, source in self.tests:
                source.print(filename)
                sources = [os.path.join(Paths.SOURCES, '{}.py'.format(name)) for name in source.files]
                cmakeCmd = """
add_custom_target({} ALL 
    COMMAND "{}" "{}"
    WORKING_DIRECTORY {}
    DEPENDS __copy_so__
    SOURCES {}
)           
                """.format(target, '${Python3_EXECUTABLE}', filename, '${CMAKE_CURRENT_SOURCE_DIR}', ' '.join(sources))
                stream.write(cmakeCmd)

def terraform(args):
    print('-- ===> terraforming into {}'.format(Paths.DEST))
    os.makedirs(Paths.DEST, exist_ok=True)
    cases = gatherSources()
    cmake = CMakeLists(args.bin)
    cmake.add('Tests', 'main.py', TestMainFile.multiple(cases))
    for c in cases:
        cmake.add(c, 'main_{}.py'.format(c), TestMainFile.single(c))
    cmake.print()

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--mode", default='show',help='possibilities:[show terraform]')
    parser.add_argument("-b", "--bin", default=None, help='.so location')
    args = parser.parse_args()

    if args.mode == 'show':
        show()
    elif args.mode == 'terraform':
        if args.bin == None:
            raise Exception('location of .so library was not specified')
        terraform(args)
