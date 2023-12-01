import argparse, os, sys, shutil, re, subprocess
import unittest

class Path:
    def __init__(self, path):
        preamble, self.extension = os.path.splitext(path)
        self.name = os.path.basename(preamble)
        self.parent = os.path.dirname(preamble)

    def filename(self):
        return '{}{}'.format(self.name, self.extension)

def copySo(source):
    print('===> copying *.so from {}'.format(source))
    shutil.copyfile(source, Path(source).filename())

def scanTests(folder):
    fltr = filter(lambda f: re.match(r"(.*)?Test.py", f), os.listdir(folder))
    return [os.path.join(folder, f) for f in fltr]

def runTests(tests):
    print('===> running tests from:\n{}'.format('\n'.join(tests)))

    def makeTestsLines(test):
        info = Path(test)
        return """
sys.path.append('{}')
from {} import *
     """.format(info.parent, info.name)

    src="""
import sys

{}

import unittest

if __name__ == '__main__':
    unittest.main()
""".format('\n'.join([makeTestsLines(t) for t in tests]) )
    
    tmpLocation = 'tmp.py'

    with open(tmpLocation, 'w') as stream:
        stream.write(src)

    hndlr = subprocess.Popen(['python3', tmpLocation], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    out, err = hndlr.communicate()
    if not hndlr.returncode == 0:
        msg = 'Something went wrong:\n{}'.format(err)
        raise Exception(msg)
    print('{}\n{}'.format(out, err))

    os.remove(tmpLocation)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--copy", default=None, help='copy *.so library before execution')
    parser.add_argument("-t", "--test", default=None, help='run tests found in specified file. If the file is a folder, runs all tests in that folder')
    args = parser.parse_args()

    if not args.copy == None:
        copySo(args.copy)

    if args.test == None:
        path = os.path.dirname(__file__)
        tests = scanTests(path)
    elif os.path.isdir(args.test):
        tests = scanTests(args.test)
    else:
        tests = [args.test]

    runTests(tests)

if __name__ == '__main__':
    main()